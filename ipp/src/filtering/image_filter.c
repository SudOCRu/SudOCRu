#include "image_filter.h"
#include "filters.h"
#include "edge_detection.h"
#include "../transform/transform.h"
#include "../utils.h"

void FilterImage(Image* img, u32* tmp, int flags)
{
    u8 s = 1, t = 7;

    PrintStage(s, t, "Grayscale filter", 0);
    u8 min = 255, max = 0;
    GrayscaleFilter(img, &min, &max);
    PrintStage(s++, t, "Grayscale filter", 1);

    PrintStage(s, t, "Contrast stretching", 0);
    printf(" --> Min/Max: %hhu/%hhu", min, max);
    StretchContrast(img, min, max);
    PrintStage(s++, t, "Contrast stretching", 1);

    if ((flags & SC_FLG_DGRS) != 0 && SaveImageFile(img, "grayscale.png"))
        printf("Successfully saved grayscale.png\n");

    PrintStage(s, t, "Median Filter (5x5)", 0);
    MedianFilter(img, tmp, 5);
    PrintStage(s++, t, "Median Filter (5x5)", 1);

    PrintStage(s, t, "Bilateral Filter (11x11)", 0);
    BilateralFilter(img, tmp, 11, 55, 55);
    PrintStage(s++, t, "Bilateral Filter (11x11)", 1);

    if ((flags & SC_FLG_DMED) != 0 && SaveImageFile(img, "smoothed.png"))
        printf("Successfully saved smoothed.png\n");

    /*
    PrintStage(s, t, "Erode (3x3)", 0);
    Dilate(img, tmp, 3); // Actually Erode because image is not inverted
    PrintStage(s++, t, "Erode (3x3)", 1);
    */
}

void BinarizeImage(Image* img, u32* tmp, float threshold)
{
    PrintStage(1, 2, "Adaptive Thresholding (15x15)", 0);
    // normal threshold: 2, optimal: 7%
    AdaptiveThresholding(img, tmp, 15, threshold);
    PrintStage(1, 2, "Adaptive Thresholding (15x15)", 1);

    PrintStage(2, 2, "Dilate (3x3)", 0);
    Dilate(img, tmp, 3);
    PrintStage(2, 2, "Dilate (3x3)", 1);
}

typedef struct Component {
    u8 id;
    size_t size;
} Component;

#define CheckNeighbour(x, y) (img->pixels[(y) * img->width + (x)] > 0\
        && markers[(y) * img->width + (x)] == 0)

void FindConnectedPixels(Image* img, u8* markers, size_t x, size_t y,
        Component* comp)
{
    markers[y * img->width + x] = comp->id;
    comp->size += 1;

    if (x > 0 && CheckNeighbour(x - 1, y))
        FindConnectedPixels(img, markers, x - 1, y, comp);

    if (x < img->width - 1 && CheckNeighbour(x + 1, y))
        FindConnectedPixels(img, markers, x + 1, y, comp);

    if (y > 0 && CheckNeighbour(x, y - 1))
        FindConnectedPixels(img, markers, x, y - 1, comp);

    if (y < img->height-1 && CheckNeighbour(x, y + 1))
        FindConnectedPixels(img, markers, x, y + 1, comp);
}

int CleanCell(Image* img, u8* markers)
{
    // Apply a circle mask to remove leftover board lines
    float r_squared = pow(min(img->width * 0.33f, img->height * 0.33f), 2);
    float midX = img->width / 2.0f, midY = img->height / 2.0f;
    for (size_t y = 0; y < img->height; y++)
    {
        for (size_t x = 0; x < img->width; x++)
        {
            if (pow(x - midX, 2) + pow(y - midY, 2) > r_squared)
                img->pixels[y * img->width + x] = 0;
            else
                img->pixels[y * img->width + x] &= 0xFFFFFF;
        }
    }

    size_t capacity = 4;
    size_t nb_comp = 0;
    size_t largest = 1;
    memset(markers, 0, img->width * img->height * sizeof(u8));
    Component** components = malloc(capacity * sizeof(Component*));

    // Find all the connected components in the image and the largest component
    for (size_t y = 0; y < img->height; y++)
    {
        for (size_t x = 0; x < img->width; x++)
        {
            if (CheckNeighbour(x, y))
            {
                Component* cur = calloc(1, sizeof(Component));
                components[nb_comp++] = cur;
                cur->id = nb_comp;
                FindConnectedPixels(img, markers, x, y, cur);

                if (cur->size > components[largest - 1]->size)
                    largest = nb_comp;

                if (nb_comp == capacity)
                {
                    capacity *= 2;
                    components = realloc(components, capacity *
                            sizeof(Component*));
                }
            }
        }
    }

    if (nb_comp == 0)
    {
        free(components);
        return 0;
    }

    // keep all components that are up to 50% smaller than the largest component
    size_t target = components[largest - 1]->size * 35 / 100;
    size_t total_size = 0;
    size_t count = 0;
    for (size_t i = 0; i < nb_comp; i++)
    {
        if (components[i]->size >= target)
        {
            total_size += components[i]->size;
            count++;
        }
    }

    // The total number of pixels within the circle must be >12.5% of the total
    // area in order to mark this cell as not empty.
    //printf("components: %lu/%lu\n", count, nb_comp);
    //printf("fill rate: %lu (%f%%)\n", total_size,
    //        (total_size / (M_PI * r_squared)) * 100.0f);
    if ((total_size / (M_PI * r_squared) > 0.10f))
    {
        // Component decimation: remove any component not large enough
        for (size_t y = 0; y < img->height; y++)
        {
            for (size_t x = 0; x < img->width; x++)
            {
                u8 id = markers[y * img->width + x];
                if (id > 0 && (components[id - 1]->size >= target))
                {
                    // use the line below for better visulization
                    // img->pixels[y * img->width + x] = id * 0xFFFFFF/nb_comp;
                }
                else
                {
                    markers[y * img->width + x] = 0;
                    img->pixels[y * img->width + x] = 0;
                }
            }
        }
        free(components);
        return 1;
    }
    free(components);
    return /*(total_size / (M_PI * r_squared) > 0.05f)*/0;
}

Image* PrepareCell(const Image* cell, const u8* markers) {
    size_t min_x = cell->width - 1, min_y = cell->height - 1,
           max_x = 0, max_y = 0;
    // Find the smallest box containg all the components
    for (size_t y = 0; y < cell->height; y++)
    {
        for (size_t x = 0; x < cell->width; x++)
        {
            if (markers[y * cell->width + x] != 0)
            {
                min_x = min(min_x, x);
                min_y = min(min_y, y);
                max_x = max(max_x, x);
                max_y = max(max_y, y);
            }
        }
    }

    // Make the cropped image a square
    size_t new_width = max_x - min_x;
    size_t new_height = max_y - min_y;
    if (new_width != new_height)
    {
        if (new_width > new_height)
        {
            size_t diff = new_width - new_height;
            max_y += diff - diff / 2;
            min_y -= diff / 2;
        } else {
            size_t diff = new_height - new_width;
            max_x += diff - diff / 2;
            min_x -= diff / 2;
        }
    }

    Image* r = DownscaleImage(cell, min_x, min_y, max_x, max_y, 28, 28, 0);
    for (size_t i = 0; i < 28 * 28; i++)
    {
        unsigned char val = r->pixels[i];
        r->pixels[i] = (val << 16) | (val << 8) | val;
    }
    return r;
}

void GrayscaleFilter(Image* image, u8* min, u8* max)
{
    size_t len = image->width * image->height;
    for (size_t i = 0; i < len; i++)
    {
        u32 c = image->pixels[i];
        u8 red = (c >> 16) & 0xFF, green = (c >> 8) & 0xFF, blue = c & 0xFF;
        u8 g = 0.299F * red + 0.587F * green + 0.114F * blue;

        // Prepapre for next stage: Contrast stretching
        if (g > *max) *max = g;
        else if (g < *min) *min = g;

        image->pixels[i] = g | (g << 8) | (g << 16);
    }
}

void StretchContrast(Image* img, u8 min, u8 max)
{
    if (min == 0 && max == 255) return;

    size_t len = img->width * img->height;
    for (size_t i = 0; i < len; i++)
    {
        u8 col = img->pixels[i] & 0xFF;
        col = ((col - min) * 255) / (max - min);
        img->pixels[i] = (col << 16) | (col << 8) | col;
    }
}

void AutoBrigthness(Image* image, float clip) {
    u32 hist[256] = { 0, };
    FillHistogram(image, hist);

    u32 acc[256] = { 0, };
    acc[0] = hist[0];
    for (size_t i = 1; i < 256; i++)
        acc[i] = acc[i - 1] + hist[i];

    float threshold = clip * 0.5 * acc[255];
    u8 min = 0;
    while (min < 255 && acc[min] < threshold) min++;

    threshold = (1 - clip) * 0.5 * acc[255];
    u8 max = 255;
    while (max > 0 && acc[max] >= threshold) max--;

    size_t len = image->width * image->height;
    float alpha = 255 / (max - min);
    float beta = -min * alpha;

    for (size_t i = 0; i < len; i++)
    {
        float col = alpha * (float)(image->pixels[i] & 0xFF) + beta;
        u8 c = col > 255 ? 255 : (col < 0 ? 0 : (u8)col);

        image->pixels[i] = (c << 16) | (c << 8) | c;
    }
}

void FillHistogram(const Image* image, u32 histogram[256])
{
    for (size_t i = 0; i < 256; i++)
        histogram[i] = 0;
    size_t len = image->width * image->height;

    for (size_t i = 0; i < len; i++)
    {
        u8 c = image->pixels[i] & 0xFF;
        histogram[c]++;
    }
}

u8 ComputeOtsuThreshold(size_t len, const u32 histogram[256])
{
    u32 sum = 0, sumP = 0;
    for (size_t i = 0; i < 256; i++)
        sum += histogram[i] * i;

    u32 threshold = 0, var_max = 0, q1 = 0, q2 = 0, u1 = 0, u2 = 0;
    for (size_t t = 0; t < 256; t++)
    {
        q1 += histogram[t];
        if (q1 == 0) continue;
        q2 = len - q1 + 1;

        sumP += histogram[t] * t;
        u1 = sumP / q1;
        u2 = (sum - sumP) / q2;

        u32 var = q1 * q2 * (u1 - u2) * (u1 - u2);

        if (var > var_max)
        {
            threshold = t;
            var_max = var;
        }
    }

    return threshold;
}

void AdaptiveThresholding(Image* img, u32* buf, size_t r, float threshold)
{
    size_t w = img->width, h = img->height;
    memset(buf, 0, w * h * sizeof(u32));
    size_t side = r / 2;
    for (size_t y = side; y < h - side; y++)
    {
        for (size_t x = side; x < w - side; x++)
        {
            size_t sum = 0;
            size_t endX = x + side;
            size_t endY = y + side;
            for (size_t dy = y - side; dy <= endY; dy++)
            {
                for (size_t dx = x - side; dx <= endX; dx++)
                {
                    sum += img->pixels[dy * w + dx] & 0xFF;
                }
            }
            float m = threshold > 1 ? (sum / (float)(r * r)) - threshold :
                (sum / (float)(r * r)) * (1.0 - threshold);
            buf[y * w + x] = (img->pixels[y * w + x] & 0xFF) >= m ?
                0 : 0xFFFFFF;
        }
    }

    memcpy(img->pixels, buf, w * h * sizeof(u32));
}

void ThresholdImage(Image* image, u8 threshold)
{
    size_t len = image->width * image->height;

    for (size_t i = 0; i < len; i++)
    {
        u32 c = image->pixels[i];
        image->pixels[i] = (c & 0xFF) > threshold ? 0 : 0xFFFFFF;
    }
}

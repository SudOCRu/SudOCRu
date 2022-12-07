#include "filters.h"
#include "utils.h"

void FreeKernel(Kernel* kernel)
{
    if (kernel == NULL)
        return;
    free(kernel->vals);
    free(kernel);
}

void PrintKernel(const Kernel* kernel)
{
    printf("<%hhux%hhu>[\n", kernel->radius, kernel->radius);
    float sum = 0;
    for (size_t i = 0; i < kernel->radius; i++)
    {
        printf("%f", kernel->vals[0]);
        for (size_t j = 1; j < kernel->radius; j++)
        {
            float c = kernel->vals[i * kernel->radius + j];
            sum += c;
            printf(", %f", c);
        }
        printf("\n");
    }
    printf("] (sum = %f)\n", sum);
}

void array_insert(u8* begin, u8* end, u8 val)
{
    for(; end > begin && val < *(end - 1); end--)
        *end = *(end - 1);
    *end = val;
}

void Invert(Image* img)
{
    size_t l = img->width * img->height;
    for (size_t i = 0; i < l; i++)
        img->pixels[i] = 255 - (img->pixels[i] & 0xFF);
}

void Convolve(const Image* img, u32* out, const Kernel* ker)
{
    ssize_t w = img->width, h = img->height;

    ssize_t s = ker->radius / 2;
    for (ssize_t y = 0; y < h; y++)
    {
        for (ssize_t x = 0; x < w; x++)
        {
            float sum = 0;
            for (ssize_t dy = -s; dy <= s; dy++)
            {
                ssize_t ey = clamp(y + dy, 0, h);
                for (ssize_t dx = -s; dx <= s; dx++)
                {
                    ssize_t ex = clamp(x + dx, 0, w);
                    sum += (img->pixels[ey * w + ex] & 0xFF) *
                        ker->vals[(dy + s) * ker->radius + (dx + s)];
                }
            }
            u8 col = (sum > 255) ? 255 : ((sum < 0) ? 0 : sum);
            out[y * w + x] = (col << 16) | (col << 8) | col;
        }
    }
}

Kernel* GetGaussianKernel(float sigma, size_t r)
{
    size_t ksize = r * r;
    float* vals = malloc(ksize * sizeof(float));
    Kernel* ker = malloc(sizeof(Kernel));
    ker->radius = r;
    ker->vals = vals;

    float sig2 = 2.0 * sigma * sigma;
    float a = 1.0 / (M_PI * sig2);
    float sum = 0;
    ssize_t side = r / 2;
    for (ssize_t dy = -side; dy <= side; dy++)
    {
        for (ssize_t dx = -side; dx <= side; dx++)
        {
            float v = a * exp(-((float)(dx*dx+dy*dy) / sig2));
            sum += v;
            vals[(dy + side) * r + (dx + side)] = v;
        }
    }

    for (size_t i = 0; i < ksize; i++)
        vals[i] /= sum;
    return ker;
}

void GaussianBlur(Image* img, u32* buf, float sigma, size_t r)
{
    size_t w = img->width, h = img->height;
    Kernel* ker = GetGaussianKernel(sigma, r);
    Convolve(img, buf, ker);
    memcpy(img->pixels, buf, w * h * sizeof(u32));
    free(ker);
}

void MeanFilter(Image* img, u32* buf, size_t r)
{
    size_t w = img->width, h = img->height;
    size_t ksize = r * r;

    Kernel* ker = malloc(sizeof(Kernel));
    ker->radius = r;
    ker->vals = malloc(ksize * sizeof(float));
    for (size_t i = 0; i < ksize; i++)
        ker->vals[i] = 1.0f / ksize;

    Convolve(img, buf, ker);
    memcpy(img->pixels, buf, w * h * sizeof(u32));
    free(ker);
}

void BilateralFilter(Image* img, u32* buf, size_t r, float col, float sp)
{
    float col2 = 2 * col * col;
    float sp2 = 2 * sp * sp;
    float b = M_PI * M_PI * col2 * sp2; // Precalculate the quotient

    ssize_t w = img->width, h = img->height;
    ssize_t m = r / 2;
    for (ssize_t y = 0; y < h; y++)
    {
        for (ssize_t x = 0; x < w; x++)
        {
            float sum = 0;
            float wf = 0;
            float ic = img->pixels[y * w + x] & 0xFF;
            for (ssize_t dy = -m; dy <= m; dy++)
            {
                ssize_t ey = clamp(y + dy, 0, h);
                for (ssize_t dx = -m; dx <= m; dx++)
                {
                    ssize_t ex = clamp(x + dx, 0, w);
                    float i = img->pixels[ey * w + ex] & 0xFF;
                    float dl = dx * dx + dy * dy;
                    float gauss = exp(-(dl/sp2) - (pow(i - ic, 2))/col2) / b;
                    wf += gauss;
                    sum += i * gauss;
                }
            }

            u8 c = sum / wf;
            buf[y * w + x] = (c << 16) | (c << 8) | c;
        }
    }

    memcpy(img->pixels, buf, w * h * sizeof(u32));
}

void MedianFilter(Image* img, u32* buf, size_t block)
{
    ssize_t w = img->width, h = img->height;
    ssize_t m = block / 2;
    size_t mid = block * block / 2;
    u8* vals = calloc(block * block, sizeof(u8));
    for (ssize_t y = 0; y < h; y++)
    {
        for (ssize_t x = 0; x < w; x++)
        {
            size_t i = 0;
            for (ssize_t dy = -m; dy <= m; dy++)
            {
                ssize_t ey = clamp(y + dy, 0, h);
                for (ssize_t dx = -m; dx <= m; dx++)
                {
                    ssize_t ex = clamp(x + dx, 0, w);
                    u8 col = img->pixels[ey * w + ex] & 0xFF;

                    array_insert(vals, vals + i, col);
                    i++;
                }
            }

            u8 c = vals[mid];
            buf[y * w + x] = (c << 16) | (c << 8) | c;
        }
    }

    memcpy(img->pixels, buf, w * h * sizeof(u32));
    free(vals);
}

void Dilate(Image* img, u32* buf, size_t block)
{
    ssize_t w = img->width, h = img->height;
    ssize_t s = block / 2;
    for (ssize_t y = 0; y < h; y++)
    {
        for (ssize_t x = 0; x < w; x++)
        {
            u8 ma = 0;
            for (ssize_t dy = -s; dy <= s; dy++)
            {
                ssize_t ey = clamp(y + dy, 0, h);
                for (ssize_t dx = -s; dx <= s; dx++)
                {
                    ssize_t ex = clamp(x + dx, 0, w);
                    ma = max(ma, (img->pixels[ey * w + ex] & 0xFF));
                }
            }
            buf[y * w + x] = (ma << 16) | (ma << 8) | ma;
        }
    }

    memcpy(img->pixels, buf, w * h * sizeof(u32));
}

void Erode(Image* img, u32* buf, size_t block)
{
    ssize_t w = img->width, h = img->height;
    ssize_t s = block / 2;
    for (ssize_t y = 0; y < h; y++)
    {
        for (ssize_t x = 0; x < w; x++)
        {
            u8 mi = 255;
            for (ssize_t dy = -s; dy <= s; dy++)
            {
                ssize_t ey = clamp(y + dy, 0, h);
                for (ssize_t dx = -s; dx <= s; dx++)
                {
                    ssize_t ex = clamp(x + dx, 0, w);
                    mi = min(mi, (img->pixels[ey * w + ex] & 0xFF));
                }
            }
            buf[y * w + x] = (mi << 16) | (mi << 8) | mi;
        }
    }

    memcpy(img->pixels, buf, w * h * sizeof(u32));
}

void GammaFilter(Image* img, float f)
{
    size_t len = img->width * img->height;

    for (size_t i = 0; i < len; i++)
    {
        float c = 255.0f * pow((float)(img->pixels[i] & 0xFF) / 255.0f, f);
        u8 g = clamp(c, 0.0, 256.0);
        img->pixels[i] = (g << 16) | (g << 8) | g;
    }
}

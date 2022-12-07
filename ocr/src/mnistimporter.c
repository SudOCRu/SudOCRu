#include "mnistimporter.h"

#define IMG_LEN 28*28

unsigned char* readImage(FILE *file){
    unsigned char *data = calloc(IMG_LEN, sizeof(unsigned char));
    int t = fread(data, 1, IMG_LEN, file);
    return data;
}

unsigned char readLabel(FILE *file){
    unsigned char data = 0;
    int t =fread(&data, 1, sizeof(data), file);
    return data;
}

double** readImages(FILE *file, size_t nbImg){
    double **images = calloc(nbImg, sizeof(double*));
    for (size_t i = 0; i < nbImg; i++){
        unsigned char *image = readImage(file);
        images[i] = calloc(IMG_LEN, sizeof(double));
        for (size_t j = 0; j < IMG_LEN; j++){
            (images[i])[j] = (double)image[j]/255;
        }
    }
    return images;
}

unsigned char* readLabels(FILE *file, size_t nbImg){
    unsigned char *labels = calloc(nbImg, sizeof(unsigned char));
    for (size_t i = 0; i < nbImg; i++){
        labels[i] = readLabel(file);
    }
    return labels;
}

DataPoint **generateSample(char *dataFileName, char *resultsFileName, int *nbImg){
    FILE *data = fopen(dataFileName, "rb");
    int *skip = malloc(sizeof(int));
    int t = fread(skip, sizeof(int), 1, data);
    t = fread(nbImg, sizeof(int), 1, data);
    t = fread(skip, sizeof(int), 1, data);
    t = fread(skip, sizeof(int), 1, data);
    FILE *results = fopen(resultsFileName, "rb");
    t = fread(skip, sizeof(int), 1, results);
    t = fread(skip, sizeof(int), 1, results);

    unsigned char *labels = readLabels(results, *nbImg);
    double **images = readImages(data, *nbImg);

    DataPoint **sample = calloc(*nbImg, sizeof(DataPoint));
    for (size_t i = 0; i < *nbImg; i++){
       double *outputs = calloc(10, sizeof(double));
       for(size_t j = 0; j < 10; j++){
           outputs[j] = 0;
       }
       outputs[labels[i]] = 1;

       DataPoint *data = CreateDatapoint(images[i], outputs);
       sample[i] = data;
    }

    return sample;
}

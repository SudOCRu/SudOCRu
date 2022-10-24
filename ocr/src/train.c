#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <err.h>
//#include "image.h"
#include "neuralnetwork.h"
#include "train.h"

#define NUM_THREADS 10000
#define LABEL_FILE "./train-labels.idx1-ubyte"
#define IMAGE_FILE "./train-images.idx3-ubyte"
#define IMAGE_SIZE 28
#define TRAIN_COUNT 10000
#define LEARN_ITERATION 1000000

void Convert(unsigned int *value){
    unsigned int b0,b1,b2,b3;
    b0 = (*value & 0x000000ff) << 24u;
    b1 = (*value & 0x0000ff00) << 8u;
    b2 = (*value & 0x00ff0000) >> 8u;
    b3 = (*value & 0xff000000) >> 24u;
    *value = b0 | b1 | b2 | b3;
}

unsigned char *readLabels(char *name){
    FILE *file;
    if ((file = fopen(name, "rb")) == NULL){
        printf("Couldn't read file...\n");
        return NULL;
    }

    unsigned int magic;
    unsigned int size;

    int re = fread(&magic, sizeof(unsigned int), 1, file);
    re = fread(&size, sizeof(unsigned int), 1, file);

    Convert(&magic);
    Convert(&size);

    unsigned char *labels = calloc(size, sizeof(unsigned char));

    re = fread(labels, size, 1, file);
    printf("Finished reading labels file\n");
    fclose(file);
    return labels;
}

unsigned char *readImages(char *name){
    FILE *file;
    if ((file = fopen(name, "rb")) == NULL){
        printf("Couldn't read file...\n");
        return NULL;
    }

    unsigned int magic;
    unsigned int size;
    unsigned int rows;
    unsigned int columns;

    int re = fread(&magic, sizeof(unsigned int), 1, file);
    re = fread(&size, sizeof(unsigned int), 1, file);
    re = fread(&rows, sizeof(unsigned int), 1, file);
    re = fread(&columns, sizeof(unsigned int), 1, file);

    Convert(&magic);
    Convert(&size);
    Convert(&rows);
    Convert(&columns);

    printf("size=%u\n",size);
    printf("rows=%u\n",rows);
    printf("columns=%u\n",columns);

    unsigned char *images = calloc(rows*columns*size, sizeof(unsigned char));

    int bufferSize = rows*columns;

    for (int j = 0; j < size/bufferSize; j++){
        unsigned char buffer[bufferSize];
        re = fread(buffer, sizeof(buffer), 1, file);
        for (int i = 0; i < bufferSize; i++){
            images[bufferSize*j+i] = buffer[i];
        }
    }
    printf("Finished reading images file\n");
    fclose(file);
    return images;
}

DataPoint **MNISTData(){
    unsigned char *labels = readLabels(LABEL_FILE);
    unsigned char *images = readImages(IMAGE_FILE);
    int pixelCount = IMAGE_SIZE * IMAGE_SIZE;

    DataPoint **dataSet = calloc(TRAIN_COUNT, sizeof(DataPoint));

    for (int i = 0; i < TRAIN_COUNT; i++){
        double *pixels = calloc(pixelCount, sizeof(double));
        for (int j = 0; j < pixelCount; j++){
            pixels[j] = 1-(double)images[pixelCount*i+j]/255;
        }
        double *number = calloc(10, sizeof(double));
        for (int j = 0; j < 10; j++){
            number[j] = 0;
        }
        number[labels[i]] = 1;
        DataPoint *data = CreateDatapoint(pixels, number);
        dataSet[i] = data;
    }
    free(labels);
    free(images);
    return dataSet;
}

typedef struct LearnData{
    NeuralNetwork *network;
    DataPoint **dataSet;
    int dataLength;
    double learnRate;
} LearnData;

void *LearnThread(void *data){
    LearnData *learnData = (LearnData*)data;
    Learn(learnData->network, learnData->dataSet, learnData->dataLength, learnData->learnRate);
    printf("finished learning\n");
    pthread_exit(NULL);
}

void SaveNetwork(NeuralNetwork *network){

}

int GetMaxIndex(double *data, int size);

void ReadNetwork(NeuralNetwork *network){
    FILE *file;
    if ((file = fopen("./network", "rb")) == NULL){
        printf("Error ! Couldn't read file\n");
        return;
    }
    for (size_t i = 0; i < network->arrayLayerLength; i++){
        Layer *layer = network->layers[i];
        double *weights = calloc(layer->numNodesIn*layer->numNodesOut, sizeof(double));
        double *biases = calloc(layer->numNodesOut, sizeof(double));
        printf("sizeof=%i\n", layer->numNodesIn*layer->numNodesOut);
        int t = fread(weights, layer->numNodesIn*layer->numNodesOut, sizeof(double), file);
        t = fread(biases, layer->numNodesOut, sizeof(double), file);
        for (size_t j = 0; j < layer->numNodesIn*layer->numNodesOut; j++){
            layer->weights[j] = weights[j];
        }
        for (size_t j = 0; j < layer->numNodesOut; j++){
            layer->biases[j] = biases[j];
        }
    }
}

DataPoint **GenerateXorData(){
    DataPoint **sample = calloc(4, sizeof(DataPoint));
    double *True = malloc(sizeof(double));
    double *False = malloc(sizeof(double));
    True[0] = 1;
    False[0] = 0;
    
    double *t1 = calloc(2, sizeof(double));
    t1[0] = 0;
    t1[1] = 0;
    double *t2 = calloc(2, sizeof(double));
    t2[0] = 1;
    t2[1] = 0;
    double *t3 = calloc(2, sizeof(double));
    t3[0] = 0;
    t3[1] = 1;
    double *t4 = calloc(2, sizeof(double));
    t4[0] = 1;
    t4[1] = 1;

    
    DataPoint *data1 = CreateDatapoint(t1, False);
    DataPoint *data2 = CreateDatapoint(t2, True);
    DataPoint *data3 = CreateDatapoint(t3, True);
    DataPoint *data4 = CreateDatapoint(t4, False);

    sample[0] = data1;
    sample[1] = data2;
    sample[2] = data3;
    sample[3] = data4;

    return sample;
}

void Train(NeuralNetwork *network, DataPoint **trainingSample, int trainingSize){
    for (size_t i = 0; i < LEARN_ITERATION; i++){
        Learn(network, trainingSample, trainingSize, 1);
    }

    FILE *file;
    if ((file = fopen("./network", "wb")) == NULL){
        printf("Error ! Couldn't read file\n");
        return;
    }

    for (size_t i = 0; i < network->arrayLayerLength; i++){
        Layer *layer = network->layers[i];
        double *weights = layer->weights;
        double *biases = layer->biases;
        fwrite(weights, layer->numNodesIn*layer->numNodesOut, sizeof(double), file);
        fwrite(biases, layer->numNodesOut, sizeof(double), file);
    }

    fclose(file);
}

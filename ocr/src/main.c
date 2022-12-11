#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include "datapoint.h"
#include "neuralnetwork.h"
#include "mnistimporter.h"


int GetMaxIndex(double* data, int size){
    int max = 0;
    for (int i = 1; i < size; i++){
        if (data[i] > data[max]){
            max = i;
        }
    }
    return max;
}

DataPoint *gotten(char *fileName){
    FILE *file = fopen(fileName, "rb");
    double *data = calloc(28*28,sizeof(double));
    int t = fread(data, 28*28*sizeof(double), 1, file);

    char expected = 0;
    t = fread(&expected, sizeof(char), 1, file);
    double *expectedOutputs = calloc(10, sizeof(double));
    for (size_t i = 0; i < 10; i++){
        expectedOutputs[i] = 0;
    }
    expectedOutputs[expected] = 1;

    DataPoint *dataPoint = CreateDatapoint(data, expectedOutputs);
    return dataPoint;
}

int main(int argc, char *argv[])
{
    int layerStructure[] = {28*28, 50, 50, 10};
    NeuralNetwork *network = NULL;
    if (access("network", F_OK) == 0)
        network = ReadNetwork("network");
    else
        network = CreateNeuralNetwork(layerStructure, 4);
    
    if (argc < 2){
        printf("not enough args\n");
        return -1;
    }

    if (!strcmp(argv[1], "eval")){
        DataPoint *data = gotten("data");
        double *output = ProcessOutputs(network, data->inputs);
        int result = 0;
        for (int i = 1; i < 10; i++){
            if (output[i] > output[result]){
                result = i;
            }
            printf("Output%i = %lf\n",i,output[i]);
        }
        printf("\n");
        //printf("result = %i\n",result);
        DestroyNeuralNetwork(network);
        DestroyDatapoint(data);
        return result;
    } else if (!strcmp(argv[1], "train")){
        int nbImg = 0;
        DataPoint **sample = generateSample("training", "labels", &nbImg);
        long nbr = 100;
        if (argc == 3){
            nbr = strtol(argv[2], NULL, 10);
        }
        for (long i = 0; i < nbr; i++){
            Learn(network, sample, nbImg, 0.05);
            printf("i=%li\n",i);
        }
        SaveNetwork(network, "network");
        printf("network saved.\n");
        DestroyNeuralNetwork(network);
        for (size_t i = 0; i < 10; i++){
            DestroyDatapoint(sample[i]);
        }
        free(sample);
    } else {
        printf("wrong argument\n");
        return -1;
    }
    return 0;
}

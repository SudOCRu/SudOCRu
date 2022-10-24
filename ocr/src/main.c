#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <time.h>
#include "datapoint.h"
#include "neuralnetwork.h"
#include "train.h"

void xor(){
    double inputs[] = {0, 0};
    double expected[] = {0};
    double inputs2[] = {0, 1};
    double expected2[] = {1};
    double inputs3[] = {1, 0};
    double inputs4[] = {1, 1};
    DataPoint *data1 = CreateDatapoint(inputs, expected);
    DataPoint *data2 = CreateDatapoint(inputs2, expected2);
    DataPoint *data3 = CreateDatapoint(inputs3, expected2);
    DataPoint *data4 = CreateDatapoint(inputs4, expected);
    DataPoint **sample = calloc(4, sizeof(DataPoint));
    sample[0] = data1;
    sample[1] = data2;
    sample[2] = data3;
    sample[3] = data4;
    //DataPoint sample[] = {*data1, *data2, *data3, *data4};
    
    int neuralCount[] = {2, 2, 1};
    NeuralNetwork *network = CreateNeuralNetwork(neuralCount, sizeof(neuralCount)/sizeof(int));
    for (size_t i = 0; i < 10000; i++){
        Learn(network, sample, 4, 2);
    }
    
    double *results = ProcessOutputs(network, data1->inputs);
    double *results2 = ProcessOutputs(network, data2->inputs);
    double *results3 = ProcessOutputs(network, data3->inputs);
    double *results4 = ProcessOutputs(network, data4->inputs);

    printf("result=%lf\n",results[0]);
    printf("result=%lf\n",results2[0]);
    printf("result=%lf\n",results3[0]);
    printf("result=%lf\n",results4[0]);
    
    DestroyNeuralNetwork(network);
    DestroyDatapoint(data1);
}

int GetMaxIndex(double* data, int size){
    int max = 0;
    for (int i = 1; i < size; i++){
        if (data[i] > data[max]){
            max = i;
        }
    }
    return max;
}

DataPoint **GenerateXorData(){
    DataPoint **sample = calloc(4, sizeof(DataPoint));
    double *T = malloc(sizeof(double));
    double *F = malloc(sizeof(double));

    T[0] = 1;
    F[0] = 0;

    double *s1 = malloc(2*sizeof(double));
    double *s2 = malloc(2*sizeof(double));
    double *s3 = malloc(2*sizeof(double));
    double *s4 = malloc(2*sizeof(double));

    s1[0] = 0;
    s1[1] = 0;

    s2[0] = 1;
    s2[1] = 0;
    
    s3[0] = 0;
    s3[1] = 1;
    
    s4[0] = 1;
    s4[1] = 1;

    DataPoint *data1 = CreateDatapoint(s1, F);
    DataPoint *data2 = CreateDatapoint(s2, T);
    DataPoint *data3 = CreateDatapoint(s3, T);
    DataPoint *data4 = CreateDatapoint(s4, F);

    sample[0] = data1;
    sample[1] = data2;
    sample[2] = data3;
    sample[3] = data4;

    return sample;
}

void StartBrain(DataPoint **sample, int sampleSize, int layerStructure[], int size, int learnCount){
    NeuralNetwork *network = CreateNeuralNetwork(layerStructure, size);
    printf("Created neural network\n");
    for (size_t i = 0; i < learnCount; i++){
        Learn(network, sample, sampleSize-1, 1);
    }
    printf("Finished learning\n");
    
    for (int i = 0; i < sampleSize; i++){
        double* result = ProcessOutputs(network, sample[i]->inputs);
        int maxi = GetMaxIndex(result, sampleSize);
        printf("Number=%i, proba=%f\n", maxi, result[maxi]);
    }
    
    DestroyNeuralNetwork(network);
    for (int i = 0; i < sampleSize; i++){
        DestroyDatapoint(sample[i]);
    }

}

void ReadNetwork(NeuralNetwork *network);

int main()
{
    /*char* test[25] = {"samples2/0.png", "samples2/1.png", "samples2/2.png", "samples2/3.png", "samples2/4.png", "samples2/5.png", "samples2/6.png", "samples2/7.png",
                        "samples2/8.png", "samples2/9.png", "samples2/3Test.png"};
    DataPoint **sample = GenerateSampleData(test, 11);
    int layerStructure[] = {32*32, 100, 10};
    
    printf("testtest \n");

    StartBrain(sample, 11, layerStructure, sizeof(layerStructure)/sizeof(int), 200);
    //image(test);    
    //xor();*/
    
    Train();

    srand(time(NULL));

    printf("\n--------------------------------\n");

    int layerStructure[] = {28*28, 100, 100, 10};

    NeuralNetwork *network = CreateNeuralNetwork(layerStructure, 4);
    ReadNetwork(network);

    printf("layer size=%i\n",network->arrayLayerLength);

    printf("finished\n");
    return 0;
}

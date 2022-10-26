#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
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

/*DataPoint **GenerateXorData(){
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
}*/

int TrainCmd(int argc, char **argv){
    NeuralNetwork *network = NULL;
    int layerStructure[] = {2, 2, 2};
    long learnTimes;
    double learnRate;
    char *filename = NULL;
    if (argc >= 4){
        char *iCount;
        char *lRate;
        if (strcmp(argv[2], "-f") == 0){
            if (argc < 4){
                printf("Please specify file name\n.");
                return 1;
            }
            filename = argv[3];
            struct stat buffer;
            if (stat(filename, &buffer) == 0){
                network = ReadNetwork(filename);
            } else {
                printf("Neural network file doesn't exist. Creating a new one.\n");
            }
            if (argc != 6){
                printf("You have to specify learn iterations and learn rate.\n");
                return 1;
            }
            iCount = argv[4];
            lRate = argv[5];
        } else {
            iCount = argv[2];
            lRate = argv[3];
        }
        learnTimes = strtol(iCount, NULL, 10);
        learnRate = strtod(lRate, NULL);
        if (learnTimes == 0L || learnRate == 0 || errno == ERANGE){
            printf("Wrong argument: %s\n",iCount);
            return 1;
        }
    } else {
        printf("Not enought arguments !\n");
        return 1;
    } 

    if (network == NULL){
        srand(time(NULL));
        network = CreateNeuralNetwork(layerStructure, 3);
    }
    
    DataPoint **sample = GenerateXorData();
    for (long i = 0; i < learnTimes; i++){
        Learn(network, sample, 4, learnRate);
    }
    SaveNetwork(network, (filename == NULL) ? "network" : filename);
    printf("Neural network saved in:\"%s\"\n",(filename == NULL) ? "network" : filename);
    

    DestroyNeuralNetwork(network);
    for (int i = 0; i < 4; i++){
        DestroyDatapoint(sample[i]);
    }
    free(sample);
    return 0;
}

int EvalCmd(int argc, char **argv){
    char *file = argv[2];
    char *input = argv[3];
    NeuralNetwork *network = ReadNetwork(file);
    double *inputs = calloc(network->layers[0]->numNodesIn, sizeof(double));
    char *token = strtok(input, ",");
    for (int i = 0; token != NULL; i++){
        inputs[i] = atoi(token);
        token = strtok(NULL, ",");
    }

    double *output = ProcessOutputs(network, inputs);
    for (int i = 0; i < network->layers[network->arrayLayerLength-1]->numNodesOut; i++){
        printf("Output%i = %lf\n", i, output[i]);
    }

    DestroyNeuralNetwork(network);
    free(output);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc == 1){
        printf("Please use an argument.\n");
        return 1;
    }

    if (strcmp(argv[1], "train") == 0){
        return TrainCmd(argc, argv);
    } else if (argc == 4 && strcmp(argv[1], "eval") == 0){
        return EvalCmd(argc, argv);
    } else {
        printf("Not enough arguments.\n");
        return 1;
    }
    /*srand(time(NULL));
    int layerStructure[] = {2, 2, 2};
    DataPoint **sample = GenerateXorData();
    NeuralNetwork *network = CreateNeuralNetwork(layerStructure, 3);
    Train(network, sample, 4, 1);

    printf("Training completed.\n");
    
    double inputs[] = {0, 0};
    double *results = ProcessOutputs(network, inputs);
    printf("result=%lf\n",results[0]);
    printf("result=%lf\n",results[1]);

    DestroyNeuralNetwork(network);
    for (size_t i = 0; i < 4; i++){
        DestroyDatapoint(sample[i]);
        free(sample[i]);
    }
    free(sample);*/
}

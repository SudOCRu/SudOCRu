#include <stdio.h>
#include "datapoint.h"
#include "neuralnetwork.h"

int main()
{
    double inputs[] = {0.2};
    double expected[] = {0.835};
    DataPoint *data1 = CreateDatapoint(inputs, expected);
    DataPoint sample[] = {*data1};
    printf("created datapoint\n");
    int neuralCount[] = {1, 50, 50, 1};
    NeuralNetwork *network = CreateNeuralNetwork(neuralCount, sizeof(neuralCount)/sizeof(int));
    printf("Created neural network\n");
    for (size_t i = 0; i < 10000; i++){
        Learn(network, sample, 1, 0.2);
    }
    printf("Finished learning\n");
    
    double *results = ProcessOutputs(network, data1->inputs);

    printf("result=%lf\n",results[0]);
    
    DestroyNeuralNetwork(network);
    DestroyDatapoint(data1);
    return 0;
}

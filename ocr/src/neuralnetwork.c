#include "neuralnetwork.h"
#include "layer.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

LayerLearnData* CreateLayerLearnData(Layer *layer){
    LayerLearnData* learnData = malloc(sizeof(LayerLearnData));
    //learnData->inputs = calloc(layer.numNodesOut, sizeof(double));
    learnData->activations = calloc(layer->numNodesOut, sizeof(double));
    learnData->activationsLength = layer->numNodesOut;

    learnData->nodeValues = calloc(layer->numNodesOut, sizeof(double));
    learnData->nodeValuesLength = layer->numNodesOut;

    learnData->weights = calloc(layer->numNodesOut, sizeof(double));
    learnData->weightsLength = layer->numNodesOut;

    return learnData;
}

NetworkLearnData* CreateNetworkLearnData(Layer **layers, int arrayLength){
    NetworkLearnData* learnData = malloc(sizeof(NetworkLearnData));
    learnData->layerData = calloc(arrayLength, sizeof(LayerLearnData));
    learnData->layerDataLength = arrayLength;
    for (int i = 0; i < arrayLength; i++){
        learnData->layerData[i] = CreateLayerLearnData(layers[i]);
    }
    return learnData;
}

NeuralNetwork* CreateNeuralNetwork(int layerSizes[], int arrayLength){
    NeuralNetwork* network = malloc(sizeof(NeuralNetwork));
    network->layers = calloc(arrayLength-1, sizeof(Layer));
    network->arrayLayerLength = arrayLength-1;
    for (int i = 0; i < arrayLength-1; i++){
        network->layers[i] = CreateLayer(layerSizes[i], layerSizes[i+1]);
    }
    return network;
}

void DestroyLayerLearnData(LayerLearnData *learnData){
    free(learnData->activations);
    free(learnData->nodeValues);
    free(learnData->weights);
    free(learnData);
}

void DestroyNetworkLearnData(NetworkLearnData *learnData){
    for (int i = 0; i < learnData->layerDataLength; i++){
        DestroyLayerLearnData(learnData->layerData[i]);
    }
    free(learnData->layerData);
    free(learnData);
}

void DestroyNeuralNetwork(NeuralNetwork *network){
    for (int i = 0; i < network->arrayBatchLearnDataLength; i++){
        DestroyNetworkLearnData(network->batchLearnData[i]);
    }
    free(network->batchLearnData);
    for (int i = 0; i < network->arrayLayerLength; i++){
        DestroyLayer(network->layers[i]);
    }
    free(network->layers);
    free(network);
}

double *ProcessOutputs(NeuralNetwork* neuralNetwork, double inputs[]){
    for (int i = 0; i < neuralNetwork->arrayLayerLength; i++){
        inputs = CalculateOutputs(neuralNetwork->layers[i], inputs);
    }

    return inputs;
}

void LearnGradients(NeuralNetwork* neuralNetwork, DataPoint* data, NetworkLearnData* learnData){
    double *inputsToNextLayer = data->inputs;

    for (int i = 0; i < neuralNetwork->arrayLayerLength; i++){
        inputsToNextLayer = LearnOutputs(neuralNetwork->layers[i], inputsToNextLayer, learnData->layerData[i]);
    }

    int outputLayerIndex = neuralNetwork->arrayLayerLength - 1;
    Layer *outputLayer = neuralNetwork->layers[outputLayerIndex];
    LayerLearnData *outputLearnData = learnData->layerData[outputLayerIndex];

    CalculateOutputLayerNodeValues(outputLearnData, data->expectedOutputs);
    UpdateGradients(outputLayer, outputLearnData);

    for (int i = outputLayerIndex - 1; i >= 0; i--){
        LayerLearnData *layerLearnData = learnData->layerData[i];
        Layer *hiddenLayer = neuralNetwork->layers[i];

        CalculateHiddenLayerNodeValues(hiddenLayer, layerLearnData,
                neuralNetwork->layers[i+1], learnData->layerData[i+1]->nodeValues,
                learnData->layerData[i+1]->nodeValuesLength);
        UpdateGradients(hiddenLayer, layerLearnData);
    }
}

typedef struct GradientData{
    NeuralNetwork *network;
    DataPoint *trainingData;
    NetworkLearnData *batchLearnData;
} GradientData;

void *ThreadGradient(void *data){
    GradientData *gradientData = (GradientData*)data;
    LearnGradients(gradientData->network, gradientData->trainingData, gradientData->batchLearnData);
    pthread_exit(NULL);
}

void Learn(NeuralNetwork* neuralNetwork, DataPoint **trainingData, int trainingDataLength, double learnRate){
    if (!neuralNetwork->batchLearnData ||
            neuralNetwork->arrayBatchLearnDataLength != trainingDataLength){
        neuralNetwork->batchLearnData = calloc(trainingDataLength, sizeof(DataPoint));
        neuralNetwork->arrayBatchLearnDataLength = trainingDataLength;
        for (int i = 0; i < trainingDataLength; i++){
            neuralNetwork->batchLearnData[i] = CreateNetworkLearnData(neuralNetwork->layers, neuralNetwork->arrayLayerLength);
        }
    }

    pthread_t threads[trainingDataLength];
    int rc;
    
    for (int i = 0; i < trainingDataLength; i++){
        /*GradientData *gradientData = malloc(sizeof(GradientData));
        gradientData->network = neuralNetwork;
        gradientData->trainingData = trainingData[i];
        gradientData->batchLearnData = neuralNetwork->batchLearnData[i];
        rc = pthread_create(&threads[i], NULL, ThreadGradient, gradientData);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n",rc);
        }*/
        LearnGradients(neuralNetwork, trainingData[i], neuralNetwork->batchLearnData[i]);
    }

    /*for (int i = 0; i < trainingDataLength; i++){
        pthread_join(threads[i], NULL);
    }*/

    for (int i = 0; i < neuralNetwork->arrayLayerLength; i++){
        ApplyGradients(neuralNetwork->layers[i], learnRate / trainingDataLength);
    }

}

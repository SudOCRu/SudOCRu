#pragma once
#include "datapoint.h"

typedef struct LayerLearnData {
    double* inputs;
    int inputsLength;
    
    double* weights;
    int weightsLength;
    
    double* activations;
    int activationsLength;

    double* nodeValues;
    int nodeValuesLength;
} LayerLearnData;

typedef struct NetworkLearnData {
    LayerLearnData** layerData;
    int layerDataLength;
} NetworkLearnData;

typedef struct Layer {
    int numNodesIn;
    int numNodesOut;
    double* costGradientW;
    double* costGradientB;
    double* weights;
    double* biases;
} Layer;

typedef struct NeuralNetwork {
    int arrayLayerLength;
    Layer** layers;
    int arrayBatchLearnDataLength;
    NetworkLearnData** batchLearnData;
} NeuralNetwork;

Layer* CreateLayer(int numNodesIn, int numNodesOut);

NeuralNetwork* CreateNeuralNetwork(int layerSizes[], int arrayLength);

NetworkLearnData* CreateNetworkLearnData(Layer **layers, int arrayLength);

LayerLearnData* CreateLayerLearnData(Layer *layer);

void DestroyLayer(Layer* layer);

void DestroyNeuralNetwork(NeuralNetwork* network);

void DestroyNetworkLearnData(NetworkLearnData* learnData);

void DestroyLayerLearnData(LayerLearnData* learnData);


//NeuralNetwork methods

void Learn(NeuralNetwork* neuralNetwork, DataPoint **trainingData, int trainingDataLength, double learnRate);

void LearnGradients(NeuralNetwork* neuralNetwork, DataPoint* data, NetworkLearnData* learnData);

double *ProcessOutputs(NeuralNetwork* neuralNetwork, double inputs[]);

void SaveNetwork(NeuralNetwork *network, char *fileName);

NeuralNetwork *ReadNetwork(char *fileName);

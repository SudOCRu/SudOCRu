#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "layer.h"

double randfrom(double min, double max){
    double range = (max-min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

void InitializeRandomWeights(Layer* layer){
    for (int i = 0; i < layer->numNodesIn*layer->numNodesOut; i++){
        double randValue = randfrom(-1,1);
        layer->weights[i] = randValue / sqrt(layer->numNodesIn);
    }
}

Layer* CreateLayer(int numNodesIn, int numNodesOut){
    Layer* layer = malloc(sizeof(Layer));
    layer->costGradientW = calloc(numNodesIn*numNodesOut, sizeof(double));
    layer->costGradientB = calloc(numNodesOut, sizeof(double));
    layer->weights = calloc(numNodesIn*numNodesOut, sizeof(double));
    layer->biases = calloc(numNodesOut, sizeof(double));
    layer->numNodesIn = numNodesIn;
    layer->numNodesOut = numNodesOut;

    InitializeRandomWeights(layer);
    return layer;
}

void DestroyLayer(Layer *layer){
    free(layer->biases);
    free(layer->costGradientB);
    free(layer->costGradientW);
    free(layer->weights);
    //free(layer);
}

double Activate(double x){
    return 1 / (1 + exp(-x));
}

double DerivativeActivate(double x){
    double activation = Activate(x);
    return activation * (1 - activation);
}

double CostDerivative(double output, double expectedOutput){
    return 2 * (output - expectedOutput);
}

int GetFlatIndex(Layer* layer, int in, int out){
    return out * layer->numNodesIn + in;
}

double GetWeight(Layer* layer, int in, int out){
    return layer->weights[GetFlatIndex(layer, in, out)];
}

void CalculateOutputLayerNodeValues(LayerLearnData* layerLearnData, double expectedOutputs[]){
    for (int i = 0; i < layerLearnData->nodeValuesLength; i++){
        double costDerivative = CostDerivative(layerLearnData->activations[i], expectedOutputs[i]);
        double activateDerivate = DerivativeActivate(layerLearnData->weights[i]);
        layerLearnData->nodeValues[i] = costDerivative * activateDerivate;
    }
}

void ApplyGradients(Layer* layer, double learnRate){
    for(int i = 0; i < layer->numNodesIn * layer->numNodesOut; i++){
        layer->weights[i] -= layer->costGradientW[i] * learnRate;
        layer->costGradientW[i] = 0;
    }

    for (int i = 0; i < layer->numNodesOut; i++){
        layer->biases[i] -= layer->costGradientB[i] * learnRate;
        layer->costGradientB[i] = 0;
    }
}

double* LearnOutputs(Layer* layer, double inputs[], LayerLearnData* learnData){
    learnData->inputs = inputs;

    for (int out = 0; out < layer->numNodesOut; out++){
        double inputWeight = layer->biases[out];
        for (int in = 0; in < layer->numNodesIn; in++){
            inputWeight += inputs[in] * GetWeight(layer, in, out);
        }
        learnData->weights[out] = inputWeight;
    }

    for (int i = 0; i < learnData->activationsLength; i++){
        learnData->activations[i] = Activate(learnData->weights[i]);
    }

    return learnData->activations;
}

double *CalculateOutputs(Layer* layer, double* inputs){
    double inputWeights[layer->numNodesOut];
    for (int out = 0; out < layer->numNodesOut; out++){
        double inputWeight = layer->biases[out];
        for (int in = 0; in < layer->numNodesIn; in++){
            inputWeight += inputs[in] * GetWeight(layer, in, out);
        }
        inputWeights[out] = inputWeight;
    }
    
    double *activations = calloc(layer->numNodesOut, sizeof(double));
    for (int output = 0; output < layer->numNodesOut; output++){
        activations[output] = Activate(inputWeights[output]);
    }

    return activations;
}

void UpdateGradients(Layer* layer, LayerLearnData* layerLearnData){
    for (int out = 0; out < layer->numNodesOut; out++){
        double nodeValue = layerLearnData->nodeValues[out];
        for (int in = 0; in < layer->numNodesIn; in++){
            double derivativeCWW = layerLearnData->inputs[in] * nodeValue;
            layer->costGradientW[GetFlatIndex(layer, in, out)] += derivativeCWW;
        }
    }

    for (int out = 0; out < layer->numNodesOut; out++){
        layer->costGradientB[out] += layerLearnData->nodeValues[out];
    }
}

void CalculateHiddenLayerNodeValues(Layer* layer, LayerLearnData* learnData, Layer* oldLayer, double oldNodeValues[], int oldNodeValuesLength){
    for (int i = 0; i < layer->numNodesOut; i++){
        double newValue = 0;
        for (int old = 0; old < oldNodeValuesLength; old++){
            newValue += GetWeight(oldLayer, i, old) * oldNodeValues[old];
        }
        newValue *= DerivativeActivate(learnData->weights[i]);
        learnData->nodeValues[i] = newValue;
    }
}

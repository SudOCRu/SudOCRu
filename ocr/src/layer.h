#pragma once
#include "neuralnetwork.h"

void CalculateOutputLayerNodeValues(LayerLearnData* layerLearnData, double expectedOutputs[]);

void ApplyGradients(Layer* layer, double learnRate);

double* LearnOutputs(Layer* layer, double inputs[], LayerLearnData* learnData);
double* LearnOutputsOL(Layer* layer, double inputs[], LayerLearnData* learnData);

double *CalculateOutputs(Layer* layer, double inputs[]);
double *CalculateOutputsOL(Layer* layer, double inputs[]);

void UpdateGradients(Layer* layer, LayerLearnData* layerLearnData);

void CalculateHiddenLayerNodeValues(Layer* layer, LayerLearnData* learnData, Layer* oldLayer, double oldNodeValues[], int oldNodeValuesLength);

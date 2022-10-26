#pragma once
#include "datapoint.h"
#include "neuralnetwork.h"

DataPoint **GenerateXorData();

void Train(NeuralNetwork *network, DataPoint **trainingSample, int trainingSize, double learnRate);

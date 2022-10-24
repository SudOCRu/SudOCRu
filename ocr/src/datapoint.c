#include <stdlib.h>
#include "datapoint.h"

DataPoint* CreateDatapoint(double inputs[], double expectedOutputs[]){
    DataPoint* datapoint = malloc(sizeof(DataPoint));
    datapoint->inputs = inputs;
    datapoint->expectedOutputs = expectedOutputs;
    return datapoint;
}

void DestroyDatapoint(DataPoint *datapoint){
    free(datapoint->inputs);
    free(datapoint->expectedOutputs);
    free(datapoint);
}

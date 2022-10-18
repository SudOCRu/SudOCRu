#pragma once

typedef struct DataPoint {
    double* inputs;
    double* expectedOutputs;
} DataPoint;

DataPoint* CreateDatapoint(double inputs[], double expectedOutputs[]);

void DestroyDatapoint(DataPoint* datapoint);

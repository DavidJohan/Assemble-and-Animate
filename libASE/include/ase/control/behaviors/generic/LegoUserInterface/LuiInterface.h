#ifndef LUIINTERFACE_H_
#define LUIINTERFACE_H_

void applyControlOutput(signed char* outputValues, char nOutput);
int getControlInput(signed char* inputValues, char maxInputs, bool* readSuccess);
int getNumberOfInputs();
int getNumberOfOutputs();
#endif

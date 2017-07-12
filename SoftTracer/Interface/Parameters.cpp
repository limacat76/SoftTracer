#include "Parameters.h"

Parameters::Parameters() {
}

Parameters::Parameters(int tn, int tt, int wi, int he) :
	threadNumber(tn), totalThreads(tt), width(wi), height(he) {
}
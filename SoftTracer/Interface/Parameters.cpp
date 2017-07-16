#include "Parameters.h"

Parameters::Parameters() {
}

Parameters::Parameters(int tn, int tt, int wi, int he, int start_x, int width_x, int start_y, int width_y) :
	threadNumber(tn), totalThreads(tt), width(wi), height(he),
	start_x(start_x), width_x(width_x), start_y(start_y), height_y(height_y)
	{
}
#include "Parameters.h"

Parameters::Parameters() {
	;
}

Parameters::Parameters(int tn, int tt, int wi, int he, int start_x, int width_x, int start_y, int height_y) :
	threadNumber(tn), totalThreads(tt), width(wi), height(he),
	start_x(start_x), width_x(width_x), start_y(start_y), height_y(height_y)
	{
}

WorkUnit::WorkUnit() {
	;
}

WorkUnit::WorkUnit(int allocated_width, int allocated_height, int real_block_width, int real_block_height) :
	allocated_width(allocated_width), allocated_height(allocated_height), real_block_width(real_block_width), real_block_height(real_block_height) {
	;
}
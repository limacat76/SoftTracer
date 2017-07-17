#pragma once

class Parameters {
public:
	Parameters();

	Parameters(int threadNumber, int totalThreads, int width, int height, int start_x, int width_x, int start_y, int height_y);

	int totalThreads;

	int threadNumber;

	int width;

	int height;

	int start_x;

	int width_x;

	int start_y;

	int height_y;

};

class WorkUnit {
public:
	WorkUnit();

	WorkUnit(int allocated_width, int allocated_height, int real_block_width, int real_block_height);

	int allocated_width;

	int allocated_height;

	int real_block_width;

	int real_block_height;

};
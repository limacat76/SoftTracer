#pragma once

class Parameters {
public:
	Parameters();

	Parameters(int threadNumber, int totalThreads, int width, int height);

	int totalThreads;

	int threadNumber;

	int width;

	int height;
};
#pragma once
#include "Mailbox.h"

class CallableAlgorithm {
public:
	CallableAlgorithm() {
	}

	virtual ~CallableAlgorithm() {
	}

	virtual void initialize_scene() = 0;

	virtual void render(void* target, MailBox* mailbox, const void* parameters) = 0;

};

class Parameters {
public:
	Parameters();

	Parameters(int threadNumber, int totalThreads, int width, int height);

	int totalThreads;

	int threadNumber;

	int width;

	int height;
};
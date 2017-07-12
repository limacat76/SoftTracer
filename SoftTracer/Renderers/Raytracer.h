#pragma once
#include "../Interface/CallableAlgorithm.h"

class Raytracer : public CallableAlgorithm {
private:

public:
	Raytracer() {
	}

	~Raytracer() {
	}

	void initialize_scene();

	void render(void* target, void* mailbox, const void* parameters);
};
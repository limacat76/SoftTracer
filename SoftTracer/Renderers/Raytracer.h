#pragma once
#include "../Interface/CallableAlgorithm.h"

/**
* Epsilon is an arbitrarily low value
*/
// Physics
const float EPSILON = 0.0001f;

/**
* Number of times we will reflect a ray
*/
const int TRACE_DEPTH = 6;

enum IntersectionStatus {
	INSIDE = -1,
	MISS = 0,
	HIT = 1
};

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
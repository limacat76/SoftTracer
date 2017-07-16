#pragma once
#include "../Interface/WorkEngine.h"

class Raytracer : public WorkEngine {
private:

public:
	Raytracer();

	~Raytracer();

	void initialize_scene(const void* parameters);

	void render(void* target, void* mailbox, const void* parameters);
};

#pragma once
#include "../Interface/WorkEngine.h"

class Raytracer : public WorkEngine2 {
private:
	void render(void* target, WorkUnit* parameters, const Parameters* myParameters, MailBox* my_mailbox);

public:
	Raytracer();

	~Raytracer();

	void initialize_scene(const void* parameters);

	void job(void* target, void* mailbox, const void* parameters);
};

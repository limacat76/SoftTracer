#pragma once
#include "../Interface/CallableAlgorithm.h"
#include "../Interface/Mailbox.h"

class Raytracer : public CallableAlgorithm {
private:

public:
	Raytracer();

	~Raytracer();

	void initialize_scene();

	void render(void* target, MailBox* mailbox, const void* parameters);
};
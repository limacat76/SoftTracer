#pragma once
#include "../Interface/WorkEngine.h"

class Test : public WorkEngine {
private:
	void render(void* target, WorkUnit* parameters, const Parameters* myParameters, MailBox* my_mailbox);

public:
	Test() {
	}

	~Test() {
	}

	void initialize_scene(const void* parameters);

	void job(void* target, void* mailbox, const void* parameters);
};

class Bands : public WorkEngine {
private:
	void render(void* target, WorkUnit* parameters, const Parameters* myParameters, MailBox* my_mailbox);

public:
	Bands() {
	}

	~Bands() {
	}

	void initialize_scene(const void* parameters);

	void job(void* target, void* mailbox, const void* parameters);
};

class Noise : public WorkEngine {
private:
	void render(void* target, WorkUnit* parameters, const Parameters* myParameters, MailBox* my_mailbox);

public:
	Noise() {
	}

	~Noise() {
	}

	void initialize_scene(const void* parameters);

	void job(void* target, void* mailbox, const void* parameters);
};
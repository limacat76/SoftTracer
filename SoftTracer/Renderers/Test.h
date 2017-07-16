#pragma once
#include "../Interface/WorkEngine.h"

class Test : public WorkEngine {
private:

public:
	Test() {
	}

	~Test() {
	}

	void initialize_scene(const void* parameters);

	void render(void* target, void* mailbox, const void* parameters);
};

class Bands : public WorkEngine {
private:

public:
	Bands() {
	}

	~Bands() {
	}

	void initialize_scene(const void* parameters);

	void render(void* target, void* mailbox, const void* parameters);
};

class Noise : public WorkEngine {
private:

public:
	Noise() {
	}

	~Noise() {
	}

	void initialize_scene(const void* parameters);

	void render(void* target, void* mailbox, const void* parameters);
};
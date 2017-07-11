#pragma once
#include "../Interface/CallableAlgorithm.h"
#include "../Interface/Mailbox.h"

class Test : public CallableAlgorithm {
private:

public:
	Test() {
	}

	~Test() {
	}

	void initialize_scene();

	void render(void* target, MailBox* mailbox, const void* parameters);
};

class Bands : public CallableAlgorithm {
private:

public:
	Bands() {
	}

	~Bands() {
	}

	void initialize_scene();

	void render(void* target, MailBox* mailbox, const void* parameters);
};

class Noise : public CallableAlgorithm {
private:

public:
	Noise() {
	}

	~Noise() {
	}

	void initialize_scene();

	void render(void* target, MailBox* mailbox, const void* parameters);
};
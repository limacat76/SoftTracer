#include "Raytracer.h"
#ifdef _DEBUG
#include <iostream>
#endif

Raytracer::Raytracer() {
#ifdef _DEBUG
	std::cout << "ctor called! \n";
#endif
}

Raytracer::~Raytracer() {
#ifdef _DEBUG
	std::cout << "dtor called! \n";
#endif
}

void Raytracer::initialize_scene() {
	// Bands does not need to initialize a scene...
}

void Raytracer::render(void* target, MailBox* mailbox, const void* parameters) {
	uint32_t* image = (uint32_t*)target;
	const Parameters* myParameters = (const Parameters *)parameters;

	const int thread_no = myParameters->threadNumber;
	const int total_threads = myParameters->totalThreads;
	const int aSection = myParameters->height / total_threads;
	const int startY = thread_no * aSection;
	const int endY = thread_no == total_threads - 1 ? myParameters->height : thread_no * aSection + aSection;

	mailbox->work_started(thread_no);
	uint32_t* pixel = image + (startY * myParameters->width);

	// Trace rays
	bool quit = false;
	for (unsigned y = startY; !quit && y < endY; ++y) {
		for (unsigned x = 0; !quit && x < myParameters->width; ++x, ++pixel) {
			uint32_t anUint = 255 << 8;
			anUint = (anUint + x % 255) << 8;
			anUint = (anUint + y % 255) << 8;
			anUint = (anUint + (x + y) % 255);

			*pixel = anUint;
			quit = mailbox->to_children_quit;
		}
	}

	mailbox->work_done(thread_no);
}
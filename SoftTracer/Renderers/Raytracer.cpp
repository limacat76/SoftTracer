#include "../Graphics/Graphics.h"
#include "../Interface/Mailbox.h"
#include "../Interface/Parameters.h"
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

void Raytracer::initialize_scene(const void* parameters) {
	// Bands does not need to initialize a scene...
}

void Raytracer::render(void* target, void* mailbox, const void* parameters) {
	pixel* image = (pixel*)target;
	MailBox* my_mailbox = (MailBox *)mailbox;
	const Parameters* myParameters = (const Parameters *)parameters;

	const int thread_no = myParameters->threadNumber;
	const int total_threads = myParameters->totalThreads;

	const int startX = myParameters->start_x;
	int endX = startX + myParameters->width_x;

	const int startY = myParameters->start_y; // thread_no * aSection;
	int endY = startY + myParameters->height_y;   // thread_no == total_threads - 1 ? myParameters->height : thread_no * aSection + aSection;

	my_mailbox->work_started(thread_no);
	pixel* start_pixel = image + startX + (startY * myParameters->width);

	// Trace rays
	bool quit = false;
	for (int y = startY; !quit && y < endY; ++y) {
		pixel* current_pixel = start_pixel;

		for (int x = startX; !quit && x < endX; ++x, ++current_pixel) {
			*current_pixel = (255 << 24) + ((x % 255) << 16) + ((y % 255) << 8) + ((x + y) % 255);
			quit = my_mailbox->to_children_quit;
		}
		start_pixel = start_pixel + myParameters->width;
	}

	my_mailbox->work_done(thread_no);
}

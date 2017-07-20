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

}

void Raytracer::job(void* target, void* mailbox, const void* parameters) {
	
	MailBox* my_mailbox = (MailBox *)mailbox;
	const Parameters* myParameters = (const Parameters *)parameters;
	const int thread_no = myParameters->threadNumber;
	my_mailbox->work_started(thread_no);
	int current_work = 0;
	while (!my_mailbox->to_children_quit && !(my_mailbox->to_children_no_more_work && my_mailbox->work_queue.size() <= current_work)) {
		current_work = my_mailbox->current_work++;
		if (my_mailbox->work_queue.size() > current_work) {
			WorkUnit * aValue = my_mailbox->work_queue[current_work];
			render(target, aValue, myParameters, my_mailbox);
		}
	}
	my_mailbox->work_done(thread_no);
}

void Raytracer::render(void* target, WorkUnit* workUnit, const Parameters* myParameters, MailBox* my_mailbox) {
	pixel* image = (pixel*)target;
	const int startX = workUnit->allocated_width;
	int endX = startX + workUnit->real_block_width;

	const int startY = workUnit->allocated_height; // thread_no * aSection;
	int endY = startY + workUnit->real_block_height;   // thread_no == total_threads - 1 ? myParameters->height : thread_no * aSection + aSection;

	
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


}

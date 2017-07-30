#include <random>
#include <limits>
#include "../Graphics/Graphics.h"
#include "../Interface/Mailbox.h"
#include "../Interface/Parameters.h"
#include "Test.h"

void Test::initialize_scene(const void* parameters) {
	// Test does not need to initialize a scene...
}

void Test::job(void* target, void* mailbox, const void* parameters) {

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

void Test::render(void* target, WorkUnit* workUnit, const Parameters* myParameters, MailBox* my_mailbox) {
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
			pixel anUint = 255 << 8;
			anUint = (anUint + x % 255) << 8;
			anUint = (anUint + y % 255) << 8;
			anUint = (anUint + (x + y) % 255);

			*current_pixel = anUint;
			quit = my_mailbox->to_children_quit;
		}
		start_pixel = start_pixel + myParameters->width;
	}


}


void Bands::initialize_scene(const void* parameters) {
	// Bands does not need to initialize a scene...
}

void Bands::render(void* target, WorkUnit* workUnit, const Parameters* myParameters, MailBox* my_mailbox) {
	pixel* image = (pixel*)target;
	const int startX = workUnit->allocated_width;
	int endX = startX + workUnit->real_block_width;

	const int startY = workUnit->allocated_height; // thread_no * aSection;
	int endY = startY + workUnit->real_block_height;   // thread_no == total_threads - 1 ? myParameters->height : thread_no * aSection + aSection;


	pixel* start_pixel = image + startX + (startY * myParameters->width);
	const float luminosity = 255.0f / myParameters->height;
	// Trace rays
	bool quit = false;
	for (int y = startY; !quit && y < endY; ++y) {
		pixel* current_pixel = start_pixel;

		int my_luminosity = (int)(luminosity * y);

		for (int x = startX; !quit && x < endX; ++x, ++current_pixel) {
			pixel anUint = 255 << 8;
			anUint = (anUint + my_luminosity) << 8;
			anUint = (anUint + my_luminosity) << 8;
			anUint = (anUint + my_luminosity);

			*current_pixel = anUint;
			quit = my_mailbox->to_children_quit;
		}
		start_pixel = start_pixel + myParameters->width;
	}
}

void Bands::job(void* target, void* mailbox, const void* parameters) {

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

void Noise::initialize_scene(const void* parameters) {
	// Noise does not need to initialize a scene...
}

void Noise::render(void* target, WorkUnit* workUnit, const Parameters* myParameters, MailBox* my_mailbox) {
	pixel* image = (pixel*)target;
	const int startX = workUnit->allocated_width;
	int endX = startX + workUnit->real_block_width;

	const int startY = workUnit->allocated_height; // thread_no * aSection;
	int endY = startY + workUnit->real_block_height;   // thread_no == total_threads - 1 ? myParameters->height : thread_no * aSection + aSection;


	pixel* start_pixel = image + startX + (startY * myParameters->width);
	const float luminosity = 255.0f / myParameters->height;

	std::random_device rd;
	std::mt19937_64 eng(rd());
	std::uniform_int_distribution<pixel> distr;
	const pixel all_alpha = 255 << 24;

	// Trace rays
	bool quit = false;
	for (int y = startY; !quit && y < endY; ++y) {
		pixel* current_pixel = start_pixel;

		for (int x = startX; !quit && x < endX; ++x, ++current_pixel) {
			*current_pixel = distr(eng) | all_alpha;
			quit = my_mailbox->to_children_quit;
		}
		start_pixel = start_pixel + myParameters->width;

	}

}

void Noise::job(void* target, void* mailbox, const void* parameters) {

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
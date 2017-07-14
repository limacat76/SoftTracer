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

void Raytracer::initialize_scene() {
	// Bands does not need to initialize a scene...
}

void Raytracer::render(void* target, void* mailbox, const void* parameters) {
	pixel* image = (pixel*)target;
	
	MailBox* my_mailbox = (MailBox *)mailbox;
	const Parameters* my_parameters = (const Parameters *)parameters;

	const int thread_no = my_parameters->threadNumber;
	const int total_threads = my_parameters->totalThreads;
	const int band = my_parameters->height / total_threads;
	const int startY = thread_no * band;
	const int endY = thread_no == total_threads - 1 ? my_parameters->height : thread_no * band + band;
	const int width = my_parameters->width;

	my_mailbox->work_started(thread_no);
	pixel* currentPixel = image + (startY * width);

	// Trace rays
	bool quit = false;
	for (int y = startY; !quit && y < endY; ++y) {
		for (int x = 0; !quit && x < width; ++x, ++currentPixel) {
			*currentPixel = (255 << 24) + ((x % 255) << 16) + ((y % 255) << 8) + ((x + y) % 255);
			quit = my_mailbox->to_children_quit;
		}
	}

	my_mailbox->work_done(thread_no);
}

JBEngine::JBEngine() {
#ifdef _DEBUG
	std::cout << "ctor called\n";
#endif
}

JBEngine::~JBEngine() {
#ifdef _DEBUG
	std::cout << my_thread_no << " dtor called\n";
#endif
}

void JBEngine::initialize_scene() {
	;
}

void JBEngine::render(void* target, void* mailbox, const void* parameters) {
	Scene* m_Scene = new Scene();

	pixel* image = (pixel*)target;

	MailBox* my_mailbox = (MailBox *)mailbox;
	const Parameters* my_parameters = (const Parameters *)parameters;

	const int thread_no = my_parameters->threadNumber;
#ifdef _DEBUG
	my_thread_no = thread_no;
#endif
	const int total_threads = my_parameters->totalThreads;
	const int band = my_parameters->height / total_threads;
	const int startY = thread_no * band;
	const int endY = thread_no == total_threads - 1 ? my_parameters->height : thread_no * band + band;
	const int width = my_parameters->width;

	my_mailbox->work_started(thread_no);
	pixel* currentPixel = image + (startY * width);

	// Trace rays
	bool quit = false;
	for (int y = startY; !quit && y < endY; ++y) {
		for (int x = 0; !quit && x < width; ++x, ++currentPixel) {
			*currentPixel = (255 << 24) + (((x + y) % 255) << 16) + ((y % 255) << 8) + (x % 255);
			quit = my_mailbox->to_children_quit;
		}
	}

	my_mailbox->work_done(thread_no);
	delete m_Scene;
}
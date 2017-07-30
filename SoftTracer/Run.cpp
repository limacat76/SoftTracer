#include <iostream>
#include <ctime>
#include <chrono>
#include <vector>
#include <thread>
#include <conio.h>
#include <boost/program_options.hpp>
#include "Graphics/Graphics.h"
#include "Interface/Mailbox.h"
#include "Interface/Parameters.h"
#include "Interface/Target.h"
#include "Interface/WorkEngine.h"
#include "Renderers/Test.h"
#include "Renderers/Raytracer.h"
#include "Tutorial/Tutorial.h"
#include "Time.h"

namespace options = boost::program_options;

#ifdef _DEBUG
void printDefines() {
	std::cout << "Additional defines go here \n";
}
#endif

void run_engine(const int &no_threads, const int &width, const int &height, WorkEngine *engine, pixel *&image, Target *target, const bool &log_threads, const bool &log_total) {
	make_picture_blank(image, width, height);
	
	std::vector<std::thread> threads(no_threads);
	std::vector<bool> allocated(no_threads);

	const int block_width = 8;
	const int block_height = 8;

	engine->initialize_scene(new Parameters(-1, no_threads, width, height, 0, block_width, 0, block_height));

	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	std::time_t start_time = std::chrono::system_clock::to_time_t(start);
	if (log_total) {
		std::cout << "starting computation at " << limacat::take_my_time();
	}

	int frames = 0;
	const int hz = 1000 / 24;
	bool allClosed = false;
	std::chrono::time_point<std::chrono::system_clock> end;

	MailBox* mailbox = new MailBox(no_threads);
	Parameters* parameters = new Parameters[no_threads];

	bool quit = false;
	bool has_work = true;
	bool must_subdivide = true;

	int currentWorkUnit = 0;
	int allocated_threads = 0;

	int allocated_width = 0;
	int allocated_height = 0;

	while (must_subdivide) {
		WorkUnit* aWorkUnit = new WorkUnit();
		aWorkUnit->real_block_width = block_width;
		if (allocated_width + aWorkUnit->real_block_width > width) {
			aWorkUnit->real_block_width = width - allocated_width;
		}

		aWorkUnit->real_block_height = block_height;
		if (allocated_height + aWorkUnit->real_block_height > height) {
			aWorkUnit->real_block_height = height - allocated_height;
		}

		aWorkUnit->allocated_width = allocated_width;
		aWorkUnit->allocated_height = allocated_height;

		mailbox->work_queue.push_back(aWorkUnit);

		allocated_width = allocated_width + block_width;
		if (allocated_width > width) {
			allocated_width = 0;
			allocated_height = allocated_height + block_height;
			if (allocated_height > height) {
				must_subdivide = false;
			}
		}
	}

	mailbox->to_children_no_more_work = true;

	for (int i = 0; i < no_threads; i++) {
		parameters[i] = Parameters(i, no_threads, width, height, 0, 0, 0, 0);
		threads[i] = (std::thread(&WorkEngine::job, engine, image, mailbox, (const void *)&parameters[i]));
	}

	while(!quit) {
		target->loop(quit, frames, allClosed);
		if (!allClosed) {
			for (int i = 0; i < no_threads; i++) {
				bool has_finished = mailbox->to_main_finished_working[i];
			}
			bool found = true;
			for (int i = 0; found && i < no_threads; i++) {
				found = mailbox->to_main_finished_working[i];
			}
			if (found) {
				allClosed = true;
				if (log_threads) {
					std::cout << "Threads stopped running, switching to blocking mode" << "\n";
				}
				end = std::chrono::system_clock::now();
			}
		}
		if (quit) {
			mailbox->to_children_quit = true;
		}
	}
	for (int i = 0; i < no_threads; i++) {
		threads[i].join();
	}

	while (!mailbox->work_queue.empty())
	{
		WorkUnit* aWorkUnit  = mailbox->work_queue.back();
		delete aWorkUnit;
		mailbox->work_queue.pop_back();
	}


	if (log_threads) {
		for (int i = 0; i < no_threads; i++) {
			std::cout << "Thread " << i << " started at " << mailbox->to_main_start_working_time[i];
			std::cout << " finished at " << mailbox->to_main_finish_working_time[i] << "\n";
		}
	}

	if (log_total) {
		std::chrono::duration<double> elapsed_seconds = end - start;
		std::cout << "finished computation at " << limacat::take_my_time() << "elapsed time: " << elapsed_seconds.count() << "s\n";
		std::cout << "Rendered " << frames << " frames @" << frames / elapsed_seconds.count() << " fps \n";
	}
}

template <typename T> T getParameter(options::variables_map value_map, std::string parameter_key) {
	T result = value_map[parameter_key].as<T>();
#ifdef _DEBUG
	std::cout << parameter_key << ":=" << result << "\n";
#endif
	return result;
}

int main(int argc, char *argv[]) {
#ifdef _DEBUG
	printDefines();
#endif

	// Declare the supported options.
	options::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("width", options::value<int>()->default_value(1920), "set screen width")
		("height", options::value<int>()->default_value(1080), "set screen height")
		("continue", options::value<bool>()->default_value(false)->implicit_value(true), "set continue")
		("continue-times", options::value<int>()->default_value(10), "set number of times test have to run for --continue")		
		("threads", options::value<int>()->default_value(8), "set default number of threads")
		("target", options::value<int>()->default_value(0), "set target, 0 for SDL, 1 for Headless")
		("auto-quit", options::value<bool>()->default_value(false)->implicit_value(true), "set auto-quit for profiling")
		("full-screen", options::value<bool>()->default_value(false)->implicit_value(true), "set full-screen (non headless targets)")
		("engine", options::value<int>()->default_value(0), "set engine, 0 for JBikker, 1 for Noise")
		("no-log-threads", options::value<bool>()->default_value(false)->implicit_value(true), "disable logging for threads (start and end)")
		("no-log-total", options::value<bool>()->default_value(false)->implicit_value(true), "disable logging for totals (start and end)")
		("log-continue-modulo", options::value<int>()->default_value(100), "will print a row every X operation in continue mode")
		;

	options::variables_map vm;
	options::store(options::parse_command_line(argc, argv, desc), vm);
	options::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << "\n";
#pragma warning( push ) 
#pragma warning( disable : 6031) 
		_getch();
#pragma warning( pop ) 
		return 1;
	}

	int width = getParameter<int>(vm, "width");
	int height = getParameter<int>(vm, "height");
	bool test_continue = getParameter<bool>(vm, "continue");
	int test_continue_times = getParameter<int>(vm, "continue-times");
	int no_threads = getParameter<int>(vm, "threads");
	int target_number = getParameter<int>(vm, "target");

	bool auto_quit = getParameter<bool>(vm, "auto-quit");
	bool full_screen = getParameter<bool>(vm, "full-screen");
	int engine_number = getParameter<int>(vm, "engine");
	bool log_threads = !getParameter<bool>(vm, "no-log-threads");
	bool log_total = !getParameter<bool>(vm, "no-log-total");
	int continue_times_print = getParameter<int>(vm, "log-continue-modulo");

	pixel* image = new pixel[width * height];
	Target* target;
	switch (target_number) {
	case 0: 
		target = new SDLTarget(image, width, height, full_screen);
		break;
	case 1:
	default:
		target = new Headless();
		break;
	}
	target->set_auto_continue(test_continue);

	WorkEngine* engine;
	switch (engine_number) {
	case 0:
		engine = new JBikker::Engine();
		break;
	case 1:
	default:
		engine = new Noise();
		break;
	}
	
	if (!test_continue) {
		test_continue_times = 1;
	}
	for (int ii = 1; ii <= test_continue_times; ii++) {
		run_engine(no_threads, width, height, engine, image, target, log_threads, log_total);
		if (ii % continue_times_print == 0) {
			std::cout << "Done " << ii << "jobs\n";
		}
	}

	target->stop();
	delete[] image;
	if (!auto_quit) {
		std::cin.clear();
		std::cout << "Press any key to exit \n";
#pragma warning( push ) 
#pragma warning( disable : 6031) 
		_getch();
#pragma warning( pop ) 
	}

	return 0;
}


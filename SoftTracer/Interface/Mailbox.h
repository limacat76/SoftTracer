#pragma once
#include <mutex>
#include <deque>
#include <thread>

class MailBox {
public:
	MailBox(int no_threads);
	~MailBox();
	void work_done(int by_thread);
	void work_started(int by_thread);

	bool to_children_quit;
	bool* to_main_finished_working;
	std::string* to_main_start_working_time;
	std::string* to_main_finish_working_time;

	// Messages
	std::deque<std::string> messages;
	std::mutex messages_mutex;
};
#include "Mailbox.h"
#include "../Time.h"

MailBox::MailBox(int no_threads) {
	to_main_finished_working = new bool[no_threads] { false };
	to_main_start_working_time = new std::string[no_threads];
	to_main_finish_working_time = new std::string[no_threads];
}

MailBox::~MailBox() {
	delete[] to_main_finished_working;
	delete[] to_main_start_working_time;
	delete[] to_main_finish_working_time;
}

void MailBox::work_started(int by_thread) {
	to_main_start_working_time[by_thread] = limacat::take_my_time();
}

void MailBox::work_done(int by_thread) {
	to_main_finish_working_time[by_thread] = limacat::take_my_time();
	to_main_finished_working[by_thread] = true;
}


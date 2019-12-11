#include "log.h"
#include <thread>
#include <string>
#include <iostream>

void threadFun(const int id, const LogLevel level){
	std::cout<<"thread " << id << "start... "<< std::this_thread::get_id() << std::endl;
	int count = 100;
	while(--count > 0){
		Log(level, "thread ", std::to_string(id), ": ", std::to_string(count));
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

int main(){
	//Logger log(LogLevel::Info);

	auto t1 = std::thread(threadFun, 1, LogLevel::Debug);
	auto t2 = std::thread(threadFun, 2, LogLevel::Info);
	auto t3 = std::thread(threadFun, 3, LogLevel::Warn);
	auto t4 = std::thread(threadFun, 4, LogLevel::Error);

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	//log.logClose();
	return 0;
}

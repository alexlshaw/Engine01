#ifndef ENGINE01_TASKMANAGER_H
#define ENGINE01_TASKMANAGER_H

#include <chrono>
#include <mutex>
#include <queue>
#include <thread>

#include "DebuggingTools.h"
#include "Task.h"

const static int WORKER_THREAD_COUNT = 2;		//number of worker threads we spawn to handle background processing

class TaskManager
{
private:
	bool exitRequested = false;
	std::thread workers[WORKER_THREAD_COUNT];
	std::mutex queueLock;
	std::queue<Task*> tasks;
	void workerThreadMethod();
public:
	TaskManager();
	~TaskManager();
	void queueTask(Task* task);
};

#endif
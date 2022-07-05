#include "TaskManager.h"

TaskManager::TaskManager()
{
	//spawn the desired number of worker threads
	for (int i = 0; i < WORKER_THREAD_COUNT; i++)
	{
		workers[i] = std::thread(&TaskManager::workerThreadMethod, this);
	}
}

TaskManager::~TaskManager()
{
	exitRequested = true;	//Set a flag notifying the threads that they no longer have to do any work, then wait for them all to finish
	for (auto& worker : workers)
	{
		worker.join();
	}
}

void TaskManager::workerThreadMethod()
{
	DEBUG_PRINT("Worker starting\n");
	while (!exitRequested)
	{
		queueLock.lock();	//acquire exclusive access to the queue (has to be done before checking if empty in case there is only one task and two threads want to check it)
		if (!tasks.empty())
		{
			Task* taskToExecute = tasks.front();
			tasks.pop();
			queueLock.unlock();
			taskToExecute->execute();
			delete taskToExecute;	//worker thread takes care of deleting the queued task after it has been completed
		}
		else
		{
			queueLock.unlock();
			//nothing to work on, but we havn't received notification to exit yet, so we'll sleep for a quarter second
			std::this_thread::sleep_for(std::chrono::milliseconds(250));
		}
	}
}

void TaskManager::queueTask(Task* task)
{
	queueLock.lock();
	tasks.push(task);
	queueLock.unlock();
}
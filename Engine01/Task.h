#ifndef ENGINE01_TASK_H
#define ENGINE01_TASK_H

#include "DebuggingTools.h"

class Task
{
private:
public:
	Task();
	virtual ~Task();
	virtual void execute();
};

#endif
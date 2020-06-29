#pragma once
#include "ComLib.h"
#include <string>

using namespace std;

class Consumer
{
private:
	ComLib* conComLib;

	char* message;

	bool error;

	int delay;
	int memorySize;
	int numMessages;
	int msgSize;

public:
	Consumer(int delay, int memorySize, int numMessages, int msgSize);
	void Run();
	void Retry();
	bool GetError() { return error; }
	~Consumer();
};


#pragma once
#include "ComLib.h"
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

class Producer
{
private:
	ComLib* prodComLib;

	int delay;
	int memorySize;
	int numMessages;
	int msgSize;

	bool randomMsg;
	bool canClose;

	size_t maxSize;
	size_t msgByteSize;


public:
	Producer(int delay, int memorySize, int numMessages, int msgSize, bool randomize);
	~Producer();

	void Run();
	void gen_random(char* s, const int len);
};


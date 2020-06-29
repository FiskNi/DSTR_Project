#include "Producer.h"

#include <stdio.h>
#include <conio.h>
#include <tchar.h>

Producer::Producer(int delay, int memorySize, int numMessages, int msgSize, bool randomize)
{
	this->delay = delay;
	this->memorySize = memorySize;

	this->numMessages = numMessages;
	this->msgSize = msgSize;
	randomMsg = randomize;

	prodComLib = new ComLib(this->memorySize, PRODUCER);
	
		cout << "CombLib prepared" << endl;

	prodComLib->CreateFileMap();
	
		cout << "Filemap created and opened" << endl;

	prodComLib->GetMap();
	
		cout << "Filemap linked" << endl;


	   
		cout << "Ready to write" << endl << endl;
}

Producer::~Producer()
{
	prodComLib->Free();
	if (prodComLib)
		delete prodComLib;
}

void Producer::Run()
{
	// Handle memory
	msgByteSize = 1 << 6; // 64 Bytes
	memorySize = memorySize << 10;
	maxSize = memorySize / 2;

	for (int i = 0; i < numMessages; i++)
	{
		if (DEBUGcl)
		{
			cout << "Current message: " << i << endl;
			cout << "Preparing message..." << endl;
		}
		if (randomMsg)
		{
			msgSize = rand() % (memorySize / 2);
		}

		if (msgSize > maxSize)
		{
			// Adjust size to the max size
			if (DEBUGcl)
				cout << "Message size too large, resizing.." << endl;
			msgSize = maxSize;
		}
		else
		{
			// Adjust size to fit message
			msgByteSize *= ceil((float)msgSize / (float)msgByteSize);
		}
		if (DEBUGcl)
			cout << "Message size: " << msgByteSize << " Bytes" << endl;

		// Randomize char string
		char* randomMessage = new char[msgByteSize];
		gen_random(randomMessage, msgSize);

		// Send message
		prodComLib->send(randomMessage, msgByteSize);
		if (DEBUGcl)
			cout << "Sent the message: " << randomMessage << endl << endl;
		else
			cout << randomMessage << endl;


		delete randomMessage;

		Sleep(delay);
	}
	
	// TODO: Only close when consumer done reading

	while (!prodComLib->CanClose())
	{
		Sleep(10);	
	}
	

	if (DEBUGcl)
		cout << "Done sending messages" << endl;
}

void Producer::gen_random(char* s, const int len) 
{ 

	char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"; 

	for (auto i = 0; i < len; ++i) 
	{ 
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)]; 
	}

	s[len - 1] = 0; 
}
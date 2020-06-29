#pragma once
enum TYPE { PRODUCER, CONSUMER };
const bool DEBUGcl = false;

#include <Windows.h>
#include <string>
#include <iostream>

using namespace std;

class ComLib
{


private:
	// 1) define variables (global or in a struct/class)
	HANDLE hFileMap;
	void* mData;
	bool exists;

	size_t mSize;
	size_t availableMem;
	size_t headerMemSize;

	int maxCharOffset;

	int headOffset;
	int tailOffset;
	int loopedOffset;
	int rsOffset;

	int head;
	int tail;

	int looped;

	int writeState;
	int readState;

	
public:


    // Constructor
    // secret is the known name for the shared memory
    // buffSize is in MEGABYTES (multiple of 1<<20)
    // type is TYPE::PRODUCER or TYPE::CONSUMER
	ComLib(const size_t& buffSize, TYPE type);

    // returns "true" if data was sent successfully.
    // false if for ANY reason the data could not be sent.
    // we will not implement an "error handling" mechanism, so we will assume
    // that false means that there was no space in the buffer to put the message.
    // msg is a void pointer to the data.
    // length is the amount of bytes of the message to send.
	bool send(const void* msg, const size_t length);

    // returns: "true" if a message was received.
    // false if there was nothing to read.
    // "msg" is expected to have enough space for the message.
    // use "nextSize()" to check whether our temporary buffer is big enough
    // to hold the next message.
    // @length returns the size of the message just read.
    // @msg contains the actual message read.
	bool recv(char*& msg, size_t& length);

    // return the length of the next message
    // return 0 if no message is available.
    size_t nextSize();

	size_t GetAvailableMemory();

	// Create the shared memory FileMap
	bool CreateFileMap();
	bool OpenFileMap();
	void* GetMap();
	bool CanClose();
	void DoneReading();

	void Free();

    /* destroy all resources */
	~ComLib();
};


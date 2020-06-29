#include "ComLib.h"
#include <tchar.h>
#include <atlstr.h>
#include <string>
#include <sstream>


ComLib::ComLib(const size_t& buffSize, TYPE type)
{
	hFileMap = NULL;
	mData = NULL;
	exists = false;
	mSize = buffSize << 10;

	if (DEBUGcl)
		cout << "Total momery: " << mSize << " Bytes" <<  endl;

	availableMem = mSize - sizeof(int) * 5;
	if (DEBUGcl)
		cout << "Available read write momery: " << availableMem << " Bytes" <<  endl;

	maxCharOffset = availableMem;
	if (DEBUGcl)
		cout << "Max char pointer offset: " << maxCharOffset << endl;

	headOffset		= (mSize / sizeof(int)) - sizeof(int);
	tailOffset		= (mSize / sizeof(int)) - sizeof(int) * 2;
	loopedOffset	= (mSize / sizeof(int)) - sizeof(int) * 3;
	rsOffset		= (mSize / sizeof(int)) - sizeof(int) * 4;

	headerMemSize = sizeof(int);

	head = 0;
	tail = 0;

	writeState = 0;
	readState = 0;
}

bool ComLib::send(const void* msg, const size_t length)
{
	if (writeState == 0)
		writeState = 1;

	// Recieve tail offset
	tail = *((int*)mData + tailOffset);

	if (DEBUGcl)
	{
		cout << "Head offset: " << head << endl;
		cout << "Tail offset: " << tail << endl;
	}

	// Wait
	// Don't go past tail (let it read)
	bool wait = false;
	while (head + length + headerMemSize >= tail && looped)
	{
		tail = *((int*)mData + tailOffset);
		looped = *((int*)mData + loopedOffset);

		if (!wait)
		{
			if (DEBUGcl)
				cout << "Waiting for tail..." << endl;
			wait = true;
		}
	}

	// Get header pointer and save header
	int header = (int)length;
	memcpy((int*)mData + (head / sizeof(int)), &header, headerMemSize);

	// Check if message fits after header
	if (head + headerMemSize + length > maxCharOffset)
	{
		head = 0;
		looped = 1;
		memcpy((int*)mData + headOffset, &head, sizeof(int));
		memcpy((int*)mData + loopedOffset, &looped, sizeof(int));

		if (DEBUGcl)
		{
			cout << "Head offset: " << head << endl;
			cout << "Tail offset: " << tail << endl;
		}
		while (head + length + headerMemSize >= tail && looped)
		{
			tail = *((int*)mData + tailOffset);
			looped = *((int*)mData + loopedOffset);

			if (!wait)
			{
				if (DEBUGcl)
					cout << "Waiting for tail..." << endl;
				wait = true;
			}
		}
	}

	// Get message pointer and save message
	char* message = (char*)msg;
	size_t messageMemSize = (sizeof(char) * length);
	memcpy((char*)mData + head + headerMemSize, message, messageMemSize);

	// Increase head location
	head += (int)length + headerMemSize;
	// Check if header fits for next message
	if (head > maxCharOffset)
	{
		head = 0;
		looped = 1;

		// Save current head and looped status into momery
		memcpy((int*)mData + headOffset, &head, sizeof(int));
		memcpy((int*)mData + loopedOffset, &looped, sizeof(int));
	}
	else
	{
		// Save current head position into momery
		memcpy((int*)mData + headOffset, &head, sizeof(int));
	}

	return false;
}

bool ComLib::recv(char*& msg, size_t& length)
{
	if (readState == 0)
	{
		readState = 1;
		memcpy((int*)mData + rsOffset, &readState, sizeof(int));
	}

	// Recieve head offset
	head = *((int*)mData + headOffset);
	// Recieve looped status
	looped = *((int*)mData + loopedOffset);

	if (DEBUGcl)
	{
		cout << "Head offset: " << head << endl;
		cout << "Tail offset: " << tail << endl;
	}

	// Wait
	bool wait = false;
	while (tail >= head && !looped)
	{
		head = *((int*)mData + headOffset);
		looped = *((int*)mData + loopedOffset);

		if (!wait)
		{
			if (DEBUGcl)
				cout << "Waiting for head..." << endl;
			wait = true;
		}
	}

	while (tail < head && looped)
	{
		head = *((int*)mData + headOffset);
		looped = *((int*)mData + loopedOffset);

		if (!wait)
		{
			if (DEBUGcl)
				cout << "Waiting for head..." << endl;
			wait = true;
		}
	}

	// Recieve data pointer to message start
	memcpy(msg, (char*)mData + tail + sizeof(int), (int)length);

	// Don't go past max momery size
	tail += (int)length + headerMemSize;

	if (tail > maxCharOffset)
	{
		tail = 0;
		looped = 0;

		// Save current tail and looped status into momery
		memcpy((int*)mData + tailOffset, &tail, sizeof(int));
		memcpy((int*)mData + loopedOffset, &looped, sizeof(int));
	}
	else
	{
		// Save current tail position into momery
		memcpy((int*)mData + tailOffset, &tail, sizeof(int));
	}

	return false;
}

size_t ComLib::nextSize()
{
	bool wait = false;
	while (tail >= head && !looped)
	{
		head = *((int*)mData + headOffset);
		looped = *((int*)mData + loopedOffset);

		if (!wait)
		{
			if (DEBUGcl)
				cout << "Waiting for head..." << endl;
			wait = true;
		}
	}

	while (tail < head && looped)
	{
		head = *((int*)mData + headOffset);
		looped = *((int*)mData + loopedOffset);

		if (!wait)
		{
			if (DEBUGcl)
				cout << "Waiting for head..." << endl;
			wait = true;
		}
	}

	// Recive message size 
	if (tail + headerMemSize > maxCharOffset)
	{
		tail = 0;
	}
	int messageSize = *((int*)mData + (tail / sizeof(int)));

	if (tail + headerMemSize + messageSize > maxCharOffset)
	{
		tail = 0;
		looped = 0;
		// Save current tail and looped status into momery
		memcpy((int*)mData + tailOffset, &tail, sizeof(int));
		memcpy((int*)mData + loopedOffset, &looped, sizeof(int));
	}

	if (DEBUGcl)
		cout << "Recieved data size: " << messageSize << endl;

	return messageSize;
}

size_t ComLib::GetAvailableMemory()
{
	return availableMem; 
}

bool ComLib::CreateFileMap()
{

	// 2) API call to create FileMap (use pagefile as a backup)
	hFileMap = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		(DWORD)0,
		mSize,
		(LPCWSTR) "myFileMap");

	// 3) check if hFileMap is NULL -> FATAL ERROR
	if (hFileMap == NULL)
	{
		if (DEBUGcl)
			std::cout << "Failed to create" << std::endl;
		return false;
	}

	// 4) check 
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		if (DEBUGcl)
			std::cout << "ERROR_ALREADY_EXISTS" << std::endl;
		//return false;
	}
	//    This means that the file map already existed!, but we
	//    still get a handle to it, we share it!
	//    THIS COULD BE USEFUL INFORMATION FOR OUR PROTOCOL.


	mData = MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, mSize);

	// initialize head and tail as 0 in memory
	int zeroInt = 0;
	memcpy((int*)mData + headOffset, &zeroInt, sizeof(int));
	memcpy((int*)mData + tailOffset, &zeroInt, sizeof(int));
	memcpy((int*)mData + loopedOffset, &zeroInt, sizeof(int));
	memcpy((int*)mData + rsOffset, &zeroInt, sizeof(int));
	

	return false;
}

bool ComLib::OpenFileMap()
{
	// 5) Create one VIEW of the map with a void pointer
	//    This API Call will create a view to ALL the memory
	//    of the File map.
	hFileMap = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,				// read/write access
		FALSE,								// do not inherit the name
		(LPCWSTR)"myFileMap");              // name of mapping object

	if (!hFileMap)
		return false;

	return true;
}

void* ComLib::GetMap()
{
	mData = MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, mSize);

	return mData;
}

bool ComLib::CanClose()
{
	readState = *((int*)mData + rsOffset);
	if (readState == 0)
		return true;

	return false;
}

void ComLib::DoneReading()
{
	readState = 0;
	memcpy((int*)mData + rsOffset, &readState, sizeof(int));

}

void ComLib::Free()
{
	// LAST BUT NOT LEAST
	// Always, close the view, and close the handle, before quiting
	UnmapViewOfFile((LPCVOID)mData);
	CloseHandle(hFileMap);
}

ComLib::~ComLib()
{
	//Free();
}

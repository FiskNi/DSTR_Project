#include <Pch/Pch.h>
#include "MayaCom.h"
#include <tchar.h>
#include <atlstr.h>
#include <string>
#include <sstream>


MayaCom::MayaCom(const size_t& buffSize)
{
	hFileMap = NULL;
	mData = NULL;
	exists = false;

	// Size of shared memory
	// << 20 gives megabytes
	mSize = buffSize << 10;
	cout << "Total momery: " << mSize << " Bytes" << endl;

	availableMem = mSize - sizeof(int) * 5;
	cout << "Available read write momery: " << availableMem << " Bytes" << endl;

	maxCharOffset = availableMem;
	cout << "Max char pointer offset: " << maxCharOffset << endl;

	// Permanent data to store at the end of memory buffer
	// (mSize / sizeof(int) - Convert to integer sized blocks
	// - sizeof(int) - Go back one integer sized blocked to fit data
	headOffset = (mSize / sizeof(int)) - sizeof(int) * 1;
	tailOffset = (mSize / sizeof(int)) - sizeof(int) * 2;
	loopedOffset = (mSize / sizeof(int)) - sizeof(int) * 3;
	rsOffset = (mSize / sizeof(int)) - sizeof(int) * 4;

	head = 0;
	tail = 0;

	writeState = 0;
	readState = 0;

	looped = false;
}

bool MayaCom::CanWrite(const size_t length)
{
	// Check if data fits in memory
	if (head + length > maxCharOffset)
	{
		// Doesnt fit, loop around
		head = 0;
		looped = 1;

		// Tell shared memory writer has looped
		memcpy((int*)mData + headOffset, &head, sizeof(int));
		memcpy((int*)mData + loopedOffset, &looped, sizeof(int));
	}

	// Check if data will overwrite tail
	tail = *((int*)mData + tailOffset);
	looped = *((int*)mData + loopedOffset);
	if (head + length >= tail && looped)
	{
		return false;
	}



	return true;
}

void MayaCom::Write(const void* data, const size_t length)
{
	if (writeState == 0)
		writeState = 1;

	// Get message pointer and save message
	size_t dataSize = (sizeof(char) * length);
	memcpy((char*)mData + head, (char*)data, dataSize);

	// Move head to end of written data
	head += (int)length;
	cout << "Head offset: " << head << endl;

	// Might be useless 
	// Check if head is inside available memory and save header to back
	//if (head >= maxCharOffset)
	//{
	//	// Not inside, loop around
	//	head = 0;
	//	looped = 1;

	//	memcpy((int*)mData + headOffset, &head, sizeof(int));
	//	memcpy((int*)mData + loopedOffset, &looped, sizeof(int));
	//}
	//else
	//{
	//	memcpy((int*)mData + headOffset, &head, sizeof(int));
	//}
	memcpy((int*)mData + headOffset, &head, sizeof(int));
}

bool MayaCom::CanRead(const size_t length)
{
	head = *((int*)mData + headOffset);
	looped = *((int*)mData + loopedOffset);

	// Check if data is inside available memory
	if (tail + length > maxCharOffset)
	{
		// Not inside, loop around
		tail = 0;
		looped = 0;
		
		// Write to shared memory
		memcpy((int*)mData + tailOffset, &tail, sizeof(int));
		memcpy((int*)mData + loopedOffset, &looped, sizeof(int));

		// Check if caught up to head
		if (tail >= head && !looped)
		{
			return false;
		}


	}

	// Check if caught up to head
	if (tail >= head && !looped)
	{
		return false;
	}

	// Check if caught up to head when head is looped
	if (tail < head && looped)
	{
		return false;
	}

	return true;
}

bool MayaCom::Read(char* data, const size_t length)
{
	// Recieve data
	memcpy(data, (char*)mData + tail, (int)length);

	tail += (int)length;

	// Write to shared memory
	memcpy((int*)mData + tailOffset, &tail, sizeof(int));

	return false;
}


size_t MayaCom::GetAvailableMemory()
{
	return availableMem;
}

bool MayaCom::CreateFileMap()
{

	// 2) API call to create FileMap (use pagefile as a backup)
	hFileMap = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		(DWORD)0,
		mSize,
		(LPCWSTR)"mayacom");

	// 3) check if hFileMap is NULL -> FATAL ERROR
	if (hFileMap == NULL)
	{
		cout << "Failed to create filemap" << endl;
		return false;
	}

	// 4) check 
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		cout << "Filemap already exists" << endl;


		//return false;
	}
	//	This means that the file map already existed!, but we
	//	still get a handle to it, we share it!
	//	THIS COULD BE USEFUL INFORMATION FOR OUR PROTOCOL.


	mData = MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, mSize);

	// initialize head and tail as 0 in memory
	int zeroInt = 0;
	memcpy((int*)mData + headOffset, &zeroInt, sizeof(int));
	memcpy((int*)mData + tailOffset, &zeroInt, sizeof(int));
	memcpy((int*)mData + loopedOffset, &zeroInt, sizeof(int));
	memcpy((int*)mData + rsOffset, &zeroInt, sizeof(int));


	return false;
}

bool MayaCom::OpenFileMap()
{
	// 5) Create one VIEW of the map with a void pointer
	//    This API Call will create a view to ALL the memory
	//    of the File map.
	hFileMap = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,				// read/write access
		FALSE,								// do not inherit the name
		(LPCWSTR)"mayacom");              // name of mapping object

	if (!hFileMap)
		return false;

	return true;
}

void* MayaCom::GetMap()
{
	mData = MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, mSize);

	return mData;
}

bool MayaCom::CanClose()
{
	readState = *((int*)mData + rsOffset);
	if (readState == 0)
		return true;

	return false;
}



void MayaCom::UnmapView()
{
	// LAST BUT NOT LEAST
	// Always, close the view, and close the handle, before quiting
	UnmapViewOfFile((LPCVOID)mData);
}

void MayaCom::CloseFile()
{
	CloseHandle(hFileMap);

}

MayaCom::~MayaCom()
{
	UnmapView();
}
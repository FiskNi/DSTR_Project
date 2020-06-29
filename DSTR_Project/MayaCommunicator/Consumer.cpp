#include "Consumer.h"

Consumer::Consumer(int delay, int memorySize, int numMessages, int msgSize)
{
	this->delay = delay;
	this->memorySize = memorySize;
	this->numMessages = numMessages;
	this->msgSize = msgSize;
	message = nullptr;
	
	error = false;

	conComLib = new ComLib(this->memorySize, CONSUMER);
	if (DEBUGcl)
		cout << "Trying to open filemap.." << endl;
	if (!conComLib->OpenFileMap())
	{
		if (DEBUGcl)
		{
			cout << "Error opening filemap!" << endl;
			cout << "Retrying.." << endl;
		}
		Retry();
		error = false;
	}
	if (!error)
	{
		if (DEBUGcl)
			cout << "Filemap opened" << endl;
		conComLib->GetMap();
		if (DEBUGcl)
			cout << "Filemap linked" << endl << endl;
	}
	else
	{
		if (DEBUGcl)
			cout << "Exiting.." << endl;
	}


	message = nullptr;
}

void Consumer::Run()
{
	for (int i = 0; i < numMessages; i++)
	{
		if (DEBUGcl)
		{
			cout << "Recieving message..." << endl;
			cout << "Current message: " << i << endl;
		}


		size_t msgSize = conComLib->nextSize();
		message = new char[msgSize];

		//char* data = nullptr;
		conComLib->recv(message, msgSize);
		//memcpy(message, data, msgSize);


		if (DEBUGcl)
			cout << "Message recieved: " << message << ", Message size: " << msgSize << " Bytes" << endl << endl;
		else
			cout << message << endl;

		delete message;
		message = nullptr;

		Sleep(delay);
	}

	conComLib->DoneReading();

	if (DEBUGcl)
		cout << "Done writing messages" << endl;
}

void Consumer::Retry()
{
	bool retrying = true;
	int attempt = 0;
	while (retrying && attempt < 50)
	{
		Sleep(1000);
		if (DEBUGcl)
			cout << "Retrying.." << endl;
		if (conComLib->OpenFileMap())
			retrying = false;

		if (DEBUGcl)
		{
			if (attempt == 50 && retrying)
			{
				if (DEBUGcl)
					std::cout << "Continue to try?" << endl;
				//TODO add input
				attempt = 0;
			}
		}
		attempt++;
	}
}


Consumer::~Consumer()
{
}

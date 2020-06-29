#ifndef _APPLICATION_h
#define _APPLICATION_h
#include <Pch/Pch.h>
#include "Input.h"
#include "StateManager.h"

class Application {

public:
	Application();
	~Application();
	
	bool init();
	void run();


private:
	void initGraphics();
	void calcFPS(const float& dt);
	void centerWindowOnMonitor();
	GLFWwindow* m_window;
	Input* m_input;
	StateManager* m_stateManager;	
	bool initialFrame;
	bool deleteImgui = true;
	//NoLogger* m_noLog;
};




#endif

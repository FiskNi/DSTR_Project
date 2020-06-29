#include <Pch/Pch.h>
#include <System/Application.h>
#define STB_IMAGE_IMPLEMENTATION

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);


	Log::initialize();
	
	Application* m_app = new Application();
	
	if (m_app->init()) {
		m_app->run();
	}
	
	delete m_app;

	return 0;
}

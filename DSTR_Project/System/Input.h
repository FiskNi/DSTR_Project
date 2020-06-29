#ifndef _INPUT_H
#define _INPUT_H
#include <Pch/Pch.h>

/* 
	This is include in the PCH so you don't need to include it anywhere
	This is a static class and is initialized in application.
	For example you could do:

	Input::isKeyPressed(GLFW_KEY_W)

	which returns if the W key was pressed or not during that frame.
*/

class Application;

#define MAX_KEYS 375
#define MAX_MOUSE_KEYS 8
#define MAX_TEXTINPUT 16

class Input
{
public:
	Input();
	~Input();

	static bool isKeyPressed(int keycode);
	static bool isKeyReleased(int keycode);
	static bool isKeyHeldDown(int keycode);
	static bool isMousePressed(int button);
	static bool isMouseHeldDown(int button);
	static bool isMouseReleased(int button);
	static bool isAnyKeyPressed();
	static bool isAnyKeyReleased();
	static bool isAnyMousePressed();
	static bool isAnyMouseReleased();
	static bool isWindowFocused();
	static bool isTextInput();
	static const glm::vec2& getMousePosition();
	static char getLatestPressedCharacter();
	static int getLatestPressedCharacterAsInt();
	static int getLatestPressedMouseButton();
	static char* getInputBuffer();
	static int getBufferSize();
	static unsigned int getLatestKeyPressed();
private:

	friend class Application;
	friend void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	friend void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	friend void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	friend void window_focus_callback(GLFWwindow* window, int focused);
	friend void character_callback(GLFWwindow* window, unsigned int codepoint);

	void handleKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods);
	void handleMouseInput(GLFWwindow* window, int button, int action, int mods);
	void handleCursorMovement(GLFWwindow* window, double xpos, double ypos);
	void handleWindowFocused(GLFWwindow* window, int focused);
	void handleCharacterCallback(GLFWwindow* window, unsigned int codepoint);

	static void leftShift(char* words, int len);
	

	void clearKeys();

	static bool m_keysPressed[MAX_KEYS];
	static bool m_keysHeldDown[MAX_KEYS];
	static bool m_keysReleased[MAX_KEYS];

	static bool m_mousePressed[MAX_MOUSE_KEYS];
	static bool m_mouseHeldDown[MAX_MOUSE_KEYS];
	static bool m_mouseReleased[MAX_MOUSE_KEYS];

	static glm::vec2 m_mousePosition;
	static bool m_isWindowFocused;

	static char m_latestPressedCharacter;
	static unsigned int m_latestPressedMouseButton;
	static int m_latestPressedCharacterAsInt;
	
	static unsigned int m_latestKeyPressed;

	static bool m_anyKeyPressed;
	static bool m_anyKeyReleased;
	static bool m_anyMousePressed;
	static bool m_anyMouseReleased;
	static bool m_textInput;

	static char m_charBuffer[MAX_TEXTINPUT];
	static int m_charBufferIndex;

};

#endif
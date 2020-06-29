#ifndef _STATEMANAGER_H
#define _STATEMANAGER_H
#include <Pch/Pch.h>
#include "State.h"

class StateManager
{
public:
	StateManager();
	~StateManager();

	void popState();
	void pushState(State* newState);
	void clearAllAndSetState(State* state);

	bool getImGuiState();
	void getGuiInfo();

	void Update(float dt);
	void Render();

private:
	void clearStates();
	void clearKillList();

private:
	std::vector<State*> m_states;
	std::vector<State*> m_killList;	
};

#endif
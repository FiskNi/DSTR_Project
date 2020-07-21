#ifndef _PLAYSTATE_H
#define _PLAYSTATE_H
#include <Pch/Pch.h>
#include <System/State.h>
#include <GFX/MaterialMap.h>
#include <GameObject/GameObject.h>
#include <Player/Player.h>
#include <GameObject/WorldObject.h>
#include <GameObject/MapObject.h>
#include <System/BulletPhysics.h>
#include <GFX/Pointlight.h>
#include <System/MemoryUsage.h>

#include <DSTR-Project/DestructibleObject.h>

class PlayState : public State 
{

public:
	//PlayState(){}
	PlayState();
	void LoadSkybox(Renderer* renderer);
	void loadMap(Renderer* renderer);
	void loadDecor(Renderer* renderer);
	void loadLights(Renderer* renderer);
	void loadDestructibles();
	virtual ~PlayState() override;
	virtual void Update(float dt) override;
	void removeDeadObjects();
	virtual void Render() override;

private:
	void Update_isPlaying(const float& dt);


private:
	// Hardcoded solution for choosing the scene
	// 0 is default
	// 1 is debug
	int m_map = 1;

	float m_rotVal;
	
	//Any inherited GameObject class added to this vector will support basic rendering
	std::vector<GameObject*> m_objects;
	std::vector<Pointlight*> m_pointlights;

	/*DstrGenerator m_dstr;
	DstrGenerator m_dstr_alt1;*/

	Player* m_player;
	Camera* m_camera;
	SkyBox* m_skybox;
	MemoryUsage mu;
	
	float startY;

};



#endif
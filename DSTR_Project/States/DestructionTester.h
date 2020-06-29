#ifndef _DESTRUCTIONTESTER_H
#define _DESTRUCTIONTESTER_H
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

#include <DSTR-Project/DebugObjects.h>

class DestructionTester : public State
{

public:
	DestructionTester();
	virtual ~DestructionTester() override;
	virtual void Update(float dt) override;
	virtual void Render() override;

	void ShowMemoryInfo();

	static bool BulletCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1,
		const btCollisionObjectWrapper* obj2, int id2, int index2);

private:
	void LoadSkybox();
	void LoadScene();
	void LoadLights();
	void LoadDestructibles();
	void LoadDefaultDestructables();
	void LoadShowcaseDestructibles();


	void DebugScript();
	void RayCastBreak();
	void DebugButtons();
	void DebugCreatePlane();


	MemoryUsage mu;
	
	Renderer* m_renderer;
	Player* m_player;
	Camera* m_camera;
	SkyBox* m_skybox;
	std::vector<GameObject*> m_objects;
	std::vector<Pointlight*> m_pointlights;

	float memTimer;
	float fpsTimer;

	FractureGenerator m_debugFractureGen;
	FractureGenerator m_fractureGen;

	Weapon weapon;
	bool m_runDebug = false;
	DebugFractureTimer m_debugFractureTimer;

};



#endif
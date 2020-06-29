#ifndef _MAYAVIEWER_H
#define _MAYAVIEWER_H
#include <Pch/Pch.h>
#include <System/State.h>
#include <GFX/MaterialMap.h>
#include <GameObject/GameObject.h>
#include <GameObject/WorldObject.h>
#include <System/BulletPhysics.h>
#include <GFX/Pointlight.h>
#include <System/MemoryUsage.h>

// Maya reciever
#include "Reciever.h"
#include "MayaObject.h"


class MayaViewer : public State
{

public:
	//PlayState(){}
	MayaViewer();
	virtual ~MayaViewer() override;

	void LoadMap(Renderer* renderer);
	void LoadLights(Renderer* renderer);


	virtual void Update(float dt) override;
	void UpdateMaya();
	virtual void Render() override;

private:
	void Update_isPlaying(const float& dt);


private:
	//Any inherited GameObject class added to this vector will support basic rendering
	std::vector<GameObject*> m_objects;
	// Quick way of getting references
	std::map<std::string, GameObject*> m_objectMap;

	std::vector<Pointlight*> m_pointlights;

	Camera* m_camera;
	MemoryUsage mu;
};



#endif
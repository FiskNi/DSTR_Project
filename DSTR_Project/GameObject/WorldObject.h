#ifndef _WORLDOBJECT_h
#define _WORLDOBJECT_h
#include <GameObject/GameObject.h>

class WorldObject : public GameObject {
public:
	WorldObject();
	WorldObject(std::string name);
	virtual ~WorldObject();

	void Update(float dt);

private: 



};

#endif

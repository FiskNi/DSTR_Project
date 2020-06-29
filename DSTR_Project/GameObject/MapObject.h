#ifndef MAP_OBJECT_h
#define MAP_OBJECT_h

#include <GameObject/GameObject.h>

class MapObject : public GameObject {
public:
	MapObject();
	MapObject(std::string name);
	virtual ~MapObject();

	void Update(float dt);

private:



};


#endif
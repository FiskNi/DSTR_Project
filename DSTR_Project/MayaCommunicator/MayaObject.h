#ifndef	_MAYAOBJECT_h
#define _MAYAOBJECT_h

#include <GameObject/GameObject.h>
#include <GameObject/MapObject.h>

class MayaObject :
	public GameObject
{
private:

public:
	MayaObject(std::string name);
	virtual ~MayaObject();
	void Update(float dt);

	void InitMesh(Mesh* mesh);

};


#endif

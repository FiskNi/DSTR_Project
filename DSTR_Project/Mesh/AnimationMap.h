#ifndef _ANIMATIONMAP_H
#define _ANIMATIONMAP_H
#include <Pch/Pch.h>
#include <Mesh/MeshFormat.h>

class AnimationMap
{
public:
	static AnimationMap* GetInstance();
	void cleanUp();
	bool existsWithName(std::string name);
	Animation* getAnimation(std::string name);
	Animation* createAnimation(std::string name, Animation anim);
	void destroy();
private:
	AnimationMap();
	static AnimationMap* m_animMapInstance;
	std::map<std::string, Animation*> m_animationMap;
};

#endif
#include "Pch/Pch.h"
#include "AnimationMap.h"

AnimationMap* AnimationMap::m_animMapInstance = 0;
AnimationMap::AnimationMap() {}

AnimationMap* AnimationMap::GetInstance()
{
	if (m_animMapInstance == 0)
	{
		m_animMapInstance = new AnimationMap();
	}
	return m_animMapInstance;
}

void AnimationMap::cleanUp()
{
	std::map<std::string, Animation*>::iterator it;
	for (it = m_animationMap.begin(); it != m_animationMap.end(); it++) {
		delete it->second;
	}
	m_animationMap.clear();
}

bool AnimationMap::existsWithName(std::string name)
{
	if (m_animationMap.find(name) != m_animationMap.end())
		return true;
	return false;
}

Animation* AnimationMap::getAnimation(std::string name)
{
	if (existsWithName(name))
		return m_animationMap[name];
	return nullptr;
}

Animation* AnimationMap::createAnimation(std::string name, Animation animation)
{
	if (existsWithName(name))
		return nullptr;

	Animation* newAnimation = new Animation();
	*newAnimation = animation;
	m_animationMap[name] = newAnimation;
	return newAnimation;
}

void AnimationMap::destroy()
{
	cleanUp();
	delete m_animMapInstance;
	m_animMapInstance = nullptr;
}



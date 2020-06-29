#include "Pch/Pch.h"
#include "SkeletonMap.h"

SkeletonMap* SkeletonMap::m_skeletonMapInstance = 0;
SkeletonMap::SkeletonMap() {}

SkeletonMap* SkeletonMap::GetInstance()
{
	if (m_skeletonMapInstance == 0)
	{
		m_skeletonMapInstance = new SkeletonMap();
	}
	return m_skeletonMapInstance;
}

void SkeletonMap::cleanUp()
{
	std::map<std::string, Skeleton*>::iterator it;
	for (it = m_skeletonMap.begin(); it != m_skeletonMap.end(); it++) {
		delete it->second;
	}
	m_skeletonMap.clear();
}

bool SkeletonMap::existsWithName(std::string name)
{
	if (m_skeletonMap.find(name) != m_skeletonMap.end())
		return true;
	return false;
}

Skeleton* SkeletonMap::getSkeleton(std::string name)
{
	if (existsWithName(name))
		return m_skeletonMap[name];
	return nullptr;
}

Skeleton* SkeletonMap::createSkeleton(std::string name, Skeleton skeleton)
{
	if (existsWithName(name))
		return nullptr;

	Skeleton* newSkeleton = new Skeleton();
	*newSkeleton = skeleton;
	m_skeletonMap[name] = newSkeleton;
	return newSkeleton;
}

void SkeletonMap::destroy()
{
	cleanUp();
	delete m_skeletonMapInstance;
	m_skeletonMapInstance = nullptr;
}



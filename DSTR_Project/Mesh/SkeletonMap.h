#ifndef _SKELETONMAP_H
#define _SKELETONMAP_H
#include <Pch/Pch.h>
#include <Mesh/MeshFormat.h>

class SkeletonMap
{
public:
	static SkeletonMap* GetInstance();
	void cleanUp();
	bool existsWithName(std::string name);
	Skeleton* getSkeleton(std::string name);
	Skeleton* createSkeleton(std::string name, Skeleton skeleton);
	void destroy();
private:
	SkeletonMap();
	static SkeletonMap* m_skeletonMapInstance;
	std::map<std::string, Skeleton*> m_skeletonMap;
};

#endif
#ifndef _FRACTUREGENERATOR_h
#define _FRACTUREGENERATOR_h
#include <Pch/Pch.h>

#define JC_VORONOI_IMPLEMENTATION
#define JC_VORONOI_CLIP_IMPLEMENTATION
// If you wish to use doubles
//#define JCV_REAL_TYPE double
//#define JCV_ATAN2 atan2
//#define JCV_FLT_MAX 1.7976931348623157E+308
#include <DSTR-Project/jc_voronoi.h>
#include <DSTR-Project/jc_voronoi_clip.h>

#include <DSTR-Project/DebugObjects.h>

#include <DSTR-Project/DestructibleObject.h>
enum class VoronoiType
{
	FORTUNESWEEP,
	DELAUNAY
};

class FractureGenerator
{
public:
	FractureGenerator();
	~FractureGenerator();

	bool DebugFracture(DestructibleObject* object, glm::vec2 hitPosition, glm::vec3 hitDirection);
	bool DebugFracture(DestructibleObject* object, const glm::vec2 hitPosition, std::vector<GameObject*>& objectList, DebugFractureTimer* debugFracture);

	bool Fracture(DestructibleObject* object, const glm::vec2 hitPosition, const glm::vec3 hitDirection, std::vector<GameObject*>& objectList);

	void SetPointAmount(int amount);
	void SetRadiusSize(float radius);
	void SetForceStrength(float str);
	void SetBulletHoles(bool arg);
	void SetCirclePattern(bool arg);

	const int& GetPointAmount() { return m_pointCount; };
	const float& GetRadiusSize() { return m_circleRadius; };
	const float& GetForceStrength() { return m_forceAll_mod; };
	const bool& GetBulletHoles() { return m_bulletHoles; };
	const bool& GetCirclePattern() { return m_circlePattern; };
	const unsigned int seedRand(int seed = -1);
	const unsigned int getSeed(unsigned int seed) const { return m_seed; }


private:
	void RandomizePoints(const int& numpoints, const float& pointDiamater, jcv_point* j_points, const glm::vec2& hitPosition);
	bool MeshFromSite(Mesh*& mesh, const float& scale, const std::vector<glm::vec2>& siteVertices);

	void ApplyPhysicsProjectile(DestructibleObject* object, const glm::vec2& hitPosition, const glm::vec3& center, const glm::vec3& hitDirection, const int& mi = 0);
	void ApplyPhysicsExplosion(DestructibleObject* object, const glm::vec2& hitPosition, const glm::vec3& center, const glm::vec3& hitDirection, const int& mi = 0);
	void ApplyPhysicsFracture(DestructibleObject* object, const glm::vec2& hitPosition, const glm::vec3& center, const glm::vec3& hitDirection, const int& mi = 0);


	VoronoiType m_voronoiType;
	int m_seed;	// Seed for randomizer, used for network consistency


	// Settings
	int m_pointCount;
	int m_maxFratureDepth;
	bool m_circlePattern;
	bool m_bulletHoles;
	float m_circleRadius;
	float m_forceAll_mod;

	//jcv_point* m_j_points;
};

#endif


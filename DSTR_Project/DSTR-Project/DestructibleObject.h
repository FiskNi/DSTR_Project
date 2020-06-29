#ifndef _DESTRUCTIBLEOBJECT_h
#define _DESTRUCTIBLEOBJECT_h
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>

class DstrGenerator;
class FractureGenerator;


class DestructibleObject : public GameObject
{
public:
	DestructibleObject(DstrGenerator* dstr);
	DestructibleObject(FractureGenerator* fractureGen);
	~DestructibleObject();

	void Update(float dt);

	void LoadBasic(std::string name, float size);
	void LoadBasic2(std::string name, float size);
	void SetPolygon(std::vector <glm::vec2> vertices);
	void SetScale(float scale);


	void setLifetime(float time) { m_lifetime = time; }
	void SetDestroyed(bool state) { m_destroyed = state; }
	void SetFractureDepth(int depth) { m_fractureDepth = depth; }

	void CalculateMinMax();

	// Get
	const std::vector<glm::vec2>& GetPolygon() const { return m_polygonFace; }
	const glm::vec2& GetMin() const { return min; }
	const glm::vec2& GetMax() const { return max; }
	const float& getScale() const { return m_scale; }
	const float& getLifetime() const { return m_lifetime; }
	const float& GetFractureDepth() const { return m_fractureDepth; }
	const bool& IsDestroyed() const { return m_destroyed; }

	FractureGenerator* GetFractureGenerator() { return fractureGenRef; }
	
	
	// TODO:
	const std::vector<glm::vec2>& getUv() const { return m_polygonUV; }
	DstrGenerator* getDstr() { return dstrRef; }

	void loadDestructible(std::string fileName, float size);
	void loadDestructible(std::vector<Vertex> vertices_in, std::string name_in,
		Material newMaterial_in, std::string albedo_in, Transform transform, float size);
	void LoadBasic3(std::string name, float size);
	void LoadBasic4(std::string name, float size);
	void LoadDefined(std::string name, std::vector<glm::vec2> polygon);
	void FindVertices(const std::vector<Vertex>& vertices);
	void meshFromPolygon(std::string name);
	bool MeshFromSite(const float& scale, const std::vector<glm::vec2>& siteVertices);


private:
	std::vector<glm::vec2> m_polygonFace;
	std::vector<glm::vec2> m_polygonUV;
	glm::vec2 min;
	glm::vec2 max;

	float m_lifetime = 0.0f;
	float m_scale = 0.0f;
	bool m_destroyed = false;
	int m_fractureDepth = 0;

	DstrGenerator* dstrRef = nullptr;
	FractureGenerator* fractureGenRef = nullptr;
};


#endif


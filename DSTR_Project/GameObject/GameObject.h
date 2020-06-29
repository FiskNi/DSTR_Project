#ifndef	_GAMEOBJECT_h
#define _GAMEOBJECT_h
#include <Pch/Pch.h>
#include <Mesh/Mesh.h>
#include <GFX/MaterialMap.h>
#include <Mesh/Mesh.h>
#include <System/BulletPhysics.h>
#include <Renderer/Camera.h>

class GameObject {
public:
	//Create an Empty object
	GameObject();
	//Create an Empty object with a different name
	GameObject(std::string objectName);
	virtual ~GameObject();
	virtual void Update(float dt) = 0;
	
	//Loads all the meshes from the file into the GameObject
	void loadMesh(std::string fileName);
	void InitMesh(Mesh mesh);
	void InitMesh(Mesh* mesh);
	void InitMesh(const std::vector<Vertex>& vertices, const std::vector<Face>& faces, std::string name = std::string());
	void BindMaterialToShader(std::string shaderName, int meshIndex = 0);
	void BindMaterialToShader(Shader* shader, const int& meshIndex = 0);
	
	// Bullet
	void BulletMakeStatic();
	void UpdateBulletRigids();
	void SetTransformFromRigid(int meshIndex = 0);
	void BulletAddRigidBody(btRigidBody* body, int meshIndex = 0);
	void BulletCreateDynamic(CollisionObject shape = box, float weight = 1.0f, int meshIndex = 0, bool recenter = true);	
	void RemoveBody(int bodyIndex);
	
	void SetTexture(const char* fileName);
	   	
	//Set functions
	void SetTransform(Transform transform);
	void SetTransform(glm::vec3 worldPosition = glm::vec3(), glm::quat worldRot = glm::quat(), glm::vec3 worldScale = glm::vec3(1.0f));
	void SetMeshOffsetTransform(Transform transform, int meshIndex = 0);
	void SetObjectWorldPosition(glm::vec3 worldPosition);
	void SetObjectWorldRotation(glm::quat worldRotation);
	void SetObjectWorldScale(glm::vec3 scale);
	void SetMeshOffsetPosition(glm::vec3 position, int meshIndex = 0);
	void SetMeshOffsetRotation(glm::quat rotation, int meshIndex = 0);
	void SetBodyWorldPosition(glm::vec3 worldPosition, int meshIndex = 0);

	void SetName(std::string name) { m_objectName = name; };
	void SetShouldRender(bool condition);
	void SetMaterial(Material* material, int meshIndex = 0);


	//Get functions
	const std::string& GetName() const { return m_objectName; };

	Mesh* GetMesh(const int& meshIndex = 0); //Get a mesh from the meshbox
	Material* GetMeshMaterial(const int& meshIndex = 0); //Get a material from the meshbox

	const Transform GetMeshWorldTransform(int meshIndex = 0) const;
	const Transform GetObjectWorldTransform() const;
	const Transform GetLocalTransform(int meshIndex = 0) const;
	const Transform GetRigidTransform(int meshIndex = 0) const;
	const int getMeshesCount() const { return (int)m_meshes.size(); }
	const glm::mat4& GetMatrix(const int& i = 0) const;
	const int getType() const { return m_type; }
	const bool& GetShouldRender() const;
	const glm::vec3 GetLastPosition() const;	

	btRigidBody* getRigidBody(int meshIndex = 0) const { return m_meshes[meshIndex].body; }


	void UpdateModelMatrices();

private:
	struct MeshObject // Handles seperate transforms for same mesh
	{
		Mesh* mesh;
		Material* material;
		Transform transform;

		btRigidBody* body;

		MeshObject()
		{
			mesh = nullptr;
			material = nullptr;
			body = nullptr;
		}
	};

	std::string m_objectName;
	Transform m_transform;
	glm::vec3 m_lastPosition;

	// Allocation for perfomance
	Transform t_transform;

protected:
	int m_type;	
	std::vector<MeshObject> m_meshes;
	std::vector<glm::mat4> m_modelMatrixes;

	bool m_shouldRender;
};


#endif
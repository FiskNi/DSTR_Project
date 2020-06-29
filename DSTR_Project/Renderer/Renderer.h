#ifndef _RENDERER_h
#define _RENDERER_h

//Define the names of the shaders
#define LIGHT_CULL "Light_Cull"
#define BASIC_FORWARD "Basic_Forward"
#define DEPTH_MAP "Depth_Map"
#define TRANSPARENCY "Transparent_Render"
#define SKYBOX "Skybox_Shader"
#define DEBUGDRAWER "Debug_Drawer"

//Max number of lights
#define P_LIGHT_COUNT 64

#pragma region Includes
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
#include <Mesh/MeshFormat.h>
#include <Renderer/SkyBox.h>
#include <System/Timer.h>
#include "NotificationStructure.h"
#include <GFX/Pointlight.h>
#pragma endregion

#include <System/MemoryUsage.h>

#define P_LIGHT_COUNT 64
#define P_LIGHT_RADIUS 5

struct ObjectRenderData 
{
	Buffers buffer;
	glm::vec3 worldPos;
};

struct LightIndex 
{
	int index[P_LIGHT_COUNT];
};

struct PLIGHT 
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec4 attenAndRadius;
	float strength;
	int index;
};

enum RENDER_TYPE 
{
	STATIC,
	DYNAMIC,
	ANIMATEDSTATIC,
	ANIMATEDDYNAMIC,
	POINTLIGHT_SOURCE,
	SKYOBJECTS
};

class Renderer
{
private:
	static Renderer* m_rendererInstance;
	GLFWwindow* m_gWindow;
	Camera* m_camera;
	SkyBox* m_skyBox;
	Timer m_timer;

	//Store gameobjects directly to the renderer
	std::vector<GameObject*> m_staticObjects;
	std::vector<GameObject*> m_dynamicObjects;
	std::vector<GameObject*> m_anistaticObjects;
	std::vector<GameObject*> m_anidynamicObjects;
	std::vector<GameObject*> m_skyObjects;
	std::vector<PLIGHT> m_lights;

	//Buffers
	unsigned int m_depthFBO;
	unsigned int m_depthMap;

	//unsigned int m_hdrFbo;
	unsigned int m_colourBuffer;
	unsigned int m_rbo;
	bool m_renderedDepthmap;

	//Storage Buffer for light indices
	unsigned int m_lightIndexSSBO;
	glm::uvec2 workGroups;

	void createDepthMap();
	void initShaders();


	Renderer();
	~Renderer();
public:
	static Renderer* GetInstance();

	void init(GLFWwindow* window);
	void setupCamera(Camera* camera);

	void destroy();
	void clear();
	//SUBMIT POINTLIGHTS BY IN THEM HERE
	void submit(GameObject* gameObject, RENDER_TYPE objType);
	void submitSkybox(SkyBox* skybox);
	void removeRenderObject(GameObject* gameObject, RENDER_TYPE objType); //Remove an object from the dynamic array
	void renderSkybox();
	void Render();
	void renderDepthmap(); //Generate a depthmap

	void BindMatrixes(const std::string& shaderName);
	void BindMatrixes(Shader* shader);
	
	Camera* getMainCamera() const;
};

#endif

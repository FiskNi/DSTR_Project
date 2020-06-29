#ifndef _PCH_H
#define _PCH_H


#include <System/DeltaTime.h>
#include <System/Framerate.h>
#include "Bullet/btBulletDynamicsCommon.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <limits.h>
#include <time.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <math.h>
#include <chrono>
#include <thread>
#include <string>
#include <stdio.h>
#include <future>
#include <functional>
#include <queue>
#include <chrono>


#define WIN32_LEAN_AND_MEAN

#include <d3d11_4.h>
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")
#include <psapi.h>

//GL related includes
#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/common.hpp>
#include "glm/ext.hpp"
#include <System/Lerp.h>
#include <glm/gtx/matrix_decompose.hpp>

#include "System/Log.h"
#include "System/Randomizer.h"

//Custom
#include <Mesh/MeshFormat.h>
#include <Mesh/Mesh.h>
#include <Mesh/MeshMap.h>
#include <Mesh/SkeletonMap.h>
#include <Mesh/AnimationMap.h>
#include <Renderer/Shader.h>
#include <Renderer/ShaderMap.h>
#include <Renderer/Camera.h>
#include <Renderer/Renderer.h>
#include <System/Input.h>
#include <GameObject/ObjectTypes.h>	

// DSTR
#include <Destruction/DstrFormats.h>
#include <Destruction/TriangleNode.h>
#include <Destruction/Geometry.h>
#include <Destruction/DelaunayGenerator.h>
#include <Destruction/VoroniCalculator.h>
#include <Destruction/VoroniClipper.h>


#include <Destruction/DstrGenerator.h>
#include <DSTR-Project/FractureGenerator.h>


const std::string SHADERPATH = "Assets/Shaders/";
const std::string TEXTUREPATH = "Assets/Textures/";
const std::string MESHPATH = "Assets/Meshes/";
const std::string SOUNDEFFECTPATH = "Assets/SoundEffects/";

const std::string CHARACTER = "CharacterBody";

const unsigned int SCREEN_WIDTH = 1280;
const unsigned int SCREEN_HEIGHT = 720;
const int MAX_BONES = 64;

constexpr int SPACE_ASCII = 32;
constexpr int DESIRED_PADDING = 6;
constexpr float LINE_HEIGHT = 0.3f;

#endif

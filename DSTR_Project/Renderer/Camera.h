#pragma once
#include <Pch/Pch.h>

enum class CameraMode
{
	FreeCamera,
	FirstPerson
};

class Camera
{
private:
	// Camera modes
	void freeCameraMode();
	void LE_orbitCamera();

	void resetMouseToMiddle();

	float m_cameraMoveSpeed;

	void calcVectors();
	CameraMode m_cameraMode;
	bool m_firstMouse = true;
	double m_xpos, m_ypos;
	float m_lastX, m_lastY;

	glm::vec3 m_camPos;
	glm::vec3 m_camFace;
	glm::vec3 m_worldUp;
	glm::vec3 m_camUp;
	glm::vec3 m_camRight;

	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewMatrix;

	float m_camYaw;
	float m_camPitch;
	float m_width;
	float m_height;
	float m_nearPlane;
	float m_farPlane;

	void updateMouseMovement();
	void updateThirdPersonMouseMovement();
	bool m_fpEnabled;
	bool m_activeCamera;
	bool m_lock = false;
	glm::vec2 oldPosition;

public:
	Camera();
	void InitSettings();
	Camera(glm::vec3 pos, float yaw, float pitch);
	~Camera();

	void setWindowSize(float width, float height);
	void mouseControls(float xOffset, float yOffset, bool pitchLimit);
	void setProjMat(float width, float height, float nearPlane, float farPlane);
	void setCameraPos(const glm::vec3& pos);
	void lookAt(const glm::vec3& position);
	void disableCameraMovement(const bool condition);
	void resetCamera();

	const glm::mat4 getViewMat() const;
	const glm::mat4& getProjMat() const;
	const double& getXpos() const;
	const double& getYpos() const;

	const float& getPitch() const;
	const float& getYaw() const;
	const glm::vec3& getCamPos() const;
	const bool& isCameraActive() const;

	const glm::vec3& getCamFace();
	const glm::vec3& getCamRight();
	const glm::vec3& getCamUp();	

	void Update();
	void updateLevelEd();
	
};


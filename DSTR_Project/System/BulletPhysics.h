#ifndef BULLETPHYSICS_H
#define BULLETPHYSICS_H
#include <Pch/Pch.h>
#include <Bullet/BulletDynamics/Character/btKinematicCharacterController.h>
#include <Bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include "BulletDebugDrawer.h"

// box, sphere, capsule
enum CollisionObject
{
	box,
	sphere,
	capsule
};

class BulletPhysics
{
public:
	BulletPhysics(float gravity);
	~BulletPhysics();
	static BulletPhysics* GetInstance();
	void destroy();
	void Update(float dt);

	//objects box, sphere, capsule. (Extend.x is radius, Extend.y is height) 
	btRigidBody* createObject(CollisionObject object, float inMass,
		glm::vec3 position, glm::vec3 extend, glm::quat rotation = glm::quat(),
		bool destruction = false, float restitution = 1.0f, float friction = 0.2f);

	btKinematicCharacterController* createCharacter(const glm::vec3& position);
	
	btDiscreteDynamicsWorld* getDynamicsWorld() const;
	btVector3 getCharacterSize() const;

	void setCharacterSize(glm::vec3 halfSize);
	void removeObject(btRigidBody* body);

private:
	static BulletPhysics* m_bpInstance;

	void destructionobj(btRigidBody* body);




	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btCollisionDispatcher* m_dispatcher;
	btBroadphaseInterface* m_overlappingPairCache;
	btSequentialImpulseConstraintSolver* m_solver;
	btDiscreteDynamicsWorld* m_dynamicsWorld;


	btAlignedObjectArray<btCollisionShape*> m_collisionShapes;

	//character controller
	btConvexShape* m_playerShape;
	btPairCachingGhostObject* m_ghostObject;
	btKinematicCharacterController* m_character;
	btGhostPairCallback* m_ghostCallback;

	int m_counter = 0;
	bool m_setGravity = false;
	btVector3 m_boxSize;


	BulletDebugDrawer debugDrawer;

};

#endif

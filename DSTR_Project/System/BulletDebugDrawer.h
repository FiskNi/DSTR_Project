#ifndef BULLETDEBUGDRAWER_H
#define BULLETDEBUGDRAWER_H
#include <Bullet/LinearMath/btIDebugDraw.h>


class BulletDebugDrawer :
	public btIDebugDraw
{
public:

    BulletDebugDrawer();

	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	virtual void drawContactPoint(const btVector3&, const btVector3&, btScalar, int, const btVector3&) {}
	virtual void reportErrorWarning(const char*) {}
	virtual void draw3dText(const btVector3&, const char*) {}
	virtual void setDebugMode(int p) {}
	int getDebugMode(void) const { return 3; }

private:
	int m_debugMode;
	GLuint VBO, VAO;
};


#endif



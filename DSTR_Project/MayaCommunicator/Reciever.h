#ifndef _RECIEVER_H
#define _RECIEVER_H
#include <Pch/Pch.h>
#include <MayaCommunicator/Comdata.h>
#include "MayaCom.h"

struct IncomingPacket
{
	ComData::DataType type;
	bool headerRead = false;
	bool contentRead = false;


	bool ready = true;

	void Clear()
	{
		type = ComData::NONE;
		headerRead = false;
		contentRead = false;

		ready = true;
	}
};

struct IncomingMesh
{
	// If this is true the mesh can be sent to the rendering pipeline
	bool meshReady = false;

	// This should be taken over by the MeshMap
	// Memory needs to be cleaned if it's not sent there
	Mesh *mesh = nullptr;
	bool meshRead = false;

	int vertexCount = 0;
	int currVertReadOffset = 0;
	std::vector<Vertex> vertices;
	std::vector<bool> vertexRead;
	bool verticesRead = false;

	int faceCount = 0;
	int currFaceReadOffset = 0;
	std::vector<Face> faces;
	std::vector<bool> faceRead;
	bool facesRead = false;

	void Clear()
	{
		meshReady = false;

		mesh = nullptr;
		meshRead = false;

		// Clear vertices
		vertexCount = 0;
		currVertReadOffset = 0;
		vertices.clear();
		vertexRead.clear();
		verticesRead = false;

		// Clear faces
		faceCount = 0;
		currFaceReadOffset = 0;
		faces.clear();
		faceRead.clear();
		facesRead = false;
	}

	void PrepVertices(int count)
	{
		vertexCount = count;
		vertices.resize(count);
		vertexRead.resize(count);
	}
	void PrepFaces(int count)
	{
		faceCount = count;
		faces.resize(count);
		faceRead.resize(count);
	}

};


struct IncomingTransform
{
	bool transformReady = false;

	std::string name;
	Transform transform;
	bool transformRead = false;

	void Clear()
	{
		transformReady = false;
		name.clear();
	}
};



class Reciever
{
public:
	static void Init();
	static Reciever* GetInstance();
	void Recieve();
	bool GetMesh(Mesh*& mesh);
	bool UpdateTransform(std::string& name, Transform& transform);

	void ClearPackets();

	void Destroy();

private:
	Reciever();
	static Reciever* m_senderInstance;

	MayaCom* mayaCom;

	// Data sizes
	size_t s_headerSize;
	size_t s_meshSize;
	size_t s_vertexSize;
	size_t s_faceSize;
	size_t s_transformSize;

	IncomingPacket m_incomingPacket;
	IncomingMesh m_incomingMesh;
	IncomingTransform m_incomingTransform;


};

#endif


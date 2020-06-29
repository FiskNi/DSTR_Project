#include "Pch/Pch.h"
#include "Reciever.h"

Reciever* Reciever::m_senderInstance = 0;

Reciever::Reciever()
{
	mayaCom = new MayaCom();

	mayaCom->CreateFileMap();
	mayaCom->GetMap();

	s_headerSize = sizeof(ComData::Header);
	s_meshSize = sizeof(ComData::Mesh);
	s_vertexSize = sizeof(ComData::Vertex);
	s_faceSize = sizeof(ComData::Face);
	s_transformSize = sizeof(ComData::TransformChange);
}

void Reciever::Init()
{
	if (m_senderInstance == 0)
		m_senderInstance = new Reciever();
}

Reciever* Reciever::GetInstance()
{
	if (m_senderInstance == 0)
		m_senderInstance = new Reciever();
	return m_senderInstance;
}

void Reciever::Recieve()
{
	// Check if we can a new packet
	if (m_incomingPacket.ready)
	{
		// Check if we can read memory
		if (mayaCom->CanRead(s_headerSize))
		{
			// Read header
			ComData::Header header;
			mayaCom->Read((char*)&header, s_headerSize);

			m_incomingPacket.type = header.dataType;
			m_incomingPacket.headerRead = true;
			m_incomingPacket.ready = false;
		}
	}

	// If packet is not ready content is in the process of reading
	if (!m_incomingPacket.ready)
	{
		// Check if the main header has been read, if not error has occured
		if (m_incomingPacket.headerRead)
		{
			// Check type based on header
			switch (m_incomingPacket.type)
			{
			case ComData::MESH:
			{
				// Check if mesh header has been read
				if (!m_incomingMesh.meshRead)
				{
					// Check if we can read memory
					if (mayaCom->CanRead(s_meshSize))
					{
						// Read mesh
						ComData::Mesh meshData;
						mayaCom->Read((char*)&meshData, s_meshSize);

						m_incomingMesh.mesh = new Mesh();
						m_incomingMesh.mesh->nameMesh(meshData.name);
						m_incomingMesh.PrepVertices(meshData.vertexCount);
						m_incomingMesh.PrepFaces(meshData.faceCount);

						m_incomingMesh.meshRead = true;
					}
				}

				if (m_incomingMesh.meshRead)
				{
					// Check for read vertices and read as many as possible, else try again next loop
					for (int i = m_incomingMesh.currVertReadOffset; i < m_incomingMesh.vertexCount; i++)
					{
						if (!m_incomingMesh.vertexRead[i])
						{
							// Check if we can read again after each read vertex
							if (mayaCom->CanRead(s_vertexSize))
							{
								ComData::Vertex vertexData;
								mayaCom->Read((char*)&vertexData, s_vertexSize);

								for (int a = 0; a < 3; a++)
									m_incomingMesh.vertices[i].position[a] = vertexData.position[a];
								for (int a = 0; a < 2; a++)
									m_incomingMesh.vertices[i].UV[a] = vertexData.uv[a];
								for (int a = 0; a < 3; a++)
									m_incomingMesh.vertices[i].Normals[a] = vertexData.normal[a];


								logTrace(vertexData.position[0]);

								m_incomingMesh.vertexRead[i] = true;
								m_incomingMesh.currVertReadOffset = i + 1;

								if (i == m_incomingMesh.vertexCount - 1)
									m_incomingMesh.verticesRead = true;

							}
							else
							{
								break;
							}
						}
						else
						{
							break;
						}
					}


					if (m_incomingMesh.verticesRead)
					{
						for (int i = m_incomingMesh.currFaceReadOffset; i < m_incomingMesh.faceCount; i++)
						{
							if (!m_incomingMesh.faceRead[i])
							{
								// Check if we can read again after each read face
								if (mayaCom->CanRead(s_faceSize))
								{
									ComData::Face faceData;
									mayaCom->Read((char*)&faceData, s_faceSize);

									for (int a = 0; a < 3; a++)
										m_incomingMesh.faces[i].indices[a] = faceData.indices[a];

									m_incomingMesh.faceRead[i] = true;
									m_incomingMesh.currFaceReadOffset = i + 1;

									if (i == m_incomingMesh.faceCount - 1)
										m_incomingMesh.facesRead = true;

								}
								else
								{
									break;
								}
							}
							else
							{
								break;
							}
						}
					}

				}


				// Declare packet has been read
				if (m_incomingMesh.facesRead)
				{
					m_incomingMesh.mesh->setUpMesh(m_incomingMesh.vertices, m_incomingMesh.faces);
					m_incomingMesh.mesh->setUpBuffers();
					m_incomingMesh.meshReady = true;
						
					m_incomingPacket.contentRead = true;
				}

				// Case end
				break;
			}

			case ComData::MESHTRANSFORMED:
			{
				
				// Check if mesh header has been read
				if (!m_incomingTransform.transformReady)
				{
					if (mayaCom->CanRead(s_transformSize))
					{
						// Read mesh
						ComData::TransformChange transformData;
						mayaCom->Read((char*)&transformData, s_transformSize);

						m_incomingTransform.name = transformData.name;

						m_incomingTransform.transform.position = glm::make_vec3(transformData.translation);
						m_incomingTransform.transform.rotation = glm::make_quat(transformData.rotation);
						m_incomingTransform.transform.scale = glm::make_vec3(transformData.scale);

						m_incomingTransform.transformRead = true;
						m_incomingTransform.transformReady = true;

						m_incomingPacket.contentRead = true;
					}
				}
				

				// Case end
				break;
			}



			default:
				break;
			}


			// Switch end
		}
		
	}




	/*if (m_incomingPacket.contentRead)
		ClearPackets();*/
	
}

bool Reciever::GetMesh(Mesh*& mesh)
{
	if (m_incomingMesh.meshReady)
	{
		mesh = m_incomingMesh.mesh;
		ClearPackets();
		return true;
	}
	else
	{
		mesh = nullptr;
		return false;
	}


	return false;
}

bool Reciever::UpdateTransform(std::string& name, Transform& transform)
{
	if (m_incomingTransform.transformReady)
	{
		transform = m_incomingTransform.transform;
		name = m_incomingTransform.name;
		ClearPackets();
		return true;
	}

	return false;
}

void Reciever::ClearPackets()
{
	m_incomingMesh.Clear();
	m_incomingTransform.Clear();

	m_incomingPacket.Clear();
}

void Reciever::Destroy()
{
	mayaCom->UnmapView();
	mayaCom->CloseFile();
	if (mayaCom)
		delete mayaCom;

	delete m_senderInstance;
	m_senderInstance = nullptr;
}

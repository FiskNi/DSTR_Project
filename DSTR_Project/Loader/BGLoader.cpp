#include <Pch/Pch.h>
#include "BGLoader.h"

BGLoader::BGLoader()
{
	fileName = "";
	meshCount = 0;
	matCount = 0;

	meshGroup = nullptr;
	material = nullptr;
	loaderMesh = nullptr;
	dirLight = nullptr;
	pointLight = nullptr;

}

BGLoader::BGLoader(std::string fileName)
{
	this->fileName = fileName;
	meshCount = 0;
	matCount = 0;

	meshGroup = nullptr;
	material = nullptr;
	loaderMesh = nullptr;
	dirLight = nullptr;
	pointLight = nullptr;

	LoadMesh(fileName);
}

BGLoader::~BGLoader()
{
	Unload();
}

void BGLoader::Unload()
{	
	for (bggMeshData bggMesh : bggMeshes)
	{
		bggMesh.bggSkeleVertices.clear();
		bggMesh.bggVertices.clear();
		bggMesh.bggFaces.clear();
		bggMesh.bggAnimation.clear();

	}
	bggMeshes.clear();
	bggMaterials.clear();

	for (LoaderFormat::Vertex* v : meshVert)
		delete[] v;
	meshVert.clear();

	for (LoaderFormat::Face* f : meshFace)
		delete[] f;
	meshFace.clear();

	animationsD.clear();
	skeletonsD.clear();


	if (meshGroup)
		delete[] meshGroup;
	if (material)
		delete[] material;
	if (loaderMesh)	
		delete[] loaderMesh;
	if (dirLight)
		delete[] dirLight;
	if (pointLight)
		delete[] pointLight;

	meshGroup = nullptr;
	material = nullptr;
	loaderMesh = nullptr;
	dirLight = nullptr;
	pointLight = nullptr;

	fileHeader.meshCount = 0;
	fileHeader.groupCount = 0;
	fileHeader.materialCount = 0;
	fileHeader.pointLightCount = 0;
	fileHeader.dirLightCount = 0;


	fileName = "";
}

void BGLoader::BGFormatData()
{
	meshCount = fileHeader.meshCount;
	matCount = fileHeader.materialCount;
	
	// Mesh vector
	bggMeshes.resize(meshCount);

	// Material vector
	bggMaterials.resize(matCount);

	for (int meshId = 0; meshId < meshCount; meshId++)
	{
		bggMeshes[meshId].bggVertices.resize(GetVertexCount(meshId));
		bggMeshes[meshId].bggFaces.resize(GetFaceCount(meshId));
		// Vertices
		for (size_t v = 0; v < bggMeshes[meshId].bggVertices.size(); v++)
		{
			Vertex& vert = bggMeshes[meshId].bggVertices[v];
			vert.position[0] = meshVert[meshId][v].position[0];
			vert.position[1] = meshVert[meshId][v].position[1];
			vert.position[2] = meshVert[meshId][v].position[2];
			vert.UV[0] = meshVert[meshId][v].uv[0];
			vert.UV[1] = meshVert[meshId][v].uv[1];
			vert.Normals[0] = meshVert[meshId][v].normal[0];
			vert.Normals[1] = meshVert[meshId][v].normal[1];
			vert.Normals[2] = meshVert[meshId][v].normal[2];
			//vert.Tangent[0] = meshVert[meshId][v].tangent[0];
			//vert.Tangent[1] = meshVert[meshId][v].tangent[1];
			//vert.Tangent[2] = meshVert[meshId][v].tangent[2];
		}
		// Faces
		for (size_t f = 0; f < bggMeshes[meshId].bggFaces.size(); f++)
		{
			Face& face = bggMeshes[meshId].bggFaces[f];
			face.indices[0] = meshFace[meshId][f].indices[0];
			face.indices[1] = meshFace[meshId][f].indices[1];
			face.indices[2] = meshFace[meshId][f].indices[2];
		}
		
		// Transforms
		bggMeshes[meshId].bggPositions = glm::make_vec3(loaderMesh[meshId].translation);

		glm::vec3 rotation1 = glm::make_vec3(loaderMesh[meshId].rotation);
		glm::vec3 rads1 = glm::radians(rotation1);
		glm::quat quat1 = glm::quat(rads1);
		bggMeshes[meshId].bggRotation = glm::quat(glm::radians(glm::make_vec3(loaderMesh[meshId].rotation)));
		bggMeshes[meshId].bggScale = glm::make_vec3(loaderMesh[meshId].scale);

		Transform newTransform;
		newTransform.position = bggMeshes[meshId].bggPositions;
		newTransform.rotation = bggMeshes[meshId].bggRotation;
		newTransform.scale = bggMeshes[meshId].bggScale;
		bggMeshes[meshId].bggTransforms = newTransform;

		// Skeleton
		if (loaderMesh[meshId].skeleton.jointCount > 0)
		{
			bggMeshes[meshId].bggVertices.clear();
			bggMeshes[meshId].bggSkeleVertices.resize(GetVertexCount(meshId));
			for (size_t v = 0; v < bggMeshes[meshId].bggSkeleVertices.size(); v++)
			{
				Vertex2& vert = bggMeshes[meshId].bggSkeleVertices[v];
				vert.position[0] = meshVert[meshId][v].position[0];
				vert.position[1] = meshVert[meshId][v].position[1];
				vert.position[2] = meshVert[meshId][v].position[2];
				vert.UV[0] = meshVert[meshId][v].uv[0];
				vert.UV[1] = meshVert[meshId][v].uv[1];
				vert.Normals[0] = meshVert[meshId][v].normal[0];
				vert.Normals[1] = meshVert[meshId][v].normal[1];
				vert.Normals[2] = meshVert[meshId][v].normal[2];

				vert.bone[0] = (int)meshVert[meshId][v].bone[0];
				vert.bone[1] = (int)meshVert[meshId][v].bone[1];
				vert.bone[2] = (int)meshVert[meshId][v].bone[2];
				vert.bone[3] = (int)meshVert[meshId][v].bone[3];

				vert.weight[0] = meshVert[meshId][v].weight[0];
				vert.weight[1] = meshVert[meshId][v].weight[1];
				vert.weight[2] = meshVert[meshId][v].weight[2];
				vert.weight[3] = meshVert[meshId][v].weight[3];
			}


			//bggMeshes[meshId].bggSkeleton.name = loaderMesh[meshId].skeleton.name;
			bggMeshes[meshId].bggSkeleton.name = skeletonsD[meshId].joint[0].name;
			bggMeshes[meshId].bggSkeleton.joints.resize(loaderMesh[meshId].skeleton.jointCount);

			for (size_t j = 0; j < bggMeshes[meshId].bggSkeleton.joints.size(); j++)
			{
				bggMeshes[meshId].bggSkeleton.joints[j].name = skeletonsD[meshId].joint[j].name;
				bggMeshes[meshId].bggSkeleton.joints[j].invBindPose = glm::make_mat4(skeletonsD[meshId].joint[j].invBindPose);
				bggMeshes[meshId].bggSkeleton.joints[j].parentIndex = skeletonsD[meshId].joint[j].parentIndex;
			}

			// Animation resize
			bggMeshes[meshId].bggAnimation.resize(loaderMesh[meshId].skeleton.aniCount);
		}

		// Animation
		for (size_t i = 0; i < bggMeshes[meshId].bggAnimation.size(); i++)
		{
			Animation& ani		= bggMeshes[meshId].bggAnimation[i];
			ani.name			= animationsD[meshId].animations[0].ani.name;
			ani.duration		= animationsD[meshId].animations[0].ani.duration;
			ani.rate			= animationsD[meshId].animations[0].ani.rate;
			ani.keyframeFirst	= animationsD[meshId].animations[0].ani.keyframeFirst;
			ani.keyframeLast	= animationsD[meshId].animations[0].ani.keyframeLast;

			ani.keyframes.resize(animationsD[meshId].animations[0].ani.keyframeCount);
			for (size_t k = 0; k < ani.keyframes.size(); k++)
			{
				ani.keyframes[k].id = animationsD[meshId].animations[0].keyFrames[k].key.id;

				int transformCount = (int)animationsD[meshId].animations[0].keyFrames[k].transforms.size();
				ani.keyframes[k].local_joint_t.resize(transformCount);
				for (int t = 0; t < transformCount; t++)
				{
					Transform tempTransform;
					tempTransform.position = glm::make_vec3(animationsD[meshId].animations[0].keyFrames[k].transforms[t].t.transform);
					tempTransform.rotation = glm::make_quat(animationsD[meshId].animations[0].keyFrames[k].transforms[t].t.rotate);
					tempTransform.scale = glm::make_vec3(animationsD[meshId].animations[0].keyFrames[k].transforms[t].t.scale);

					ani.keyframes[k].local_joint_t[t].transform = tempTransform;
					ani.keyframes[k].local_joint_t[t].jointid = animationsD[meshId].animations[0].keyFrames[k].transforms[t].t.joinId;
				}
			}
		}
	}

	for (int i = 0; i < matCount; i++)
	{
		// Material
		bggMaterials[i].name = (std::string)material[i].name;
		bggMaterials[i].ambient = glm::make_vec3(material[i].ambient);
		bggMaterials[i].diffuse = glm::make_vec3(material[i].diffuse);
		bggMaterials[i].specular = glm::make_vec3(material[i].specular);
	}
}

bool BGLoader::LoadMesh(std::string file)
{
	Unload();
	fileName = file;

	std::ifstream binFile(fileName, std::ios::binary);
	if (!binFile)
	{
		logError("Error! Could not find importer file: ", fileName);
		return false;
	}
	else
	{
		
		// Allocate memory based on header
		binFile.read((char*)&fileHeader, sizeof(LoaderFormat::BGHeader));
		meshGroup = new LoaderFormat::MeshGroup[fileHeader.groupCount];
		loaderMesh = new LoaderFormat::LoaderMesh[fileHeader.meshCount];
		meshVert.resize(fileHeader.meshCount);
		meshFace.resize(fileHeader.meshCount);
		material = new LoaderFormat::PhongMaterial[fileHeader.materialCount];
		dirLight = new LoaderFormat::DirLight[fileHeader.dirLightCount];
		pointLight = new LoaderFormat::PointLight[fileHeader.pointLightCount];

		logTrace("Opening file " + (std::string)fileName + 
			", Meshes: " + std::to_string(fileHeader.meshCount) + 
			", Materials: " + std::to_string(fileHeader.materialCount));

		
		// Fill data
		for (int i = 0; i < fileHeader.groupCount; i++)
		{
			binFile.read((char*)&meshGroup[i], sizeof(LoaderFormat::MeshGroup));
		}

		for (int i = 0; i < fileHeader.meshCount; i++)
		{
			binFile.read((char*)&loaderMesh[i], sizeof(LoaderFormat::LoaderMesh));

			// Allocate memory for the array of vertex arrays
			meshVert[i] = new LoaderFormat::Vertex[loaderMesh[i].vertexCount];

			//Read data for all the vertices, this includes pos, uv, normals, tangents and binormals.
			for (int v = 0; v < loaderMesh[i].vertexCount; v++)
			{
				binFile.read((char*)&meshVert[i][v], sizeof(LoaderFormat::Vertex));
			}

			// Allocate memory for the array of face arrays
			meshFace[i] = new LoaderFormat::Face[loaderMesh[i].faceCount];
			//Read data for all the vertices, this includes pos, uv, normals, tangents and binormals.
			for (int f = 0; f < loaderMesh[i].faceCount; f++)
			{
				binFile.read((char*)&meshFace[i][f], sizeof(LoaderFormat::Face));
			}

			// 3.3 Joints
			LoaderFormat::MeshSkeleton newSkeleton;
			// Allocate memory for the joint vector inside
			newSkeleton.joint.resize(loaderMesh[i].skeleton.jointCount);
			for (int j = 0; j < loaderMesh[i].skeleton.jointCount; j++)
			{
				LoaderFormat::Joint newJoint;
				binFile.read((char*)&newJoint, sizeof(LoaderFormat::Joint));
				newSkeleton.joint[j] = newJoint;
			}


			LoaderFormat::MeshAnis newAnimations;
			// Allocate memory for the animation vector inside
			newAnimations.animations.resize(loaderMesh[i].skeleton.aniCount);
			for (int a = 0; a < loaderMesh[i].skeleton.aniCount; a++)
			{
				// 3.4.1 Animations
				LoaderFormat::Animation newAni;
				binFile.read((char*)&newAni, sizeof(LoaderFormat::Animation));
				// Apply the data about the animation and
				// Allocate memory for the keyframe vector inside
				newAnimations.animations[a].ani = newAni;
				newAnimations.animations[a].keyFrames.resize(newAni.keyframeCount);
				for (int k = 0; k < newAni.keyframeCount; k++)
				{
					// 3.4.2 Keyframes
					LoaderFormat::KeyFrame newKey;
					binFile.read((char*)&newKey, sizeof(LoaderFormat::KeyFrame));
					// Apply the data about the keyframe and
					// Allocate memory for the transform vector inside 
					newAnimations.animations[a].keyFrames[k].key = newKey;
					newAnimations.animations[a].keyFrames[k].transforms.resize(newKey.transformCount);
					for (int t = 0; t < newKey.transformCount; t++)
					{
						// 3.4.3 Transforms
						LoaderFormat::Transform newTr;
						binFile.read((char*)&newTr, sizeof(LoaderFormat::Transform));
						// Apply the data about the transform
						newAnimations.animations[a].keyFrames[k].transforms[t].t = newTr;
					}
				}
			}

			animationsD.push_back(newAnimations);
			skeletonsD.push_back(newSkeleton);
		}

		for (int i = 0; i < fileHeader.materialCount; i++)
		{
			binFile.read((char*)&material[i], sizeof(LoaderFormat::PhongMaterial));
		}

		for (int i = 0; i < fileHeader.dirLightCount; i++)
		{
			binFile.read((char*)&dirLight[i], sizeof(float) * 10);
		}
		for (int i = 0; i < fileHeader.pointLightCount; i++)
		{
			binFile.read((char*)&pointLight[i], sizeof(float) * 7);
		}
	}
	binFile.close();

	// Parse data to a more comfortable format
	BGFormatData();

	return true;
}

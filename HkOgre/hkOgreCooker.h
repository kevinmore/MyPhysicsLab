#ifndef HKOGRE_COOKER
#define HKOGRE_COOKER
#include <hkOgrePrerequisites.h>

#include <Ogre.h>
using namespace std;

namespace HkOgre
{
	namespace Cooker
	{
		struct OgreMeshData
		{
				OgreMeshData() {}
				OgreMeshData(Ogre::Vector3* iVertices, unsigned long* iIndices, int iFaceNum, int iVertNum)
				{
						vertices        = iVertices;
						indices         = iIndices;
						//faces = iFaces;
						faceCount       = iFaceNum;
						vertCount       = iVertNum;
				}

				friend ostream& operator<<(ostream& os, const OgreMeshData& data)
				{
						os << "\n------- GENERAL INFO -------\n";
						os << "Vertex Count:    " << data.vertCount << std::endl;
						os << "Triangle Count:  " << data.faceCount << std::endl;
						os << "------- /GENERAL INFO -------\n";
						os << "------- VERTICES -------\n";
						for (unsigned int i = 0; i < data.vertCount; i++)
								os << data.vertices[i] << "\n";
						os << "------- /VERTICES -------\n";
						os << "------- TRIANGLES -------\n";
						for (unsigned int i = 0; i < data.faceCount; i++)
								os << data.faces[i] << "\n";
						os << "------- /TRIANGLES -------\n\n";
			
						os << "------- NORMALS -------\n";
						for (unsigned int i = 0; i < data.vertCount; i++)
								os << data.normals[i] << "\n";
						os << "------- / NORMALS -------\n\n";
				
						return os;
				}

				unsigned int vertCount;
				unsigned int faceCount;
				unsigned int indexCount;

				Ogre::Vector3* vertices;
				Ogre::Vector3* normals;
				unsigned long* indices;
				Ogre::Vector3* faces;
		};

		inline OgreMeshData getDataFromMesh(Ogre::MeshPtr mesh, const Ogre::Vector3 position, const Ogre::Quaternion orient, const Ogre::Vector3 scale)
		{
				OgreMeshData result;
		//      ofstream st;
		//      st.open("out.log");
				result.indexCount = 0;
				result.vertCount = 0;

				bool added_shared = false;
				size_t current_offset = result.vertCount;
				size_t shared_offset = result.vertCount;
				size_t next_offset = result.vertCount;
				size_t index_offset = result.indexCount;
				size_t prev_vert = result.vertCount;
				size_t prev_ind = result.indexCount;
				unsigned int last_Offset = 0;
				// Calculate how many vertices and indices we're going to need
				for(int i = 0;i < mesh->getNumSubMeshes();i++)
				{
						Ogre::SubMesh* submesh = mesh->getSubMesh(i);

						// We only need to add the shared vertices once
						if(submesh->useSharedVertices)
						{
								if(!added_shared)
								{
										Ogre::VertexData* vertex_data = mesh->sharedVertexData;
										result.vertCount += vertex_data->vertexCount;
										added_shared = true;
								}
						}
						else
						{
								Ogre::VertexData* vertex_data = submesh->vertexData;
								result.vertCount += vertex_data->vertexCount;
						}

						// Add the indices
						Ogre::IndexData* index_data = submesh->indexData;
						result.indexCount += index_data->indexCount;
				}
		
				result.vertices = new Ogre::Vector3[result.vertCount]; 
				result.normals = new Ogre::Vector3[result.vertCount];
				result.indices  = new unsigned long[result.indexCount];
				result.faceCount = result.indexCount / 3;
				result.faces = new Ogre::Vector3[result.faceCount];
				/////////////////////////
				added_shared = false;
		
				// Run through the submeshes again, adding the data into the arrays
				for(int i = 0;i < mesh->getNumSubMeshes();i++)
				{
						Ogre::SubMesh* submesh = mesh->getSubMesh(i);

						Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;
						if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
						{
								if(submesh->useSharedVertices)
								{
										added_shared = true;
										shared_offset = current_offset;
								}

								const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
								const Ogre::VertexElement* normalElement = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_NORMAL);
								Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
								unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
								Ogre::Real* pReal;
								Ogre::Real* pNormReal;
								for(size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
								{
										posElem->baseVertexPointerToElement(vertex, &pReal);
										normalElement->baseVertexPointerToElement(vertex, &pNormReal);
										Ogre::Vector3 pt;

										pt.x = (*pReal++);
										pt.y = (*pReal++);
										pt.z = (*pReal++);
								
										Ogre::Vector3 normal;
								
										normal.x = (*pNormReal++);
										normal.y = (*pNormReal++);
										normal.z = (*pNormReal++);
								
										pt = (orient * (pt * scale)) + position;

										result.vertices[current_offset + j].x = pt.x;
										result.vertices[current_offset + j].y = pt.y;
										result.vertices[current_offset + j].z = pt.z;
								
										result.normals[current_offset + j].x = normal.x;
										result.normals[current_offset + j].y = normal.y;
										result.normals[current_offset + j].z = normal.z;
								}
								vbuf->unlock();
								next_offset += vertex_data->vertexCount;
						}

						unsigned int faceCount = 0;
						switch(submesh->operationType)
						{
						case Ogre::RenderOperation::OT_TRIANGLE_LIST:
								// tri list
								faceCount = submesh->indexData->indexCount / 3;

								break;
						case Ogre::RenderOperation::OT_LINE_LIST:
								faceCount = submesh->indexData->indexCount / 2;

								break;
						case Ogre::RenderOperation::OT_TRIANGLE_FAN:
						case Ogre::RenderOperation::OT_TRIANGLE_STRIP:
								// triangle fan or triangle strip
								faceCount = submesh->indexData->indexCount - 2;

								break;
						default:
								OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, 
										"Unsupported render operation type", 
										__FUNCTION__);
						}

						// Write each face in turn
						unsigned int sz;
						unsigned int* pInt = 0;
						unsigned short* pShort = 0;
						Ogre::HardwareIndexBufferSharedPtr ibuf = submesh->indexData->indexBuffer;

						// bool use32BitIndexes
						bool use32BitIndexes = (!submesh->indexData->indexBuffer.isNull() && 
								submesh->indexData->indexBuffer->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

						if (use32BitIndexes)
						{
								pInt = static_cast<unsigned int*>(
										ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY)); 
						}
						else
						{
								pShort = static_cast<unsigned short*>(
										ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY)); 
						}
						for (sz = last_Offset; sz < last_Offset + faceCount; ++sz)
						{
								//st << "lo: " << last_Offset << " | sz: " << sz << " | fc: " << faceCount << std::endl;
								if (use32BitIndexes)
								{
										result.faces[sz].x = *pInt++;
										if(submesh->operationType == Ogre::RenderOperation::OT_LINE_LIST)
										{
												result.faces[sz].y = *pInt++;
										}
										/// Only need all 3 vertex indices if trilist or first face
										else if (submesh->operationType == Ogre::RenderOperation::OT_TRIANGLE_LIST || sz == 0)
										{
												result.faces[sz].y = *pInt++;
												result.faces[sz].z = *pInt++;
										}
								}
								else
								{
										result.faces[sz].x = *pShort++;
										if(submesh->operationType == Ogre::RenderOperation::OT_LINE_LIST)
										{
												result.faces[sz].y = *pShort++;
										}
										/// Only need all 3 vertex indices if trilist or first face
										else if (submesh->operationType == Ogre::RenderOperation::OT_TRIANGLE_LIST || sz == 0)
										{
												result.faces[sz].y = *pShort++;
												result.faces[sz].z = *pShort++;
										}
								}
						}
						last_Offset += faceCount;
				}
		//      st << result;
		//      st.close();
				return result;
		}

		inline hkpBvTreeShape* processOgreMesh(Ogre::MeshPtr mesh, Ogre::Node* transformNode = NULL)
		{
			OgreMeshData data;
			if (transformNode)
				data = getDataFromMesh(mesh, transformNode->getPosition(), transformNode->getOrientation(), transformNode->getScale());
			else
				data = getDataFromMesh(mesh, Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY, Ogre::Vector3(1, 1, 1));
			hkpSimpleMeshShape* meshStorage = new hkpSimpleMeshShape(hkConvexShapeDefaultRadius);
			meshStorage->m_vertices.setSize(data.vertCount);
			meshStorage->m_triangles.setSize(data.faceCount);

			for(unsigned int i = 0; i < data.vertCount; i++){
				hkVector4 vertex(data.vertices[i].x, data.vertices[i].y, data.vertices[i].z);
				meshStorage->m_vertices[i] = vertex;
			}
			for(unsigned int i = 0; i < data.faceCount; i++){
				meshStorage->m_triangles[i].m_a = data.faces[i].x;
				meshStorage->m_triangles[i].m_b = data.faces[i].y;
				meshStorage->m_triangles[i].m_c = data.faces[i].z;
			}

			hkpMoppCompilerInput mfr;
			mfr.setAbsoluteFitToleranceOfAxisAlignedTriangles( hkVector4( 0.1f, 0.1f, 0.1f ) );


			hkpMoppCode* pCode = hkpMoppUtility::buildCode( meshStorage, mfr );
			hkpBvTreeShape* shape = new hkpMoppBvTreeShape( meshStorage, pCode );
			pCode->removeReference();
			meshStorage->removeReference();

			return shape;
		}

		inline hkpBvTreeShape* processOgreMesh(Ogre::Entity* ent, Ogre::Node* transformNode = NULL)
		{
			return processOgreMesh(ent->getMesh(), transformNode);
		}

	}
}
#endif
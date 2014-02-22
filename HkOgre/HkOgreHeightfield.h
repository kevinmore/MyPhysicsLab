#ifndef HKOGRE_HEIGHTFIELD
#define HKOGRE_HEIGHTFIELD
#include <hkOgrePrerequisites.h>
#include <OgreMath.h>
#include <OgreImage.h>
#include <OGRE/Terrain/OgreTerrain.h>
#include <OGRE/Terrain/OgreTerrainGroup.h>

// Need some shapes
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Collide/Shape/HeightField/SampledHeightField/hkpSampledHeightFieldShape.h>
#include <Physics/Collide/Shape/HeightField/SampledHeightField/hkpSampledHeightFieldBaseCinfo.h>
#include <Common/Base/Types/Geometry/Aabb/hkAabb.h>

namespace HkOgre
{
	namespace Heightfield
	{
		static void getCInfoFromImage(Ogre::Image img, hkpSampledHeightFieldBaseCinfo& ci)
		{
			ci.m_xRes = 513;

			ci.m_zRes = 513;

			ci.m_maxHeight = 600;

			ci.m_minHeight = 0.0f;

			float xScale = 12000.0f / 513.0f;

			float zScale = 12000.0f / 513.0f;

			ci.m_scale =  hkVector4(xScale, 1.0f, zScale, 0.0f);
		}

		struct SampledHeightfieldConfig
		{
			SampledHeightfieldConfig(Ogre::Image img)
				: heightData(NULL)
				, info(hkpSampledHeightFieldBaseCinfo())
			{
				// Get info
				getCInfoFromImage(img, info);
				 
				// Get height data
				heightData = new hkReal[513*513];
				int size = 513; // needed for indexing in loop
				int index;

				for(int i = 0; i < 513; i++)
				{
					for(int j = 0; j < 513; j++)
					{
						float height = img.getColourAt(i, j, 0).r * 600;
						index = i * size + j;
						heightData[index] = height;
					}
				}
			}

			hkReal*	heightData;
			hkpSampledHeightFieldBaseCinfo& info;
		};

		class SampledHeightfield: public hkpSampledHeightFieldShape
		{
		public:
			SampledHeightfield(SampledHeightfieldConfig cfg)
				:	hkpSampledHeightFieldShape(cfg.info)
				,	mHeightData(cfg.heightData)
			{
				ofstream str;
				str.open("heightfield.dmp");
				for(int i = 0; i < 513; i++)
				{
					str << i << "	|	" << mHeightData[i] << std::endl;
				}
				str.close();
			}

			// Generate a rough terrain
			HK_FORCE_INLINE hkReal getHeightAtImpl( int x, int z ) const
			{
				// Lookup data and return a float
				// We scale the data artificially by 5 to make it look interesting
				return mHeightData[z * m_zRes  + x];;
			}

			// Assuming each heightfield quad is defined as four points { 00, 01, 11, 10 },
			// this should return true if the two triangles share the edge p00-p11.
			// Otherwise it should return false if the triangles share the edge p01-p10
			HK_FORCE_INLINE hkBool getTriangleFlipImpl() const
			{	
				return false;
			}

			virtual void collideSpheres( const CollideSpheresInput& input, SphereCollisionOutput* outputArray) const
			{
				hkSampledHeightFieldShape_collideSpheres(*this, input, outputArray);
			}

		private:
			hkReal*			mHeightData;
			Ogre::Image		mImg;
		};

		inline Ogre::AxisAlignedBox getTerrainGroupBounds(Ogre::TerrainGroup* group)
		{
			// Calculate terrain bounds
			// TODO extract calculateTerrainExtents() ?
			Ogre::TerrainGroup::TerrainIterator ti = group->getTerrainIterator();
			Ogre::Terrain* trn;
			// TODO we could calculate terrain count here
			size_t trnCount = 0;
			float* bmin = new float[3];
			float* bmax = new float[3];
			for (int i = 0; i < 3; i++)
				bmin[i] = bmax[i] = 0;

			while(ti.hasMoreElements())
			{
				trn = ti.getNext()->instance;
				Ogre::AxisAlignedBox bb = trn->getAABB();
				Ogre::Vector3 min = bb.getMinimum();
				if(min.x < bmin[0])
					bmin[0]= min.x;
				if(min.y < bmin[1])
					bmin[1]= min.y;
				if(min.z < bmin[2])
					bmin[2]= min.z;

				Ogre::Vector3 max = bb.getMaximum();
				if(max.x > bmax[0])
					bmax[0]= max.x;
				if(max.y > bmax[1])
					bmax[1]= max.y;
				if(max.z > bmax[2])
					bmax[2]= max.z;
				trnCount++;
			}
			return Ogre::AxisAlignedBox(bmin[0], bmin[1], bmin[2], bmax[0], bmax[1], bmax[3]);
		}

		inline hkpBvTreeShape* processOgreTerranGroup(Ogre::TerrainGroup* terrainGroup)
		{

			// TODO We are only creating a navmesh from terrain at the moment
			// TODO look at PW's loadhouses method for entities loading


			// TODO check for no terrain
			//    if (srcMeshes.empty())
			//        return;

			// PARTS OF THE FOLLOWING CODE WERE TAKEN AND MODIFIED FROM AN OGRE3D FORUM POST
			//const int numNodes = srcMeshes.size();
			// TODO how to retrieve number of pages in terrain group? Hardcoded at the moment.
			int pagesTotal = 1;
			const int totalMeshes = pagesTotal /*+ numNodes*/;

			size_t nverts = 0;
			size_t ntris = 0;
			size_t *meshVertexCount = new size_t[totalMeshes];
			size_t *meshIndexCount = new size_t[totalMeshes];
			Ogre::Vector3 **meshVertices = new Ogre::Vector3*[totalMeshes];
			unsigned long **meshIndices = new unsigned long*[totalMeshes];

			// Calculate terrain bounds
			// TODO extract calculateTerrainExtents() ?
			Ogre::TerrainGroup::TerrainIterator ti = terrainGroup->getTerrainIterator();
			Ogre::Terrain* trn;
			// TODO we could calculate terrain count here
			size_t trnCount = 0;

			//---------------------------------------------------------------------------------
			// TERRAIN DATA BUILDING
			ti = terrainGroup->getTerrainIterator();
			trnCount = 0;
			while(ti.hasMoreElements())
			{
				trn = ti.getNext()->instance;
				// TODO do we want to set a query mask here? or does default suffice?
				//         trn->setQueryFlags(GEOMETRY_QUERY_MASK);

				// get height data, world size, map size
				float *mapptr = trn->getHeightData();
				float WorldSize = trn->getWorldSize();
				int MapSize = trn->getSize();
				// calculate where we need to move/place our vertices
				float DeltaPos = (WorldSize / 2.0f);

				float DeltaX = 0;
				float DeltaZ = 0;
				// TODO this hardcoded behaviour has to go! Supports only up to 4 terrain pages
				switch(trnCount)
				{
				case 0:
					DeltaX = -6000;
					DeltaZ = 6000;
					break;
				case 1:
					DeltaX = -6000;
					DeltaZ = -6000;
					break;
				case 2:
					DeltaX = 6000;
					DeltaZ = 6000;
					break;
				case 3:
					DeltaX = 6000;
					DeltaZ = -6000;
					break;
				default:
					DeltaX = 0;
					DeltaZ = 0;
				}

				std::cout << "Terrain: " << trnCount << std::endl;

				float Scale = WorldSize / (float)(MapSize - 1);

				//////////////////////////////
				// THIS CODE WAS TAKEN FROM
				// AN OGRE FORUMS THREAD IN THE
				// NEW TERRAIN SCREENSHOTS THREAD
				// IN THE SHOWCASE FORUMS - I ONLY MODIFIED IT
				// TO BE ABLE TO WORK FOR RECAST AND IN THE CONTEXT OF
				// THIS DEMO APPLICATION

				// build vertices
				meshVertices[trnCount] = new Ogre::Vector3[(MapSize*MapSize)];

				int i = 0;
				int u = 0;
				int max = MapSize; // i think i needed this for something before but now it's obviously redundant
				int z = 0;
				for(int x = 0;; ++x)
				{
					// if we've reached the right edge, start over on the next line
					if(x == max)
					{
						x = 0;
						++z;
					}
					// if we reached the bottom/end, we're done
					if(z == max)
						break;

					// Calculate world coordinates for terrain tile vertex. Terrain vertices are defined in tile-local coordinates.
					// add the vertex to the buffer
					meshVertices[trnCount][u] = Ogre::Vector3((Scale * x) + DeltaX, mapptr[(MapSize * z) + x], (Scale * -z) + DeltaZ);

					i += 3;
					++u;
				}


				size_t size = ((MapSize*MapSize)-(MapSize*2)) * 6;
				meshIndices[trnCount] = new unsigned long[size];
				// i will point to the 'indices' index to insert at, x points to the vertex # to use
				i = 0;
				for(int x = 0;;++x)
				{
					// skip rightmost vertices
					if((x+1)%MapSize == 0)
					{
						++x;
					}

					// make a square of 2 triangles
					meshIndices[trnCount][i] = x;
					meshIndices[trnCount][i+1] = x + 1;
					meshIndices[trnCount][i+2] = x + MapSize;

					meshIndices[trnCount][i+3] = x + 1;
					meshIndices[trnCount][i+4] = x + 1 + MapSize;
					meshIndices[trnCount][i+5] = x + MapSize;

					// if we just did the final square, we're done
					if(x+1+MapSize == (MapSize*MapSize)-1)
						break;

					i += 6;
				}

				meshVertexCount[trnCount] = trn->getSize()*trn->getSize();
				meshIndexCount[trnCount] = size;

				nverts += meshVertexCount[trnCount];
				ntris += meshIndexCount[trnCount];

				if(trnCount < pagesTotal)
					++trnCount;
			}
			ntris = ntris / 3;


			hkpSimpleMeshShape* meshStorage = new hkpSimpleMeshShape(hkConvexShapeDefaultRadius);
			meshStorage->m_vertices.setSize(nverts);
			meshStorage->m_triangles.setSize(ntris);

			for(unsigned int i = 0; i < meshVertexCount[0]; i++){
				hkVector4 vertex(meshVertices[0][i].x, meshVertices[0][i].y, meshVertices[0][i].z);
				meshStorage->m_vertices[i] = vertex;
			}
			for(unsigned int i = 0; i < ntris; i++){
				meshStorage->m_triangles[i].m_a = meshIndices[0][i];
				meshStorage->m_triangles[i].m_b = meshIndices[0][i+1];
				meshStorage->m_triangles[i].m_c = meshIndices[0][i+2];
			}

			hkpMoppCompilerInput mfr;
			mfr.setAbsoluteFitToleranceOfAxisAlignedTriangles( hkVector4( 0.1f, 0.1f, 0.1f ) );


			hkpMoppCode* pCode = hkpMoppUtility::buildCode( meshStorage, mfr );
			hkpBvTreeShape* shape = new hkpMoppBvTreeShape( meshStorage, pCode );
			pCode->removeReference();
			meshStorage->removeReference();

			delete [] meshVertices;
			delete [] meshVertexCount;
			delete [] meshIndices;
			delete [] meshIndexCount;

			return shape;
		}
	}
}

#endif // HKOGRE_HEIGHTFIELD
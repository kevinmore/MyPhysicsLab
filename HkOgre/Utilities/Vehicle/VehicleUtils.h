/*/////////////////////////////////////////////////////////////////////////////////
/// Copyright (c) 2012 Ilija Boshkov <Mind Calamity>
//
/// The MIT License
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////*/

#ifndef HKOGRE_VEHICLE_UTILS
#define HKOGRE_VEHICLE_UTILS

#include <Common/Internal/ConvexHull/hkGeometryUtility.h>
#include <Physics/Utilities/VisualDebugger/Viewer/hkpShapeDisplayBuilder.h>
#include <Physics/Collide/Shape/Convex/ConvexVertices/hkpConvexVerticesShape.h>
#include <Physics/Vehicle/hkpVehicleInstance.h>
#include <Physics/Vehicle/Camera/hkp1dAngularFollowCam.h>
#include <Physics/Collide/Query/Collector/RayCollector/hkpClosestRayHitCollector.h>
#include <OgreCamera.h>
#include <OgreSceneNode.h>
#include <hkOgreMath.h>

#include <Utilities/Graphics/Raycast.h>

namespace HkOgre
{
	namespace Vehicle
	{
		static hkpConvexVerticesShape* createDemoChassis()
		{
// 			hkReal xSize = 1.75f;
// 			hkReal ySize = 0.25f;
// 			hkReal zSize = 1.1f;
// 
// 			hkReal xBumper = 1.9f;
// 			hkReal yBumper = 0.15f;
// 			hkReal zBumper = 1.0f;
// 
// 			hkReal xRoofFront = 0.4f;
// 			hkReal xRoofBack = -1.0f;
// 			hkReal yRoof = ySize + 0.45f;
// 			hkReal zRoof = 0.7f;
// 
// 			hkReal xDoorFront = xRoofFront;
// 			hkReal xDoorBack = xRoofBack;
// 			hkReal yDoor = ySize;
// 			hkReal zDoor = zSize + 0.1f;

			hkReal xSize = 1.1f;
			hkReal ySize = 0.25f;
			hkReal zSize = 1.75f;

			hkReal xBumper = 1.0f;
			hkReal yBumper = 0.15f;
			hkReal zBumper = 1.9f;

			hkReal xRoofFront = 0.7f;
			hkReal xRoofBack = -1.0f;
			hkReal yRoof = ySize + 0.45f;
			hkReal zRoof = 0.4f;

			hkReal xDoorFront = xRoofFront;
			hkReal xDoorBack = xRoofBack;
			hkReal yDoor = ySize;
			hkReal zDoor = zSize + 0.1f;

			int numVertices = 22;

			// 16 = 4 (size of "each float group", 3 for x,y,z, 1 for padding) * 4 (size of float).
			int stride = sizeof(hkReal) * 4;

			HK_ALIGN16(hkReal vertices[] )= { 
				xSize, ySize, -zSize, 0.0f,		// v0
				xSize, ySize, zSize, 0.0f,		// v1
				xSize, -ySize, -zSize, 0.0f,		// v2
				xSize, -ySize, zSize, 0.0f,	// v3
				-xSize, -ySize, -zSize, 0.0f,	// v4
				-xSize, -ySize, zSize, 0.0f,	// v5

				xBumper, yBumper, -zBumper, 0.0f,	// v6
				xBumper, yBumper, zBumper, 0.0f,	// v7
				-xBumper, yBumper, -zBumper, 0.0f,	// v8
				-xBumper, yBumper, zBumper, 0.0f,	// v9

				xRoofFront, yRoof, -zRoof, 0.0f,		// v10
				xRoofFront, yRoof, zRoof, 0.0f,	// v11
				xRoofBack, yRoof, -zRoof, 0.0f,		// v12
				xRoofBack, yRoof, zRoof, 0.0f,		// v13

				xDoorFront, yDoor, -zDoor, 0.0f,		// v14
				xDoorFront, yDoor, zDoor, 0.0f,	// v15
				xDoorFront, -yDoor, -zDoor, 0.0f,	// v16
				xDoorFront, -yDoor, zDoor, 0.0f,	// v17

				xDoorBack, yDoor, -zDoor, 0.0f,		// v18
				xDoorBack, yDoor, zDoor, 0.0f,		// v19
				xDoorBack, -yDoor, -zDoor, 0.0f,		// v20
				xDoorBack, -yDoor, zDoor, 0.0f,	// v21
			};

			//
			// SHAPE CONSTRUCTION.
			//

			hkStridedVertices		stridedVerts;
			stridedVerts.m_numVertices	=	numVertices;
			stridedVerts.m_striding		=	stride;
			stridedVerts.m_vertices		=	vertices;

			return new hkpConvexVerticesShape(stridedVerts);
		}

		static void createCamera( hkp1dAngularFollowCam& camera )
		{
			hkp1dAngularFollowCamCinfo cinfo;

			cinfo.m_yawSignCorrection = 1.0f; 
			cinfo.m_upDirWS.set(0.0f, 1.0f, 0.0f); 
			cinfo.m_rigidBodyForwardDir.set(0.0, 0.0f, 1.0f); 

			cinfo.m_set[0].m_velocity = 10.0f;
			cinfo.m_set[1].m_velocity = 50.0f;
			cinfo.m_set[0].m_speedInfluenceOnCameraDirection = 1.0f;
			cinfo.m_set[1].m_speedInfluenceOnCameraDirection = 1.0f;
			cinfo.m_set[0].m_angularRelaxation = 3.5f;
			cinfo.m_set[1].m_angularRelaxation = 4.5f;

			// The two camera positions ("slow" and "fast" rest positions) are both the same here,
			// -6 units behind the chassis, and 2 units above it. Again, this is dependent on 
			// m_chassisCoordinateSystem.
			cinfo.m_set[0].m_positionUS.set( 0, 4.0f, -10.0f); 
			cinfo.m_set[1].m_positionUS.set( 0.0f, 5.7f, -16.0f); 

			cinfo.m_set[0].m_lookAtUS.set ( 0.0f, 0.5f, 2.0f );
			cinfo.m_set[1].m_lookAtUS.set ( 0.0f, 0.5f, 2.0f );

			cinfo.m_set[0].m_fov = 60.0f;
			cinfo.m_set[1].m_fov = 60.0f;

			camera.reinitialize( cinfo );
		}
		
		static void updateCamera(const hkpRigidBody& vehicleChassis,
			hkReal timestep,
			hkp1dAngularFollowCam& camera,
			Ogre::Camera* gCamera)
		{
			//
			// Update the camera, based on where the vehicle is (a "follow cam").
			//
			{
				//
				// Vehicle specific camera settings.
				//
				hkp1dAngularFollowCam::CameraInput in;
				{
					hkpWorld* world = vehicleChassis.getWorld();
					world->markForRead();
					hkReal time = world->getCurrentTime();
					world->unmarkForRead();
					vehicleChassis.approxTransformAt( time, in.m_fromTrans );	

					in.m_linearVelocity = vehicleChassis.getLinearVelocity();
					in.m_angularVelocity = vehicleChassis.getAngularVelocity();
					in.m_deltaTime = timestep;
				}

				hkp1dAngularFollowCam::CameraOutput out;
				{
					camera.calculateCamera( in, out );
				}

				// 
				// General camera settings.
				//
				{
					gCamera->setPosition( Convert::hkVec3toOgre(out.m_positionWS) );
					gCamera->lookAt( Convert::hkVec3toOgre(out.m_lookAtWS) );
				}
			}
		}
		static void steer(hkpVehicleInstance& vehicle, Ogre::Vector3 vSteering, float timestep)
		{
			{
				//
				// Update controller "position" within range [-1, 1] for both X, Y directions.
				//
				hkpVehicleDriverInputAnalogStatus*	deviceStatus = (hkpVehicleDriverInputAnalogStatus*)vehicle.m_deviceStatus;

				// Now -1 <= m_inputXPosition <= 1 and
				// -1 <= m_inputYPosition <= 1
				deviceStatus->m_positionY = vSteering.z;
				deviceStatus->m_positionX = vSteering.x;

				// AutoReverse
				deviceStatus->m_reverseButtonPressed = vSteering.z == -1;

				// Handbrake.
				//hkprintf( "%f %f\n", *inputXPosition, *inputYPosition);
				deviceStatus->m_handbrakeButtonPressed = vSteering.y == 1;
			}
		}

		static void syncWheels(hkpVehicleInstance& veh, std::vector<Ogre::SceneNode*> wheels)
		{
			// Sync wheels. This is necessary only because they do not "exist" as bodies in simulation,
			// and so are not automatically updated by the current display. We must explicity tell the
			// display that some other "display obects" (objects which are drawn but are not physical)
			// have moved.
			//
			for (size_t i = 0; i < wheels.size(); i++)
			{
				hkVector4    pos;
				hkQuaternion rot;

				//
				// XXX Check if this is the same value as the m_hardPointWS in wheelsInfo
				//
				//
				veh.calcCurrentPositionAndRotation( veh.getChassis(), 
					veh.m_suspension,					
					i,
					pos, rot );

				wheels[i]->setPosition(HkOgre::Convert::hkVec3toOgre(pos));
				wheels[i]->setOrientation(HkOgre::Convert::hkQuatToOgre(rot));
				if (i % 2 == 0)
					wheels[i]->rotate(Ogre::Quaternion(0, 0, 1, 0)); // Rotate the 2 wheels on the right 180 degrees.
			}
		}
	}
}


#endif
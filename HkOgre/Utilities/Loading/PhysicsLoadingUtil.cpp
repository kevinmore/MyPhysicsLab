/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Demos/demos.h>

#include <Demos/DemoCommon/Utilities/Loading/PhysicsLoadingUtil.h>
#include <Common/SceneData/Scene/hkxScene.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include <Common/Serialize/Util/hkSerializeUtil.h>
#include <Demos/DemoCommon/Utilities/Asset/hkAssetManagementUtil.h>
#include <Graphics/Bridge/SceneData/hkgSceneDataConverter.h>
#include <Graphics/Common/DisplayObject/hkgDisplayObject.h>
#include <Graphics/Common/DisplayWorld/hkgDisplayWorld.h>

#include <Physics/Dynamics/World/hkpWorld.h>
#include <Physics/Utilities/Serialize/hkpPhysicsData.h>

hkResult HK_CALL PhysicsLoadingUtil::loadScene( const char* filename, hkDemoEnvironment& env, hkpWorld& world, InputOptions* inputOpts, Output& output )
{
	hkResult res = LoadingUtil::loadScene( filename, env, inputOpts, output );
	if ( res == HK_FAILURE )
	{
		return HK_FAILURE;
	}

	// Grab the physics and add it if present
	hkpPhysicsData* physics = reinterpret_cast<hkpPhysicsData*>( output.m_rootLevelContainer->findObjectByType( hkpPhysicsDataClass.getName() ));

	if (!physics)
	{
		HK_WARN( 0x54e32123, "Unable to load hkpPhysicsData from file " << filename );
		return HK_FAILURE;
	}
	output.m_physicsData = physics;

	const hkArray<hkpPhysicsSystem*>& psys = physics->getPhysicsSystems();

	// Load physics systems
	for (int i=0; i < psys.getSize(); i++)
	{
		hkpPhysicsSystem* system = psys[i];

		// add the lot to the world
		world.addPhysicsSystem(system);

		for (int rb=0; rb < system->getRigidBodies().getSize(); rb++)
		{
			hkpRigidBody* body = system->getRigidBodies()[rb];

			if( inputOpts && inputOpts->m_transformScene )
			{
				hkMatrix3 rot = inputOpts->m_sceneTransformOptions.m_transformMatrix ;
				hkTransform transform; transform.setIdentity();
				transform.getRotation().setCols( rot.getColumn(0), rot.getColumn(1), rot.getColumn(2) );

				hkTransform tNew;
				tNew.setMul(transform, body->getTransform());
				body->setTransform( tNew );
			}
		}
	}

	return HK_SUCCESS;
}

/*
 * Havok SDK - NO SOURCE PC DOWNLOAD, BUILD(#20120405)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2012
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available at www.havok.com/tryhavok.
 * 
 */

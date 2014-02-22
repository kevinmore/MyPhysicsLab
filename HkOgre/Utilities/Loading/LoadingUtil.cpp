/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Demos/demos.h>

#include <Demos/DemoCommon/Utilities/Loading/LoadingUtil.h>
#include <Common/SceneData/Scene/hkxScene.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include <Common/Serialize/Util/hkSerializeUtil.h>
#include <Demos/DemoCommon/Utilities/Asset/hkAssetManagementUtil.h>
#include <Graphics/Bridge/SceneData/hkgSceneDataConverter.h>
#include <Graphics/Common/DisplayObject/hkgDisplayObject.h>
#include <Graphics/Common/DisplayWorld/hkgDisplayWorld.h>

hkResult HK_CALL LoadingUtil::loadScene( const char* filename, Output& output )
{
	hkStringBuf assetFile(filename);
	hkAssetManagementUtil::getFilePath(assetFile);

	hkObjectResource* res = hkSerializeUtil::loadOnHeap( assetFile );
	if ( !res )
	{
		HK_WARN( 0x54e32123, "Unable to load from file " << assetFile );
		return HK_FAILURE;
	}

	output.m_resource = res;
	res->removeReference();

	// Get the top level object in the file
	hkRootLevelContainer* container = res->getContents<hkRootLevelContainer>();
	if ( !container )
	{
		HK_WARN( 0xa6451543, "Unable to load root level hkRootLevelContainer from file " << assetFile );
		return HK_FAILURE;
	}
	output.m_rootLevelContainer = container;

	hkxScene* scene = reinterpret_cast<hkxScene*>( container->findObjectByType( hkxSceneClass.getName() ));
	if ( !scene )
	{
		HK_WARN( 0xa6451543, "Unable to load hkxScene from file " << assetFile );
		return HK_FAILURE;
	}
	output.m_scene = scene;

	return HK_SUCCESS;
}

hkResult HK_CALL LoadingUtil::loadScene( const char* filename, hkDemoEnvironment& env, InputOptions* inputOpts, Output& output )
{
	hkResult result = loadScene( filename, output );
	if (result == HK_FAILURE)
	{
		return HK_FAILURE;
	}

	hkxScene* scene = output.m_scene;

	if ( !scene )
	{
		HK_WARN( 0xa6451543, "Unable to load hkxScene from file " << filename );
		return HK_FAILURE;
	}
	output.m_scene = scene;

	if( inputOpts && inputOpts->m_transformScene )
	{
		hkxSceneUtils::transformScene(*scene, inputOpts->m_sceneTransformOptions );
	}

	if (scene)
	{
		env.m_sceneConverter->setShaderLibraryEnabled(true);
		bool addObjectsToWorld = inputOpts ? (bool) inputOpts->m_addDisplayObjects : true;
		env.m_sceneConverter->convert( scene, hkgAssetConverter::CONVERT_ALL, addObjectsToWorld );
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

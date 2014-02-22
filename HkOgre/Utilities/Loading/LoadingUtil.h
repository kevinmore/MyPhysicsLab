/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_LOADING_UTIL_H
#define HK_LOADING_UTIL_H

#include <Common/Base/hkBase.h>
#include <Common/SceneData/Scene/hkxSceneUtils.h>
#include <Common/Serialize/Resource/hkObjectResource.h>

class hkRootLevelContainer;
class hkxScene;

/// Utility function for loading scenes
class LoadingUtil
{
	public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,LoadingUtil);
		struct InputOptions
		{
			InputOptions()
			{
				m_transformScene = false;
				m_addDisplayObjects = true;
			}

			hkBool									m_transformScene;
			hkxSceneUtils::SceneTransformOptions	m_sceneTransformOptions;
			
				// Display
			hkBool									m_addDisplayObjects;
		};

		struct Output
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,LoadingUtil::Output);
			Output()
				: m_rootLevelContainer(HK_NULL)
				, m_scene(HK_NULL) {}

			hkRefPtr< hkObjectResource >	m_resource;
			hkRootLevelContainer*			m_rootLevelContainer;
			hkxScene*						m_scene;
		};

			/// Loads a scene from a file using serialize util
			/// Does not require a demo environment; used to retrieve the hkxScene
		static hkResult HK_CALL loadScene( const char* filename, Output& output );

			/// Loads a scene from a file using serialize util
			/// If no input options are specified (by passing HK_NULL) the default input options as per the constructor are implied
		static hkResult HK_CALL loadScene( const char* filename, hkDemoEnvironment& env, InputOptions* inputOpts, Output& output );
};

#endif // HK_LOADING_UTIL_H

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

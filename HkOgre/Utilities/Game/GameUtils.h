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

#ifndef DEMO_UTILS_H
#define DEMO_UTILS_H

#include <Ogre.h>
#include <hkOgreRenderable.h>

class GameUtils
{
	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_UTILITIES, GameUtils );
		enum RagdollPartType
		{
			RPT_BOX,
			RPT_CONVEX,
			RPT_SPHERE,
			RPT_CAPSULE,
			RPT_COUNT
		};
		enum RagdollWantHandsAndFeet
		{
			NO_HANDS_AND_FEET,
			WANT_HANDS_AND_FEET
		};
		enum RagdollInterBoneCollisions
		{
			DISABLE_ALL_BONE_COLLISIONS,
			DISABLE_ONLY_ADJOINING_BONE_COLLISIONS
		};


		struct RagdollCinfo
		{
			RagdollCinfo();

			hkReal m_height;
			hkVector4 m_position;
			hkQuaternion m_rotation; 
			int m_systemNumber; 
			int m_numVertebrae;
			bool m_wantOptimizeInertias;
			RagdollPartType m_boneShapeType;
			RagdollWantHandsAndFeet m_wantHandsAndFeet;
			RagdollInterBoneCollisions m_ragdollInterBoneCollisions;

		};

		/// This function creates a collection of constrained rigid bodies 
		/// representing a humanoid biped. From the specified height, limb 
		/// lengths are calculated using standard proportions measured in heads.
		/// The specified biped height equates to 8 heads.
		static hkpPhysicsSystem*	HK_CALL createRagdoll( hkReal height, const hkVector4& position, const hkQuaternion& rotation, int systemNumber = 1, RagdollPartType type = RPT_CAPSULE, Ogre::SceneManager* mgr = NULL, hkpWorld* wrld = NULL);
		static hkpPhysicsSystem*	HK_CALL createRagdoll( RagdollCinfo& info, Ogre::SceneManager* mgr, hkpWorld* world);
		static hkpRigidBody*		HK_CALL createCapsuleFromBox(const hkVector4& size, const hkReal mass, const hkVector4& position);
};

#endif
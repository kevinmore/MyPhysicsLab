#ifndef HKOGRE_MATH
#define HKOGRE_MATH
#include <hkOgrePrerequisites.h>
#include <OgreMath.h>

namespace HkOgre
{
	namespace Convert
	{
		static hkVector4 ogreVec3ToHk(Ogre::Vector3 v)
		{
			return hkVector4(v.x, v.y, v.z);
		}

		static hkVector4 ogreVec4ToHk(Ogre::Vector4 v)
		{
			return hkVector4(v.x, v.y, v.z, v.w);
		}

		static hkQuaternion ogreQuatToHk(Ogre::Quaternion q)
		{
			return hkQuaternion(q.x, q.y, q.z, q.w);
		}

		static Ogre::Vector3 hkVec3toOgre(const hkVector4& v)
		{
			return Ogre::Vector3(v(0), v(1), v(2));
		}

		static Ogre::Vector4 hkVec4ToOgre(const hkVector4& v)
		{
			return Ogre::Vector4(v(0), v(1), v(2), v(3));
		}

		static Ogre::Quaternion hkQuatToOgre(const hkQuaternion& q)
		{
			return Ogre::Quaternion(q.m_vec(3), q.m_vec(0), q.m_vec(1), q.m_vec(2));
		}
	}
}

#endif // HKOGRE_MATH
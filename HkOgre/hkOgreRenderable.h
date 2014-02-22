#ifndef HKOGRE_RENDERABLE
#define HKOGRE_RENDERABLE

#include <hkOgrePrerequisites.h>
#include <Physics/Dynamics/World/Listener/hkpWorldPostSimulationListener.h>
#include <Ogre.h>
#include <hkOgreMath.h>

namespace HkOgre
{
	class Renderable : public hkpWorldPostSimulationListener
	{
	public:
		Renderable(Ogre::Node* node, hkpRigidBody* body, hkpWorld* world)
		{
			mNode	= node;
			mBody	= body;
			mWorld	= world; 
			
			mWorld->markForWrite();
				mWorld->addWorldPostSimulationListener(this);
			mWorld->unmarkForWrite();
		}

		~Renderable()
		{
			mWorld->markForWrite();
				mWorld->removeWorldPostSimulationListener(this);
				mWorld->removeEntity(mBody);
			mWorld->unmarkForWrite();
		}

		/// Called at the end of the hkpWorld::simulate call.
		virtual void postSimulationCallback( hkpWorld* world )
		{
			mNode->setPosition(Convert::hkVec3toOgre(mBody->getPosition()));
			mNode->setOrientation(Convert::hkQuatToOgre(mBody->getRotation()));
		}

		/// Called when an inactive/fixed entity is moved in the hkpWorld.
		virtual void inactiveEntityMovedCallback( hkpEntity* entity )
		{

		}
	
	private:
		hkpRigidBody*		mBody;
		hkpWorld*			mWorld;
		Ogre::Node*			mNode;
	};
}

#endif // HKOGRE_RENDERABLE
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

#pragma region Includes
#include <Common/Base/hkBase.h>
#include <Common/Base/Ext/hkBaseExt.h>

#include <Common/Base/Reflection/hkClass.h>
#include <Common/Base/Types/Geometry/hkGeometry.h>
#include <Physics/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>

#include <Physics/Dynamics/Constraint/Bilateral/Ragdoll/hkpRagdollConstraintData.h>
#include <Physics/Dynamics/Constraint/Bilateral/LimitedHinge/hkpLimitedHingeConstraintData.h>
#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Dynamics/World/hkpPhysicsSystem.h>
#include <Physics/Dynamics/Entity/hkpEntityListener.h>

#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>
#include <Physics/Utilities/Collide/Filter/GroupFilter/hkpGroupFilterUtil.h>

#include <Utilities/Game/GameUtils.h>

#pragma endregion

static hkpRigidBody*	HK_CALL createRagdollPart( const hkVector4& extents, hkReal mass, const hkVector4& position, int type = GameUtils::RPT_BOX, const char* filename = HK_NULL )
{
	hkpRigidBody* rb = HK_NULL;

	rb = GameUtils::createCapsuleFromBox( extents, mass, position );
	if (type != GameUtils::RPT_CAPSULE)
		HK_ASSERT2(0x58b3164b,  type < GameUtils::RPT_COUNT, "Wrong part type!" );

	rb->setLinearDamping( 0.25f );
	rb->setAngularDamping( 0.25f );
	rb->setQualityType( HK_COLLIDABLE_QUALITY_MOVING );

	return rb;
}

static hkpRagdollConstraintData* HK_CALL createRagdollContraint(
	hkpRigidBody* rbReference,
	hkpRigidBody* rbAttached,
	hkReal coneMin,
	hkReal coneMax,
	hkReal planeMin,
	hkReal planeMax,
	hkReal twistMin,
	hkReal twistMax,
	const hkVector4 &pivot,
	const hkVector4 &twistAxis,
	const hkVector4 &planeAxis )
{
	hkVector4 pivotReference;
	hkVector4 twistReference;
	hkVector4 planeReference;
	pivotReference.setTransformedInversePos( rbReference->getTransform(), pivot );
	twistReference.setRotatedInverseDir( rbReference->getRotation(), twistAxis ); twistReference.normalize3();
	planeReference.setRotatedInverseDir( rbReference->getRotation(), planeAxis ); planeReference.normalize3();

	hkVector4 pivotAttached;
	hkVector4 twistAttached;
	hkVector4 planeAttached;
	pivotAttached.setTransformedInversePos( rbAttached->getTransform(), pivot );
	twistAttached.setRotatedInverseDir( rbAttached->getRotation(), twistAxis );	twistAttached.normalize3();
	planeAttached.setRotatedInverseDir( rbAttached->getRotation(), planeAxis ); planeAttached.normalize3();

	// check for unsymmetrical cone axes
	hkReal centerCone	= (coneMax + coneMin) * 0.5f;
	hkReal diffCone		= (coneMax - coneMin) * 0.5f;

	if( hkMath::fabs( centerCone ) > HK_REAL_EPSILON )
	{
		twistReference._setRotatedDir( hkQuaternion( planeReference, -centerCone ), twistReference );
	}

	hkpRagdollConstraintData* bob = new hkpRagdollConstraintData( );
	bob->setInBodySpace(	pivotReference,	pivotAttached,
		planeReference,	planeAttached,
		twistReference,	twistAttached );

	bob->setConeAngularLimit( diffCone );
	bob->setPlaneMinAngularLimit( planeMin );
	bob->setPlaneMaxAngularLimit( planeMax );
	bob->setTwistMinAngularLimit( twistMin );
	bob->setTwistMaxAngularLimit( twistMax );
	bob->setMaxFrictionTorque( 0.7f );

	return bob;
}


hkpPhysicsSystem* HK_CALL GameUtils::createRagdoll( hkReal height, const hkVector4& ragdollPosition, const hkQuaternion& ragdollRotation, int systemNumber, RagdollPartType type, Ogre::SceneManager* mgr, hkpWorld* wrld)
{

	RagdollCinfo info;
	info.m_height = height;
	info.m_position = ragdollPosition;
	info.m_rotation = ragdollRotation;
	info.m_systemNumber = systemNumber;
	info.m_boneShapeType = type;
	return createRagdoll( info, mgr, wrld );
}

GameUtils::RagdollCinfo::RagdollCinfo()
{
	m_height = 2;
	m_position.setZero4();
	m_rotation.setIdentity();
	m_systemNumber = 1;
	m_numVertebrae = 1;
	m_wantOptimizeInertias = true;
	m_boneShapeType = RPT_CAPSULE;
	m_wantHandsAndFeet = NO_HANDS_AND_FEET;
	m_ragdollInterBoneCollisions = DISABLE_ALL_BONE_COLLISIONS;
}

hkpPhysicsSystem* HK_CALL GameUtils::createRagdoll( RagdollCinfo& info, Ogre::SceneManager* sceneMgr, hkpWorld* wrld)
{
	const hkReal degreesToRadians  = HK_REAL_PI / 180.0f;

	hkpPhysicsSystem* ragdoll = new hkpPhysicsSystem;
	hkArray<hkTransform>	offsets; // not in the physics system (extra data?)

	struct _parts {
		hkArray<hkpRigidBody*>	spine;
		hkArray<hkpRigidBody*>	leftLeg;
		hkArray<hkpRigidBody*>	rightLeg;
		hkArray<hkpRigidBody*>	leftArm;
		hkArray<hkpRigidBody*>	rightArm;
	}	parts;

	hkBool	wantFeet	= false;
	hkBool	wantHands	= false;

	if (info.m_wantHandsAndFeet == WANT_HANDS_AND_FEET)
	{
		wantFeet = true;
		wantHands = true;
	}


	int	numVertebra	= info.m_numVertebrae;

	const hkReal head = info.m_height / 8.0f;
	const hkReal hand = head * 2.0f / 3.0f;

	// create rigid bodies
	{
		hkpRigidBody*	rb;
		hkVector4		extents;
		hkVector4		position;
		hkRotation		rotation;
		hkTransform		offset;
		hkReal			mass;

		const hkReal	scale = info.m_height / 2.0f;
		{
			extents.set( 0.65f * head * 1.75f, 0.7f * head, head + (numVertebra ? 0 : 2.25f * head), info.m_height );
			position.set( 0, 0, 4.25f * head );
			rotation.setRows( hkVector4( 0, 0, 1 ), hkVector4( 0, -1, 0 ), hkVector4( 1, 0, 0 ) );
			offset.getRotation() = rotation;
			offset.getTranslation().set( (numVertebra ? 0 : 1.125f * head), 0, 0 );
			mass = 30.0f * scale;

			hkVector4 p; p.setRotatedDir( offset.getRotation(), offset.getTranslation() ); p.add4( position );
			rb = createRagdollPart( extents, mass, p, info.m_boneShapeType, HK_NULL );

			ragdoll->addRigidBody( rb );
			rb->removeReference(); // we don't want the ref anymore, leave with the system.

			offsets.pushBack( offset );

			parts.spine.pushBack( rb );
		}

		for( int i = 0; i < numVertebra; i++ )
		{
			extents.set( 0.65f * head * 2.0f, 0.6f * head, 2.25f * head / numVertebra, info.m_height );
			position.set( 0, 0, (4.75f * head) + (i * (2.0f * head / numVertebra)) );
			rotation.setRows( hkVector4( 0, 0, 1 ), hkVector4( 0, -1, 0 ), hkVector4( 1, 0, 0 ) );
			offset.getTranslation().set( 0.5f * extents(2), 0, 0 );
			offset.getRotation() = rotation;
			mass = (20.0f * scale) / numVertebra;

			hkVector4 p; p.setRotatedDir( offset.getRotation(), offset.getTranslation() ); p.add4( position );
			rb = createRagdollPart( extents, mass, p, info.m_boneShapeType, HK_NULL );

			ragdoll->addRigidBody( rb );
			HkOgre::hkRenderable* bone = new HkOgre::hkRenderable(sceneMgr->getEntity("ragdoll")->getSkeleton()->getBone("Stomach"), rb);
			wrld->addWorldPostSimulationListener(bone);

			rb->removeReference(); // we don't want the ref anymore, leave with the system.

			offsets.pushBack( offset );

			parts.spine.pushBack( rb );


		}

		{
			extents.set( hand, head, head, info.m_height );
			position.set( 0, 0, 7.0f * head );
			rotation.setRows( hkVector4( 0, 0, 1 ), hkVector4( 0, -1, 0 ), hkVector4( 1, 0, 0 ) );
			offset.getTranslation().set( 0.5f * extents(2), 0.1f * extents(1), 0 );
			offset.getRotation() = rotation;
			mass = 5.0f * scale;

			hkVector4 p; p.setRotatedDir( offset.getRotation(), offset.getTranslation() ); p.add4( position );
			rb = createRagdollPart( extents, mass, p, info.m_boneShapeType, HK_NULL );

			ragdoll->addRigidBody( rb );

			HkOgre::hkRenderable* bone = new HkOgre::hkRenderable(sceneMgr->getEntity("ragdoll")->getSkeleton()->getBone("Chest"), rb);
			wrld->addWorldPostSimulationListener(bone);

			rb->removeReference(); // we don't want the ref anymore, leave with the system.

			offsets.pushBack( offset );

			parts.spine.pushBack( rb );
		}
	}

#pragma region Constraints
	// create ragdoll->m_constraints
	{
		// Neck
		{
			hkpRagdollConstraintData* rdc = HK_NULL;

			hkpRigidBody* rbAttached		= parts.spine[parts.spine.getSize() - 1];
			hkpRigidBody* rbReference	= parts.spine[parts.spine.getSize() - 2];

			hkVector4 twistAxis( 0.0f, 0.0f, 1.0f );
			hkVector4 planeAxis( 1.0f, 0.0f, 0.0f );
			hkVector4 pivot;

			hkTransform	offset = offsets[ragdoll->getRigidBodies().indexOf( rbAttached )];
			pivot.setRotatedDir( offset.getRotation(), offset.getTranslation() );
			pivot.setNeg4( pivot );
			pivot.add4( rbAttached->getPosition() );

			rdc = createRagdollContraint(	rbReference,
				rbAttached,
				// cone
				-45.0f	* degreesToRadians,
				10.0f	* degreesToRadians,
				// plane
				-15.0f	* degreesToRadians,
				15.0f	* degreesToRadians,
				// twist
				-5.0f	* degreesToRadians,
				5.0f	* degreesToRadians,
				pivot,
				twistAxis,
				planeAxis );

			hkpConstraintInstance* ci = new hkpConstraintInstance( rbReference, rbAttached, rdc);
			ragdoll->addConstraint( ci );
			ci->removeReference();

			rdc->removeReference();
		}

		// Vertebra
		{
			hkpRagdollConstraintData* rdc = HK_NULL;

			for( int i = 1; i < (parts.spine.getSize() - 1); i++ )
			{

				hkpRigidBody* rbAttached		= parts.spine[i];
				hkpRigidBody* rbReference	= parts.spine[i - 1];
				//			hkTransform	offset = offsets[ragdoll->getRigidBodies().indexOf( rbAttached )];
				//			pivot.setRotatedDir( offset.getRotation(), offset.getTranslation() );
				//			pivot.setNeg4( pivot );
				//			pivot.add4( rbAttached->getPosition() );
				if ( !rdc )
				{
					hkVector4 twistAxis( 0.0f, 0.0f, 1.0f );
					hkVector4 planeAxis( 0.0f, -1.0f, 0.0f );
					hkVector4 pivot;

					hkTransform	offset = offsets[ragdoll->getRigidBodies().indexOf( rbAttached )];
					pivot.setRotatedDir( offset.getRotation(), offset.getTranslation() );
					pivot.setNeg4( pivot );
					pivot.add4( rbAttached->getPosition() );

					rdc = createRagdollContraint(	rbReference, rbAttached,
						// cone
						(-25.0f / numVertebra) * degreesToRadians,
						( 25.0f / numVertebra) * degreesToRadians,
						// plane
						(-20.0f / numVertebra) * degreesToRadians,
						( 10.0f / numVertebra) * degreesToRadians,
						// twist
						( -5.0f / numVertebra) * degreesToRadians,
						(  5.0f / numVertebra) * degreesToRadians,
						pivot,		twistAxis,		planeAxis );
				}
				hkpConstraintInstance* ci = new hkpConstraintInstance( rbReference, rbAttached, rdc);
				ragdoll->addConstraint( ci );
				ci->removeReference();
			}
			rdc->removeReference();
		}
	}

	// re-position ragdoll
	// set collision information
	{
		for( int i = ragdoll->getRigidBodies().getSize()-1; i >= 0; i-- )
		{
			hkpRigidBody* rbRoot	= ragdoll->getRigidBodies()[0];
			hkpRigidBody* rb		= ragdoll->getRigidBodies()[i];
			if ( info.m_ragdollInterBoneCollisions == DISABLE_ALL_BONE_COLLISIONS )
			{
				rb->setCollisionFilterInfo( hkpGroupFilter::calcFilterInfo( 0, info.m_systemNumber ) );
			}

			hkVector4 position;
			position.setSub4( rb->getPosition(), rbRoot->getPosition() );
			position._setRotatedDir( info.m_rotation, position );
			position.add4( info.m_position );

			rb->setPosition( position );
			rb->setRotation( info.m_rotation );
		}

		if ( info.m_ragdollInterBoneCollisions == DISABLE_ONLY_ADJOINING_BONE_COLLISIONS )
		{
			hkpGroupFilterUtil::disableCollisionsBetweenConstraintBodies( ragdoll->getConstraints().begin(), ragdoll->getConstraints().getSize(), info.m_systemNumber );
		}
		if ( info.m_wantOptimizeInertias)
		{
			hkpInertiaTensorComputer::optimizeInertiasOfConstraintTree( ragdoll->getConstraints().begin(), ragdoll->getConstraints().getSize(), ragdoll->getRigidBodies()[0] );
		}

	}
#pragma endregion
	// ragdoll system will keep the references.

	return ragdoll;
}

hkpRigidBody* HK_CALL GameUtils::createCapsuleFromBox(const hkVector4& size, const hkReal mass, const hkVector4& position)
{
	int maxIndex = size.getMajorAxis3();
	int minIndex = (maxIndex + 1) % 3;
	int midIndex = (minIndex + 1) % 3;
	if( size(minIndex) > size(midIndex) )
	{
		// swap
		minIndex = minIndex ^ midIndex;
		midIndex = minIndex ^ midIndex;
		minIndex = minIndex ^ midIndex;
	}

	hkReal		radius = 0.5f * size(midIndex);
	hkReal      length = hkMath::max2( radius * 0.01f ,( 0.5f * size(maxIndex)) - radius);

	hkVector4	start;	start.setZero4();	start(maxIndex) = length;
	hkVector4	end;	end.setNeg4( start );

	hkpRigidBodyCinfo bob;

	bob.m_shape			= new hkpCapsuleShape( start, end, radius );
	bob.m_mass			= mass;

	if( 0.0f == bob.m_mass )
	{
		bob.m_motionType	= hkpMotion::MOTION_FIXED;
	}
	else
	{
		bob.m_motionType	= hkpMotion::MOTION_BOX_INERTIA;

		hkpMassProperties massProperties;
		hkpInertiaTensorComputer::setShapeVolumeMassProperties( bob.m_shape, bob.m_mass, bob );
	}

	bob.m_rotation.setIdentity();
	bob.m_position = position;

	hkpRigidBody* rb = new hkpRigidBody( bob );

	bob.m_shape->removeReference();

	return rb;
}
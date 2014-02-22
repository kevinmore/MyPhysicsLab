/************************************************************************/
/* This is class take care of saving and loading files                  */
/************************************************************************/

#ifndef __FileManager_h_
#define __FileManager_h_


#pragma once
// Havok and HkOgre includes
#include <hkOgreCooker.h>
#include <hkOgreRenderable.h>

class FileManager
{
public:
	FileManager(void);
	~FileManager(void);
	
	//function to save the world data
	void saveWorld( const hkpWorld* world, bool binary );

	//function to load the world data
	hkpWorld* loadWorld(  hkpPhysicsData** physicsData, hkResource** memData );
};

#endif

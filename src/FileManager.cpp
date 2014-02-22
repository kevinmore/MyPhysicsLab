#include "FileManager.h"

static const char SCRATCH_FILE[] = "Save/worldsnapshot.hkx";
FileManager::FileManager(void)
{
	
}


FileManager::~FileManager(void)
{
}

void FileManager::saveWorld( const hkpWorld* world,  bool binary )
{
	hkOstream outfile( SCRATCH_FILE );
	HK_ON_DEBUG( hkBool res = ) hkpHavokSnapshot::save(world, outfile.getStreamWriter(), binary);
	HK_ASSERT( 0x215d080d, res );
}

hkpWorld* FileManager::loadWorld( hkpPhysicsData** physicsData, hkResource** memData )
{
	hkIstream infile( SCRATCH_FILE );
	HK_ASSERT( 0x215d080c, infile.isOk() );
	*physicsData = hkpHavokSnapshot::load(infile.getStreamReader(), memData);
	if ( ! *physicsData )
	{
		HK_WARN_ALWAYS(0x3e65b887, "Failed to load asset " << SCRATCH_FILE);
		return HK_NULL;
	}

	// Ensure non-multi threaded simulation for non-multi threaded platforms
#if !defined(HK_PLATFORM_MULTI_THREAD) || (HK_CONFIG_THREAD == HK_CONFIG_SINGLE_THREADED)
	(*physicsData)->getWorldCinfo()->m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_CONTINUOUS;
#endif

	hkpWorld* w = (*physicsData)->createWorld();


	return w;
}

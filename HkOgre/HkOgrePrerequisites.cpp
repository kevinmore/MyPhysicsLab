#include <hkOgrePrerequisites.h>
// Keycode
#include <Common/Base/keycode.cxx>

#if !defined HK_FEATURE_PRODUCT_PHYSICS
#error Physics is needed to build this demo. It is included in the common package for reference only.
#endif

// This excludes libraries that are not going to be linked
// from the project configuration, even if the keycodes are
// present

#undef HK_FEATURE_PRODUCT_AI
#undef HK_FEATURE_PRODUCT_ANIMATION
#undef HK_FEATURE_PRODUCT_CLOTH
#undef HK_FEATURE_PRODUCT_DESTRUCTION
#undef HK_FEATURE_PRODUCT_BEHAVIOR
#define HK_FEATURE_REFLECTION_PHYSICS
#define HK_CLASSES_FILE <Common/Serialize/Classlist/hkClasses.h>
#define HK_EXCLUDE_FEATURE_MemoryTracker
#define HK_EXCLUDE_FEATURE_SerializeDeprecatedPre700
#define HK_EXCLUDE_FEATURE_RegisterVersionPatches 
#define HK_EXCLUDE_LIBRARY_hkGeometryUtilities
#include <Common/Base/Config/hkProductFeatures.cxx>

#include <stdio.h>

#ifdef HK_PLATFORM_CTR
#define PRINTF nndbgDetailPrintf
#elif defined(HK_PLATFORM_ANDROID)
#include <android/log.h>
#define PRINTF(...) __android_log_print(ANDROID_LOG_INFO, "Havok", __VA_ARGS__)
#else
#define PRINTF printf
#endif
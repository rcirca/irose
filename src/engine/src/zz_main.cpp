/** 
 * @file zz_material_terrain.h
 * @brief terrain material class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    06-feb-2003
 *
 * $Header: /engine/src/zz_main.cpp 2     04-06-29 10:09a Zho $
 */

#include <windows.h>

HINSTANCE g_hinstDLL = 0;

#ifdef ZZ_DLL_EXPORT
BOOL WINAPI DllMain (HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved)
{
	g_hinstDLL = hinstDLL;

    // Perform actions based on the reason for calling.
    switch( fdwReason ) 
    { 
        case DLL_PROCESS_ATTACH:
         // Initialize once for each new process.
         // Return FALSE to fail DLL load.
            break;

        case DLL_THREAD_ATTACH:
         // Do thread-specific initialization.
            break;

        case DLL_THREAD_DETACH:
         // Do thread-specific cleanup.
            break;

        case DLL_PROCESS_DETACH:
         // Perform any necessary cleanup.
            break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
#endif
#include "Cam.h"


	extern "C"
	{
		POL_EXPORT_FN void OnUpdate(Timestep ts)
		{
			POL_INFO("Game DLL loaded!");
			DEBUG_BREAK(); // den kör den här men måste lösa log macro för scripting
		}
	}

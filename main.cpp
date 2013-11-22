#include "c4d.h"

// forward declarations
Bool RegisterRamPlayer(void);

Bool PluginStart(void)
{	
	if (!RegisterRamPlayer()) return FALSE;
	return TRUE;
}

void PluginEnd(void)
{
}

Bool PluginMessage(LONG id, void *data)
{
	//use the following lines to set a plugin priority
	//
	switch (id)
	{
		case C4DPL_INIT_SYS:
			if (!resource.Init()) return FALSE; 
			return TRUE;

		case C4DMSG_PRIORITY: 
			return TRUE;
	}

	return FALSE;
}

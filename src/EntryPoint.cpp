#include "Program.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

int main(void)
{
	{	
		Program program;
		program.Init();
		program.Update();
		program.End();
	}
}

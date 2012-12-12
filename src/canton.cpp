#include <iostream>
#include <irrlicht.h>

#include <canton.h>
#include <engine.h>

int main(int argc, char* argv[])
{
    IRR; //FIXME: this is a code smell - IRR doesn't seem to initialize properly on its own.

	IRR.Startup();

	while(IRR.device->run())
	{
		IRR.receiver.endEventProcess();
		
        IRR.Update();

		if(IRR.device->isWindowActive())
		{
			IRR.Draw();
		}

		IRR.receiver.startEventProcess();
	}

	IRR.Shutdown();

	return 0;
}

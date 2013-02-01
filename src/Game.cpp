#include <iostream>
#include <irrlicht.h>

#include <Game.h>
#include <Engine.h>

int main(int argc, char* argv[])
{
    IRR; //FIXME: this is a code smell - IRR doesn't seem to initialize properly on its own.

	IRR.Startup();

	while(IRR.device->run())
	{
		IRR.receiver.EndEventProcess();
		
        IRR.Update();

		if(IRR.device->isWindowActive())
		{
			IRR.Draw();
		}

		IRR.receiver.StartEventProcess();
	}

	IRR.Shutdown();

	return 0;
}

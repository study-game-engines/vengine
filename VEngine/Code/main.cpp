#include "vpch.h"
#include "Engine.h"

int main(int argc, char *argv[])
{
	Engine::Init(argc, argv);
	Engine::MainLoop();
	Engine::Cleanup();
	 
	return 0;
}

#pragma once

#include <Sweet.h>
#include <node\Node.h>
#include <typeinfo>
#include <Texture.h>
#include <Game.h>
#include <MY_ResourceManager.h>
#include <Log.h>
#include <ctime>
#include <NumberUtils.h>

#include <MY_Game.h>

#ifdef _DEBUG
	// memory leak debugging
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

#ifdef _DEBUG
int main(void){
	_CrtMemState s1;
	_CrtMemCheckpoint( &s1 );
#else
int WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmd, int show){	
#endif

	Log::THROW_ON_ERROR = true;
	
	//OpenAL_Sound::masterGain = 0; // mute
	OpenAL_Sound::categoricalGain["music"] = 0.8f;
	OpenAL_Sound::categoricalGain["voice"] = 1.f;
	sweet::NumberUtils::seed(time(nullptr)); // seed RNG

	sweet::initialize("Rip It, Grip It, & Sip It!");
	MY_ResourceManager::init();
	MY_ResourceManager::load();
	MY_Game * game = new MY_Game();
	game->init();
	while (game->isRunning){
		game->performGameLoop();
	}
	
	delete game;
	game = nullptr;
	MY_ResourceManager::destruct();
#ifdef _DEBUG
	std::cout << "Final node count: " << Node::nodes.size() << std::endl;

	for(auto n : Node::nodes){
		std::cout << typeid(*n).name() << " " << n << std::endl;
	}
#endif

	sweet::destruct();
	
#ifdef _DEBUG
	_CrtMemDumpAllObjectsSince(&s1);
#endif
}
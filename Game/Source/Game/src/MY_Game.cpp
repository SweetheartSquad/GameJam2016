#pragma once

#include <MY_Game.h>

#include <MY_ResourceManager.h>

#include <MY_Scene_Menu.h>
#include <MY_Scene_Box2D.h>
#include <MY_Scene_Bullet3D.h>
#include <MY_Scene_SurfaceShaders.h>
#include <MY_Scene_ScreenShaders.h>
#include <MY_Scene_VR.h>
#include <MY_Scene_Main.h>
#include <MY_Scene_Instructions.h>

#include <NumberUtils.h>


MY_Game::MY_Game() :
	Game("menu", new MY_Scene_Menu(this), true), // initialize our game with a menu scene,
	bgm(nullptr),
	currentBgm(1)
{
	scenes["game"] = nullptr;
	scenes["intro"] = nullptr;
	scenes["instructions"] = nullptr;
}

MY_Game::~MY_Game(){}

void MY_Game::addSplashes(){
	// add default splashes
	Game::addSplashes();

	// add custom splashes
	addSplash(new Scene_Splash(this, new Texture("assets/textures/GGJ-splash.png", false, true), MY_ResourceManager::globalAssets->getAudio("PORTAL")->sound));
}

void MY_Game::playBGM(){
	if(bgm != nullptr){
		bgm->stop();
	}
	
	std::stringstream ss;
	ss << "BGM_" << currentBgm;
	if(++currentBgm > NUM_BGM_TRACKS){
		currentBgm = 1;
	}
	bgm = MY_ResourceManager::globalAssets->getAudio(ss.str())->sound;
	bgm->play(false);
}

void MY_Game::update(Step * _step){
	Game::update(_step);

	if(bgm != nullptr){
		bgm->update(_step);
		if(bgm->source->state != AL_PLAYING){
			playBGM();
		}
	}else if(currentSceneKey == "menu"){
		playBGM();
	}
}
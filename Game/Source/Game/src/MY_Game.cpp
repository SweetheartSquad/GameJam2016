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

#include <NumberUtils.h>


MY_Game::MY_Game() :
	Game("menu", new MY_Scene_Menu(this), true), // initialize our game with a menu scene,
	bgm(nullptr)
{
	scenes["game"] = new MY_Scene_Main(this);

	playBGM();
}

MY_Game::~MY_Game(){}

void MY_Game::addSplashes(){
	// add default splashes
	//Game::addSplashes();

	// add custom splashes
	//addSplash(new Scene_Splash(this, MY_ResourceManager::globalAssets->getTexture("DEFAULT")->texture, MY_ResourceManager::globalAssets->getAudio("DEFAULT")->sound));
}

void MY_Game::playBGM(){
	if(bgm != nullptr){
		bgm->stop();
	}
	
	std::stringstream ss;
	ss << "BGM_" << sweet::NumberUtils::randomInt(1,3);
	bgm = MY_ResourceManager::globalAssets->getAudio(ss.str())->sound;
	bgm->play(true);
}
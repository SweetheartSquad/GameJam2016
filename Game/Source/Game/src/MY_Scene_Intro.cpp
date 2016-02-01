#pragma once

#include <MY_Game.h>
#include <MY_Scene_Intro.h>
#include <MY_Scene_Main.h>
#include <Slide.h>

MY_Scene_Intro::MY_Scene_Intro(Game * _game) :
	SlideShowScene(_game)
{
	push(new Slide(MY_ResourceManager::globalAssets->getTexture("intro_bg")->texture));

	uiLayer->eventManager.addEventListener("click", [this](sweet::Event * _event){
		changeSlide(true);
	});

	eventManager->addEventListener("overflow", [this](sweet::Event * _event){
		game->scenes["game"] = new MY_Scene_Main(dynamic_cast<MY_Game *>(game));
		game->switchScene("game", true);
	});

	changeSlide(true);
}
#pragma once

#include <MY_Scene_Win.h>
#include <MY_ResourceManager.h>
#include <Slide.h>

MY_Scene_Win::MY_Scene_Win(Game * _game) :
	SlideShowScene(_game)
{

	push(new Slide(MY_ResourceManager::globalAssets->getTexture("win_bg1")->texture));
	push(new Slide(MY_ResourceManager::globalAssets->getTexture("win_bg2")->texture));
	push(new Slide(MY_ResourceManager::globalAssets->getTexture("win_bg3")->texture));

	uiLayer->eventManager.addEventListener("click", [this](sweet::Event * _event){
		changeSlide(true);
	});

	eventManager->addEventListener("overflow", [this](sweet::Event * _event){
		game->switchScene("menu", true);
	});

	changeSlide(true);
}

MY_Scene_Win::~MY_Scene_Win(){
}
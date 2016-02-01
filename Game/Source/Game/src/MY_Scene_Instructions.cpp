#pragma once

#include <MY_Scene_Instructions.h>
#include <Slide.h>
#include <EventManager.h>
#include <Easing.h>

MY_Scene_Instructions::MY_Scene_Instructions(Game * _game) :
	SlideShowScene(_game)
{


	push(new Slide(MY_ResourceManager::globalAssets->getTexture("instructions_bg")->texture));
	push(new Slide(MY_ResourceManager::globalAssets->getTexture("instructions_bg2")->texture));

	uiLayer->eventManager.addEventListener("click", [this](sweet::Event * _event){
		changeSlide(true);
	});

	eventManager->addEventListener("overflow", [this](sweet::Event * _event){
		game->switchScene("menu", true);
	});
	eventManager->addEventListener("underflow", [this](sweet::Event * _event){
		game->switchScene("menu", true);
	});

	changeSlide(true);
}
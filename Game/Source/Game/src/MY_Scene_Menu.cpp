#pragma once

#include <MY_Scene_Menu.h>
#include <shader/ComponentShaderText.h>
#include <sweet/UI.h>
#include <MY_Button.h>
#include <MY_Scene_Instructions.h>

MY_Scene_Menu::MY_Scene_Menu(Game * _game) :
	MY_Scene_Base(_game)
{
	// Create a linear layout to contain all of our menu items
	// remember that these elements are all going to exist in the uiLayer, so we pass in its physics world in the constructors
	NodeUI * layout = new NodeUI(uiLayer->world);
	// set the layout's size to 100% of its parent
	layout->setRationalHeight(1.f);
	layout->setRationalWidth(1.f);
	layout->setBackgroundColour(1.f, 1.f, 1.f);
	layout->background->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("menu_bg")->texture);
	layout->background->mesh->setScaleMode(GL_NEAREST);

	MY_Button * gameButton = new MY_Button(uiLayer->world, "menu_play");
	MY_Button * howButton = new MY_Button(uiLayer->world, "menu_howtoplay");
	MY_Button * exitButton = new MY_Button(uiLayer->world, "menu_quit");

	float gameBtnW = 0.128f;
	float gameBtnH = 0.15;
	float gameBtnX = 0.83f;
	float gameBtnY = 0.475f;

	float exitBtnW = 0.138f;
	float exitBtnH = 0.15f;
	float exitBtnX = 0.83f;
	float exitBtnY = 0.3f;

	gameButton->setRationalWidth(1.f, layout);
	gameButton->setRationalHeight(1.f, layout);
	howButton->setRationalWidth(1.f, layout);
	howButton->setRationalHeight(1.f, layout);
	exitButton->setRationalWidth(1.f, layout);
	exitButton->setRationalHeight(1.f, layout);
	
	gameButton->setMarginLeft(gameBtnX);
	gameButton->setMarginRight(1.f - gameBtnX - gameBtnW);
	gameButton->setMarginBottom(gameBtnY);
	gameButton->setMarginTop(1.f - gameBtnY - gameBtnH);
	howButton->setMarginLeft(exitBtnX);
	howButton->setMarginRight(1.f - exitBtnX - exitBtnW);
	howButton->setMarginBottom(exitBtnY);
	howButton->setMarginTop(1.f - exitBtnY - exitBtnH);
	exitButton->setMarginLeft(exitBtnX);
	exitButton->setMarginRight(1.f - exitBtnX - exitBtnW);
	exitButton->setMarginBottom((exitBtnY - 0.15f));
	exitButton->setMarginTop(1.f - (exitBtnY - 0.15f) - exitBtnH);
	
	// make the buttons clickable
	gameButton->setMouseEnabled(true);
	howButton->setMouseEnabled(true);
	exitButton->setMouseEnabled(true);

	// add listeners to each label, making them buttons that take the player to different scenes
	gameButton->eventManager.addEventListener("click", [&](sweet::Event * _event){
		game->switchScene("game", false);
	});
	howButton->eventManager.addEventListener("click", [&](sweet::Event * _event){
		if(game->scenes["instructions"] != nullptr){
			delete game->scenes["instructions"];
		}
		game->scenes["instructions"] = new MY_Scene_Instructions(game);
		game->switchScene("instructions", false);
	});
	exitButton->eventManager.addEventListener("click", [&](sweet::Event * _event){
		game->exit();
	});

	// add the labels to the layout
	layout->addChild(gameButton);
	layout->addChild(howButton);
	layout->addChild(exitButton);

	// add the layout to the uiLayer
	uiLayer->addChild(layout);

	uiLayer->invalidateLayout();

	// add a mouse indicator (AKA a cursor) to the uiLayer so that the user can see what they're doing
	uiLayer->addMouseIndicator();
}
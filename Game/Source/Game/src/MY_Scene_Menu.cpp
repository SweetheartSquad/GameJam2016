#pragma once

#include <MY_Scene_Menu.h>
#include <shader/ComponentShaderText.h>
#include <sweet/UI.h>

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

	NodeUI * gameButton = new NodeUI(uiLayer->world);
	NodeUI * exitButton = new NodeUI(uiLayer->world);

	gameButton->boxSizing = kCONTENT_BOX;
	exitButton->boxSizing = kCONTENT_BOX;

	
	gameButton->setRationalWidth(0.128f, layout);
	gameButton->setRationalHeight(0.15f, layout);
	exitButton->setRationalWidth(0.138f, layout);
	exitButton->setRationalHeight(0.15f, layout);
	
	gameButton->setMarginLeft(0.83f);
	exitButton->setMarginLeft(0.83f);

	gameButton->setMarginBottom(0.475f);
	exitButton->setMarginBottom(0.3f);

	
	gameButton->setBackgroundColour(1,0,1,0.5f);
	exitButton->setBackgroundColour(1,1,0,0.5f);
	
	// make the labels clickable
	gameButton->setMouseEnabled(true);
	exitButton->setMouseEnabled(true);

	// add listeners to each label, making them buttons that take the player to different scenes
	gameButton->eventManager.addEventListener("click", [&](sweet::Event * _event){
		game->switchScene("game", false);
	});
	exitButton->eventManager.addEventListener("click", [&](sweet::Event * _event){
		game->exit();
	});

	// add the labels to the layout
	layout->addChild(gameButton);
	layout->addChild(exitButton);

	// add the layout to the uiLayer
	uiLayer->addChild(layout);

	uiLayer->invalidateLayout();

	// add a mouse indicator (AKA a cursor) to the uiLayer so that the user can see what they're doing
	uiLayer->addMouseIndicator();
}
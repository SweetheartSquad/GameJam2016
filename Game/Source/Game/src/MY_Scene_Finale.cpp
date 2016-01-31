#pragma once

#include <MY_Scene_Finale.h>

MY_Scene_Finale::MY_Scene_Finale(Game * _game) :
	MY_Scene_Base(_game)
{
	// Create a linear layout to contain all of our menu items
	// remember that these elements are all going to exist in the uiLayer, so we pass in its physics world in the constructors
	NodeUI * layout = new NodeUI(uiLayer->world);
	// set the layout's size to 100% of its parent
	layout->setRationalHeight(1.f);
	layout->setRationalWidth(1.f);
	layout->setBackgroundColour(1.f, 1.f, 1.f);
	layout->background->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("win_bg")->texture);
	layout->background->mesh->setScaleMode(GL_NEAREST);

	// add the layout to the uiLayer
	uiLayer->addChild(layout);

	uiLayer->invalidateLayout();

	// add a mouse indicator (AKA a cursor) to the uiLayer so that the user can see what they're doing
	uiLayer->addMouseIndicator();
}

void MY_Scene_Finale::update(Step * _step){
	if(mouse->leftJustPressed()){
		game->switchScene("menu", true);
	}

	MY_Scene_Base::update(_step);
}
#pragma once

#include <MY_Game.h>
#include <MY_Scene_Intro.h>
#include <MY_Scene_Main.h>

#include <Easing.h>

MY_Scene_Intro::MY_Scene_Intro(Game * _game) :
	MY_Scene_MenuBase(_game)
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
	NodeUI * layout2 = new NodeUI(uiLayer->world);
	// set the layout's size to 100% of its parent
	layout2->setRationalHeight(1.f);
	layout2->setRationalWidth(1.f);
	layout2->setBackgroundColour(1.f, 1.f, 1.f);
	layout2->background->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("intro_bg")->texture);
	layout2->background->mesh->setScaleMode(GL_NEAREST);
	layout2->setMarginLeft(1.f);
	layout2->boxSizing = kCONTENT_BOX;

	// add the layout to the uiLayer
	uiLayer->addChild(layout);
	uiLayer->addChild(layout2);

	uiLayer->invalidateLayout();

	// add a mouse indicator (AKA a cursor) to the uiLayer so that the user can see what they're doing
	uiLayer->addMouseIndicator();

	t = new Timeout(1.f, [this, layout](sweet::Event * _event){
		glm::vec2 v = sweet::getWindowDimensions();
		uiLayer->cam->resize(v.x,2*v.x,0,v.y);
		layout->setMarginLeft(-1.f);
		layout->setRationalWidth(2.f);
		layout->background->mesh->replaceTextures(MY_ResourceManager::globalAssets->getTexture("intro_bg")->texture);
	});
	t->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		glm::vec2 v = sweet::getWindowDimensions();
		float progress = Easing::easeInOutCirc(_event->getFloatData("progress"), 0, 1, 1);
		uiLayer->cam->resize(progress*v.x,progress*v.x+v.x,0,v.y);
	});

	childTransform->addChild(t, false);
	t->start();
}

void MY_Scene_Intro::update(Step * _step){
	MY_Scene_MenuBase::update(_step);

	if(mouse->leftJustPressed()){
		game->scenes["game"] = new MY_Scene_Main(dynamic_cast<MY_Game *>(game));
		game->switchScene("game", true);
	}
}
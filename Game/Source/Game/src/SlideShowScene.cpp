#pragma once

#include <SlideShowScene.h>
#include <MeshInterface.h>
#include <MY_Scene_Main.h>
#include <MY_Game.h>
#include <EventManager.h>
SlideShowScene::SlideShowScene(Game * _game) :
	MY_Scene_Base(_game),
	currSlide(nullptr),
	isTransitioning(false),
	slide(nullptr)
{

	slideOld = new NodeUI(uiLayer->world);
	slideOld->setRationalWidth(1.f, uiLayer);
	slideOld->setRationalHeight(1.f, uiLayer);
	slideOld->setMouseEnabled(true);

	slideNew= new NodeUI(uiLayer->world);
	slideNew->setRationalWidth(1.f, uiLayer);
	slideNew->setRationalHeight(1.f, uiLayer);
	slideNew->setVisible(false);
	slideNew->setMouseEnabled(false);

	uiLayer->addChild(slideOld);
	uiLayer->addChild(slideNew);
	
	uiLayer->addMouseIndicator();

	// initialize transition
	transition = new Timeout(1.f, [this](sweet::Event * _event){
		eventManager.triggerEvent("transitionComplete");
	});

	// add a default fade-in/out transition
	transition->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float p = _event->getFloatData("progress");
		
		slideOld->childTransform->translate(-slideOld->getWidth() * p, 0, 0, false);
		slideNew->childTransform->translate(slideNew->getWidth() * (1 - p), 0, 0, false);
	});

	uiLayer->eventManager.addEventListener("click", [this](sweet::Event * _event){
		if(forwards.size() > 0){
			setNewSlide();
		}else{
			game->scenes["game"] = new MY_Scene_Main(dynamic_cast<MY_Game *>(game));
			game->switchScene("game", true);
		}
	});

	eventManager.addEventListener("transitionComplete", [this](sweet::Event * _event){
		isTransitioning = false;
		slide = slideNew;
		if(currSlide->sound != nullptr){
			currSlide->sound->play();
		}		
	});
	
}

SlideShowScene::~SlideShowScene(){
}

void SlideShowScene::update(Step * _step){
	eventManager.update(_step);
	transition->update(_step);
	MY_Scene_Base::update(_step);
}

void SlideShowScene::push(Slide * _slide){
	forwards.push_back(_slide);
}

void SlideShowScene::next(){
	if(forwards.size() > 0){
		if(currSlide != nullptr){
			backwards.push_back(currSlide);
		}
	
		currSlide = forwards.front();
		forwards.erase(forwards.begin());
	}else{
		eventManager.triggerEvent("slidesEnd");
	}
}

void SlideShowScene::prev(){
	if(backwards.size() > 0){
		if(currSlide != nullptr){
			forwards.push_back(currSlide);
		}
	
		currSlide = backwards.back();
		backwards.pop_back();
	}else{
		eventManager.triggerEvent("slidesBegin");
	}
}

void SlideShowScene::setNewSlide(){
	if(!isTransitioning){
		isTransitioning = true;
		transition->restart();

		// setup transition
		if(slide != nullptr){
			// The fancy switcharoo
			slideNew = slideOld;
			slideOld = slide;
		}
		slideOld->childTransform->translate(0, 0, 0, false);
		slideOld->setVisible(true);
		next();
		if(currSlide != nullptr){
			slideNew->background->mesh->replaceTextures(currSlide->tex);
			slideNew->setVisible(true);
			slideNew->setMouseEnabled(false);
			slideNew->childTransform->translate(slideNew->getWidth(), 0, 0, false);
		}else{
			// Can't transition
			isTransitioning = false;
		}
		slideNew->setBackgroundColour(1.f, 0, 0);
		slideOld->setBackgroundColour(0, 1.f, 0);
	}
}
#pragma once

#include <SlideShowScene.h>
#include <MeshInterface.h>
#include <MY_Game.h>
#include <EventManager.h>
#include <Keyboard.h>
#include <Slide.h>

SlideShowScene::SlideShowScene(Game * _game) :
	MY_Scene_Base(_game),
	currSlide(nullptr),
	isTransitioning(false),
	isTransitioningForwards(true),
	slide(nullptr)
{
	eventManager = new sweet::EventManager();

	slideOld = new NodeUI(uiLayer->world);
	slideOld->setRationalWidth(1.f, uiLayer);
	slideOld->setRationalHeight(1.f, uiLayer);

	slideNew= new NodeUI(uiLayer->world);
	slideNew->setRationalWidth(1.f, uiLayer);
	slideNew->setRationalHeight(1.f, uiLayer);

	uiLayer->addChild(slideOld);
	uiLayer->addChild(slideNew);
	
	uiLayer->addMouseIndicator();

	// initialize transition
	transition = new Timeout(1.f, [this](sweet::Event * _event){
		eventManager->triggerEvent("transitionComplete");
	});

	// add a default fade-in/out transition
	transition->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float p = _event->getFloatData("progress");
		if(isTransitioningForwards){
			// right to left
			slideOld->childTransform->translate(-slideOld->getWidth() * p, 0, 0, false);
			slideNew->childTransform->translate(slideNew->getWidth() * (1 - p), 0, 0, false);
		}else{
			// left to right
			slideOld->childTransform->translate(slideOld->getWidth() * p, 0, 0, false);
			slideNew->childTransform->translate(-slideNew->getWidth() * (1 - p), 0, 0, false);
		}
	});

	eventManager->addEventListener("transitionComplete", [this](sweet::Event * _event){
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
	eventManager->update(_step);
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
	}
}

void SlideShowScene::prev(){
	if(backwards.size() > 0){
		if(currSlide != nullptr){
			forwards.insert(forwards.begin(), currSlide);
		}
	
		currSlide = backwards.back();
		backwards.pop_back();
	}
}

void SlideShowScene::changeSlide(bool _isForwards){
	if(_isForwards){
		if(forwards.size() > 0){
			setNewSlide(true);
		}else{
			eventManager->triggerEvent("overflow");
		}
	}else{
		if(backwards.size() > 0){
			setNewSlide(false);
		}else{
			eventManager->triggerEvent("underflow");
		}
	}
}

void SlideShowScene::setNewSlide(bool _isForwards){
	if(!isTransitioning){
		isTransitioning = true;
		isTransitioningForwards = _isForwards;
		transition->restart();

		// setup transition
		if(slide != nullptr){
			// The fancy switcharoo
			slideNew = slideOld;
			slideOld = slide;
		}
		// get next slide
		if(isTransitioningForwards){
			next();
		}else{
			prev();
		}

		if(currSlide != nullptr){
			slideNew->background->mesh->replaceTextures(currSlide->tex);
			slideOld->childTransform->translate(0, 0, 0, false);
			if(isTransitioningForwards){
				slideNew->childTransform->translate(slideNew->getWidth(), 0, 0, false);
			}else{
				slideNew->childTransform->translate(-slideNew->getWidth(), 0, 0, false);
			}
		}else{
			// Can't transition
			Log::error("No slide could be found");
			isTransitioning = false;
		}
		/*
		slideNew->setBackgroundColour(1.f, 0, 0);
		slideOld->setBackgroundColour(0, 1.f, 0);
		*/
	}
}
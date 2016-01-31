#pragma once

#include <MY_Scene_Finale.h>
#include <MY_Scene_Loss.h>
#include <HorizontalLinearLayout.h>
#include <MY_Scene_Main.h>
#include <MY_Scene_Win.h>

MY_Scene_Finale::MY_Scene_Finale(Game * _game) :
	MY_Scene_MenuBase(_game),
	correct(0),
	nextIsRight(true),
	curTarget("ripit")
{
	eventManager = new sweet::EventManager();

	// Create a linear layout to contain all of our menu items
	// remember that these elements are all going to exist in the uiLayer, so we pass in its physics world in the constructors
	NodeUI * layout = new NodeUI(uiLayer->world);
	// set the layout's size to 100% of its parent
	layout->setRationalHeight(1.f);
	layout->setRationalWidth(1.f);
	layout->setBackgroundColour(1.f, 1.f, 1.f);
	layout->background->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("finale_ripit")->texture);
	layout->background->mesh->setScaleMode(GL_NEAREST);


	HorizontalLinearLayout * hl = new HorizontalLinearLayout(uiLayer->world);
	layout->addChild(hl);
	hl->setRationalHeight(1.f);
	hl->setRationalWidth(1.f);
	hl->setMarginBottom(0.7f);
	hl->horizontalAlignment = kCENTER;
	hl->verticalAlignment = kMIDDLE;

	pressDisplay = new NodeUI(uiLayer->world);
	pressDisplay->background->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("press_right")->texture);
	pressDisplay->setRationalHeight(1.f, hl);
	pressDisplay->background->mesh->setScaleMode(GL_NEAREST);

	hl->addChild(pressDisplay);

	// add the layout to the uiLayer
	uiLayer->addChild(layout);

	uiLayer->invalidateLayout();

	// add a mouse indicator (AKA a cursor) to the uiLayer so that the user can see what they're doing
	uiLayer->addMouseIndicator();

	eventManager->addEventListener("ripit", [this, layout](sweet::Event * _event){
		layout->background->mesh->replaceTextures(MY_ResourceManager::globalAssets->getTexture("finale_gripit")->texture);
		timer->restart();
		curTarget = "gripit";
		MY_ResourceManager::globalAssets->getAudio(curTarget)->sound->play(false);
	});
	eventManager->addEventListener("gripit", [this, layout](sweet::Event * _event){
		layout->background->mesh->replaceTextures(MY_ResourceManager::globalAssets->getTexture("finale_sipit")->texture);
		timer->restart();
		curTarget = "sipit";
		MY_ResourceManager::globalAssets->getAudio(curTarget)->sound->play(false);
	});
	eventManager->addEventListener("sipit", [this, layout](sweet::Event * _event){
		timer->stop();
		pressDisplay->setVisible(false);

		auto newScene = new MY_Scene_Win(game);
		game->scenes["win"] = newScene;
		game->switchScene("win", true);
	});


	timer = new Timeout(4.f, [this](sweet::Event * _event){
		auto newScene = new MY_Scene_Main(reinterpret_cast<MY_Game *>(game), true);
		game->scenes["main"] = newScene;
		game->switchScene("main", true);
	});

	childTransform->addChild(timer, false);

	timer->start();
	
		MY_ResourceManager::globalAssets->getAudio(curTarget)->sound->play(false);
}

MY_Scene_Finale::~MY_Scene_Finale(){
	delete eventManager;
}

void MY_Scene_Finale::update(Step * _step){
	eventManager->update(_step);

	auto shakeVec = sweet::NumberUtils::randomVec3(glm::vec3(-3, -3, 0), glm::vec3(3, 3, 0));

	if(pressDisplay->isVisible()){
		pressDisplay->invalidateLayout();
		pressDisplay->setWidth(pressDisplay->getHeight(true,true));
		if(nextIsRight){
			// make sure to check incorrect first; that way they can't just hit both
			if(keyboard->keyJustDown(GLFW_KEY_A) || keyboard->keyJustDown(GLFW_KEY_LEFT) || mouse->leftJustPressed()){
				incorrectPress();
				MY_ResourceManager::globalAssets->getAudio("BUTTON")->sound->play(false);
			}else if(keyboard->keyJustDown(GLFW_KEY_D) || keyboard->keyJustDown(GLFW_KEY_RIGHT) || mouse->rightJustPressed()){
				correctPress();
				MY_ResourceManager::globalAssets->getAudio("BUTTON")->sound->play(false);
				shakeVec = sweet::NumberUtils::randomVec3(glm::vec3(-10, -10, 0), glm::vec3(10, 10, 0));
			}
		}else{
			if(keyboard->keyJustDown(GLFW_KEY_D) || keyboard->keyJustDown(GLFW_KEY_RIGHT) || mouse->rightJustPressed()){
				incorrectPress();
				MY_ResourceManager::globalAssets->getAudio("BUTTON")->sound->play(false);
			}else if(keyboard->keyJustDown(GLFW_KEY_A) || keyboard->keyJustDown(GLFW_KEY_LEFT) || mouse->leftJustPressed()){
				correctPress();
				MY_ResourceManager::globalAssets->getAudio("BUTTON")->sound->play(false);
				shakeVec = sweet::NumberUtils::randomVec3(glm::vec3(-10, -10, 0), glm::vec3(10, 10, 0));
			}
		}
		uiLayer->cam->firstParent()->translate(shakeVec, false);
	}else{
		 uiLayer->cam->firstParent()->translate(glm::vec3(0, 0, 0), false);
	}

	MY_Scene_MenuBase::update(_step);
}

void MY_Scene_Finale::correctPress(){
	correct += 1;
	if(correct >= NUM_REQUIRED_PRESSES){
		passTarget();
	}
	nextIsRight = !nextIsRight;
	if(nextIsRight){
		pressDisplay->background->mesh->replaceTextures(MY_ResourceManager::globalAssets->getTexture("press_right")->texture);
	}else{
		pressDisplay->background->mesh->replaceTextures(MY_ResourceManager::globalAssets->getTexture("press_left")->texture);
	}
}

void MY_Scene_Finale::incorrectPress(){
	// do something bad here
}

void MY_Scene_Finale::passTarget(){
	correct = 0;
	timer->restart();
	eventManager->triggerEvent(curTarget);
}
#pragma once

#include <MY_Demon.h>
#include <NumberUtils.h>
#include <MY_ResourceManager.h>

#define NUM_DEMON_TEXTURES 2
#define TIMEOUT_TIME 1.0

MY_DemonSpirit::MY_DemonSpirit(Shader * _shader, MY_Demon * _possessed) : 
	Sprite(_shader),
	possessed(_possessed),
	speed(0),
	state(kIN)
{
	setPrimaryTexture(MY_ResourceManager::globalAssets->getTexture("demon_spirit")->texture);

	stunTimer = new Timeout(1.f, [this](sweet::Event * _event){
		state = kOUT;
	});
	childTransform->addChild(stunTimer, false);
}

void MY_DemonSpirit::update(Step * _step){
	// accelerate towards the possession target (which is at 0,0,0)
	glm::vec3 a = firstParent()->getTranslationVector();
	float ma = glm::length(a);

	float accelMod = 0.f;
	float damping = 1.f;
	switch(state){
	case kIN:
		accelMod = 0.1f;
		damping = 0.1f;
		// if the spirit is far from the origin, they get ripped out
		if(ma > 5){
			ripIt();
		}
		break;

	case kSTUNNED:
		accelMod = 0.001;
		a += sweet::NumberUtils::randomVec3(glm::vec3(-3), glm::vec3(3));
		damping = 0.5f;
		break;

	case kOUT:
		accelMod = 0.001f;
		a += sweet::NumberUtils::randomVec3(glm::vec3(-3), glm::vec3(3));
		damping = 0.2f;
		// if the spirit is close to the origin, they can repossess the body
		if(ma < 5){
			getBackInThere();
		}
		break;

	case kDEAD:
		return;
		break;

	default: break;
	}
		
	a *= -accelMod;
	speed += a;
	speed *= (1-damping); // damping
	firstParent()->translate(speed);

	Sprite::update(_step);
}

void MY_DemonSpirit::ripIt(){
	std::cout << "demon ripped" << std::endl;
	state = kSTUNNED;
	stunTimer->restart();
}

void MY_DemonSpirit::gripIt(){
	std::cout << "demon gripped" << std::endl;
	state = kSTUNNED;
	stunTimer->restart();
}

void MY_DemonSpirit::sipIt(){
	std::cout << "demon sipped" << std::endl;
	state = kDEAD;
}

void MY_DemonSpirit::getBackInThere(){
	state = kIN;
}

glm::vec3 MY_DemonSpirit::getGamePos(){
	return firstParent()->getTranslationVector() + possessed->firstParent()->getTranslationVector();
}

MY_Demon::MY_Demon(Shader * _shader, Transform * _target) :
	Sprite(_shader), 
	speed(0.1f),
	currentState(kWALKING),
	target(_target),
	spirit(new MY_DemonSpirit(_shader, this))
{
	childTransform->addChild(spirit);

	int demonTexId = sweet::NumberUtils::randomInt(1, NUM_DEMON_TEXTURES);
	setPrimaryTexture(MY_ResourceManager::globalAssets->getTexture("demon_" + std::to_string(demonTexId))->texture);

	stateTimeout = new Timeout(TIMEOUT_TIME, [this](sweet::Event * _event){
		bool randState = sweet::NumberUtils::randomBool();
		if(randState) {
			currentState = kWALKING;
		}else {
			currentState = kIDLE;
		}
		stateTimeout->restart();
	});
	stateTimeout->start();

	eventManager.addEventListener("playerCollision", [this](sweet::Event * _event){
		// Player collision code
	});
}

void MY_Demon::render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) {

	Sprite::render(_matrixStack, _renderOptions);
}

void MY_Demon::update(Step * _step) {
	eventManager.update(_step);
	stateTimeout->update(_step);
	if(target != nullptr && currentState == kWALKING){
		float targDir = target->getTranslationVector().x < firstParent()->getTranslationVector().x ? -1.f : 1.f;
		firstParent()->translate(speed * targDir, 0.f, 0.f);
	}
	Sprite::update(_step);
}

void MY_Demon::unload() {

	Sprite::unload();
}

void MY_Demon::load() {
	
	Sprite::load();
}

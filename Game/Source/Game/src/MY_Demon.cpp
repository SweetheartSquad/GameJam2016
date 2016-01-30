#pragma once

#include <MY_Demon.h>
#include <NumberUtils.h>
#include <MY_ResourceManager.h>

#define NUM_DEMON_TEXTURES 2
#define TIMEOUT_TIME 1.0

MY_Demon::MY_Demon(Shader * _shader, Transform * _target) :
	Sprite(_shader), 
	speed(0.1f),
	currentState(kWALKING),
	target(_target)
{
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

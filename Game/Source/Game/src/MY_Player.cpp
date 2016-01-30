#pragma once

#include <MY_Player.h>
#include <MY_ResourceManager.h>
#include <JoystickVirtual.h>
#include <SpriteSheet.h>
#include <SpriteSheetAnimation.h>

MY_Player::MY_Player(Shader * _shader) :
	Sprite(_shader),
	speed(0.5f),
	joystick(new JoystickVirtual(0))
{
	spriteSheet = new SpriteSheet(MY_ResourceManager::globalAssets->getTexture("spritesheet")->texture);

	setPrimaryTexture(MY_ResourceManager::globalAssets->getTexture("DEFAULT")->texture);

	auto walkAnim = new SpriteSheetAnimation(0.4f);
	walkAnim->pushFramesInRange(0, 30, 240, 296, spriteSheet->texture->width, spriteSheet->texture->height);

	spriteSheet->addAnimation("walk", walkAnim);
	setCurrentAnimation("walk");

	eventManager.addEventListener("demonCollision", [this](sweet::Event * _event){
		// Demon collision code
	});
}

void MY_Player::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) {
	Sprite::render(_matrixStack, _renderOptions);
}

void MY_Player::update(Step * _step) {
	
	eventManager.update(_step);
	joystick->update(_step);

	if(joystick->getAxis(joystick->axisLeftX) > 0.5f) {
		firstParent()->translate(speed, 0.f, 0.f);
	}

	if(joystick->getAxis(joystick->axisLeftX) < -0.5f) {
		firstParent()->translate(-speed, 0.f, 0.f);
	}

	Sprite::update(_step);
}

void MY_Player::unload() {

	Sprite::unload();
}

void MY_Player::load() {

	Sprite::load();
}

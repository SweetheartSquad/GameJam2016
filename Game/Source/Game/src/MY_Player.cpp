#pragma once

#include <MY_Player.h>
#include <MY_ResourceManager.h>
#include <JoystickVirtual.h>
#include <SpriteSheet.h>
#include <SpriteSheetAnimation.h>

MY_Player::MY_Player(Shader * _shader) :
	Sprite(_shader),
	speed(0.25f),
	health(100.f),
	invincibilityTimer(0.f),
	invincibilityTimerLength(2.f),
	joystick(new JoystickVirtual(0)),
	isDead(false)
{
	spriteSheet = new SpriteSheet(MY_ResourceManager::globalAssets->getTexture("spritesheet")->texture);

	setPrimaryTexture(MY_ResourceManager::globalAssets->getTexture("player")->texture);
	
	auto anim = new SpriteSheetAnimation(0.4f);
	anim->pushFramesInRange(0, 3, 256, 1024, spriteSheet->texture->width, spriteSheet->texture->height);
	spriteSheet->addAnimation("idle", anim);

	anim = new SpriteSheetAnimation(0.4f);
	anim->pushFramesInRange(8, 11, 256, 1024, spriteSheet->texture->width, spriteSheet->texture->height);
	spriteSheet->addAnimation("walk", anim);

	mesh->setScaleMode(GL_NEAREST);

	setCurrentAnimation("idle");

	eventManager.addEventListener("demonCollision", [this](sweet::Event * _event){
		// Demon collision code
		
		if(invincibilityTimer <= 0){
			// Take damage
			health -= _event->getFloatData("damage");
			std::stringstream s;
			s << "HEALTH: " << health;
			Log::info(s.str());

			// Reset invincibility timer
			invincibilityTimer = invincibilityTimerLength;
			// Invincibility starts
			eventManager.triggerEvent("invincibilityStart");
		}
	});
}

void MY_Player::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) {
	Sprite::render(_matrixStack, _renderOptions);
}

void MY_Player::update(Step * _step) {
	
	eventManager.update(_step);
	joystick->update(_step);

	// Update invincibility timer if needed
	if(invincibilityTimer > 0){
		invincibilityTimer -= _step->getDeltaTime();
		if(invincibilityTimer <= 0){
			// Invincibililty ends
			eventManager.triggerEvent("invincibilityEnd");
		}
	}

	// Check health
	if(health <= 0.f){
		// lives?
		// game over
		isDead = true;
		eventManager.triggerEvent("dead");
	}
	
	setCurrentAnimation("idle");

	if(joystick->getAxis(joystick->axisLeftX) > 0.5f) {
		firstParent()->translate(speed, 0.f, 0.f);
		setCurrentAnimation("walk");
	}else if(joystick->getAxis(joystick->axisLeftX) < -0.5f) {
		firstParent()->translate(-speed, 0.f, 0.f);
		setCurrentAnimation("walk");
	}

	Sprite::update(_step);
}

void MY_Player::unload() {

	Sprite::unload();
}

void MY_Player::load() {

	Sprite::load();
}

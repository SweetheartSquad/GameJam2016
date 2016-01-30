#pragma once

#include <MY_Player.h>
#include <MY_ResourceManager.h>
#include <JoystickVirtual.h>
#include <SpriteSheet.h>
#include <SpriteSheetAnimation.h>

MY_Player::MY_Player(Shader * _shader) :
	Sprite(_shader),
	speed(0.25f),
	lives(5),
	invincibilityTimer(0.f),
	invincibilityTimerLength(2.f),
	joystick(new JoystickVirtual(0)),
	isDead(false),
	scaleAnim(1),
	invincible(false)
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
		if(!isDead){
			if(invincibilityTimer <= FLT_EPSILON){
				// Take damage
				--lives;//_event->getFloatData("damage");
				std::stringstream s;
				s << "LIVES: " << lives;
				Log::info(s.str());

				// Reset invincibility timer
				invincibilityTimer = invincibilityTimerLength;
				// Invincibility starts
				eventManager.triggerEvent("invincibilityStart");
				eventManager.triggerEvent("tookDamage");
				invincible = true;
			}
		}
	});

	for(auto & v : mesh->vertices){
		v.y += 0.5f;
	}
	mesh->dirty = true;

	idleScaleAnim = new Animation<float>(&scaleAnim.y);
	idleScaleAnim->tweens.push_back(new Tween<float>(0.4f, 0.1f, Easing::kEASE_OUT_CIRC));
	idleScaleAnim->tweens.push_back(new Tween<float>(0.2f, -0.1f, Easing::kEASE_IN_CIRC));
	idleScaleAnim->hasStart = true;
	idleScaleAnim->loopType = Animation<float>::kLOOP;
}

void MY_Player::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) {
	Sprite::render(_matrixStack, _renderOptions);
}

void MY_Player::update(Step * _step) {
	
	eventManager.update(_step);
	if(joystick != nullptr){
		joystick->update(_step);
	}

	if(!isDead){
		// Update invincibility timer if needed
		if(invincibilityTimer > FLT_EPSILON){
			invincibilityTimer -= _step->getDeltaTime();
			if(invincibilityTimer <= 0){
				// Invincibililty ends
				eventManager.triggerEvent("invincibilityEnd");
			invincible = false;
			}
		}

		// Check health
		if(lives <= 0){
			// lives?
			// game over
			isDead = true;
			eventManager.triggerEvent("gameOver");
		}
		setCurrentAnimation("idle");
		idleScaleAnim->update(_step);
		childTransform->scale(scaleAnim, false);
		
		if(joystick != nullptr){
			if(joystick->getAxis(joystick->axisLeftX) > 0.5f) {
				firstParent()->translate(speed, 0.f, 0.f);
				setCurrentAnimation("walk");
			}else if(joystick->getAxis(joystick->axisLeftX) < -0.5f) {
				firstParent()->translate(-speed, 0.f, 0.f);
				setCurrentAnimation("walk");
			}
		}
	}
	Sprite::update(_step);
}

void MY_Player::unload() {

	Sprite::unload();
}

void MY_Player::load() {

	Sprite::load();
}

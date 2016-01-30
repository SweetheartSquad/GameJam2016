#pragma once

#include <MY_Player.h>
#include <MY_ResourceManager.h>
#include <JoystickVirtual.h>
#include <SpriteSheet.h>
#include <SpriteSheetAnimation.h>

#include <NumberUtils.h>

int MY_Player::lives = MAX_LIVES;

MY_Player::MY_Player(Shader * _shader) :
	Sprite(_shader),
	paused(false),
	isDead(false),
	speed(0.1f),
	bounds(0),
	scaleAnim(1),
	invincible(false),
	invincibilityTimer(0.f),
	invincibilityTimerLength(2.f),
	joystick(new JoystickVirtual(0)),
	footsetpSound(MY_ResourceManager::globalAssets->getAudio("FOOTSTEP")->sound),
	stepTimer(0.f),
	highStep(false)
{
	spriteSheet = new SpriteSheet(MY_ResourceManager::globalAssets->getTexture("spritesheet_player")->texture);

	setPrimaryTexture(MY_ResourceManager::globalAssets->getTexture("player")->texture);
	
	auto anim = new SpriteSheetAnimation(0.4f);
	anim->pushFramesInRange(4, 5, 512, 1024, spriteSheet->texture->width, spriteSheet->texture->height);
	spriteSheet->addAnimation("idle", anim);

	anim = new SpriteSheetAnimation(0.16f);
	anim->pushFramesInRange(0, 3, 512, 1024, spriteSheet->texture->width, spriteSheet->texture->height);
	spriteSheet->addAnimation("walk", anim);

	anim = new SpriteSheetAnimation(0.4f);
	anim->pushFramesInRange(8, 9, 512, 1024, spriteSheet->texture->width, spriteSheet->texture->height);
	spriteSheet->addAnimation("sip", anim);

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


	// setup voice timer
	voiceTimer = new Timeout(3.f, [this](sweet::Event * _event){
		std::stringstream ss;
		ss << "VOICE_" << sweet::NumberUtils::randomInt(1,14);
		MY_ResourceManager::globalAssets->getAudio(ss.str())->sound->play();
		voiceTimer->targetSeconds = sweet::NumberUtils::randomFloat(3.f, 6.f);
		voiceTimer->restart();
	});
	voiceTimer->start();
	childTransform->addChild(voiceTimer, false);

	// setup pause timer
	pauseTimer = new Timeout(1.f, [this](sweet::Event * _event){
		paused = false;
	});
	childTransform->addChild(pauseTimer, false);
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
		idleScaleAnim->update(_step);
		childTransform->scale(scaleAnim, false);
		
		if(joystick != nullptr && !paused){
			setCurrentAnimation("idle");
			if(joystick->getAxis(joystick->axisLeftX) > 0.5f) {
				firstParent()->translate(speed, 0.f, 0.f);
				setCurrentAnimation("walk");
				meshTransform->scale(1,1,1, false);
			}else if(joystick->getAxis(joystick->axisLeftX) < -0.5f) {
				firstParent()->translate(-speed, 0.f, 0.f);
				setCurrentAnimation("walk");
				meshTransform->scale(-1,1,1, false);
			}

			if(joystick->getAxis(joystick->axisLeftX) > 0.5f || 
				joystick->getAxis(joystick->axisLeftX) < -0.5f) {
				stepTimer += _step->deltaTime;
				if(stepTimer >= 0.35f) {
					stepTimer = 0.f;
					if(highStep) {
						footsetpSound->setPitch(0.7f);
					}else {
						footsetpSound->setPitch(0.6f);	
					}
					highStep = !highStep;
					footsetpSound->play();
				}
			}else {
				stepTimer = 0.f;
			}

			float x = firstParent()->getTranslationVector().x;
			if(x < -bounds){
				firstParent()->translate(glm::vec3((-bounds - x),0,0));
			}else if(x > bounds){
				firstParent()->translate(glm::vec3((bounds - x),0,0));
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

void MY_Player::pause(float _seconds){
	pauseTimer->targetSeconds = _seconds;
	pauseTimer->restart();
	paused = true;
}
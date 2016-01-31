#pragma once

#include <MY_Player.h>
#include <MY_ResourceManager.h>
#include <JoystickVirtual.h>
#include <SpriteSheet.h>
#include <SpriteSheetAnimation.h>

#include <NumberUtils.h>
#include <MY_Demon.h>

#define HIT_SOUND_COUNT 2
#define FLASH_DURATION 0.05f

int MY_Player::lives = MAX_LIVES;

MY_Player::MY_Player(Shader * _shader) :
	Sprite(_shader),
	state(kIDLE),
	isDead(false),
	speed(0.15f),
	bounds(0),
	scaleAnim(1),
	spirit(nullptr),
	invincible(false),
	invincibilityTimer(0.f),
	invincibilityTimerLength(2.f),
	joystick(new JoystickVirtual(0)),
	footsetpSound(MY_ResourceManager::globalAssets->getAudio("FOOTSTEP")->sound),
	stepTimer(0.f),
	highStep(false),
	flashTimer(0.f),
	mouseWalk(0)
{
	spriteSheet = new SpriteSheet(MY_ResourceManager::globalAssets->getTexture("spritesheet_player")->texture);

	setPrimaryTexture(MY_ResourceManager::globalAssets->getTexture("player")->texture);
	

	auto anim = new SpriteSheetAnimation(0.16f);
	anim->pushFramesInRange(0, 3, 512, 1024, spriteSheet->texture->width, spriteSheet->texture->height);
	spriteSheet->addAnimation("walk", anim);

	anim = new SpriteSheetAnimation(0.4f);
	anim->pushFramesInRange(4, 5, 512, 1024, spriteSheet->texture->width, spriteSheet->texture->height);
	spriteSheet->addAnimation("idle", anim);

	anim = new SpriteSheetAnimation(0.4f);
	anim->pushFramesInRange(6, 7, 512, 1024, spriteSheet->texture->width, spriteSheet->texture->height);
	spriteSheet->addAnimation("hurt", anim);

	anim = new SpriteSheetAnimation(0.4f);
	anim->pushFramesInRange(8, 9, 512, 1024, spriteSheet->texture->width, spriteSheet->texture->height);
	spriteSheet->addAnimation("sip", anim);

	anim = new SpriteSheetAnimation(0.4f);
	anim->pushFramesInRange(10, 11, 512, 1024, spriteSheet->texture->width, spriteSheet->texture->height);
	spriteSheet->addAnimation("rip&grip", anim);

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
				eventManager.triggerEvent("");
				eventManager.triggerEvent("tookDamage");
				invincible = true;

				int randRipitSound = sweet::NumberUtils::randomInt(1, HIT_SOUND_COUNT);
				MY_ResourceManager::globalAssets->getAudio("HIT_SOUND_" + std::to_string(randRipitSound))->sound->play();
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
	stateChangeTimer = new Timeout(1.f, [this](sweet::Event * _event){
		state = delayedState;
	});
	childTransform->addChild(stateChangeTimer, false);

	eventManager.addEventListener("invincibilityStart", [this](sweet::Event * _event){
		//setVisible(false);
	});

	eventManager.addEventListener("invincibilityEnd", [this](sweet::Event * _event){
		setVisible(true);
	});
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

		if(invincible) {
			flashTimer += _step->deltaTime;
			if(flashTimer >= FLASH_DURATION) {
				setVisible(!isVisible());
				flashTimer = 0.f;
			}
		}else {
			flashTimer = 0.f;
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
		
		if(state != kSIP && state != kRIP_AND_GRIP && state != kHURT){
			float s = 0;
			if(glm::abs(mouseWalk) > 0.5f){
				s = glm::sign(mouseWalk);
			}else if(joystick != nullptr){
				if(joystick->getAxis(joystick->axisLeftX) > 0.5f) {
					s = 1;
				}else if(joystick->getAxis(joystick->axisLeftX) < -0.5f) {
					s = -1;
				}else{
					s = 0;
				}
			}

			if(glm::abs(s) > 0.5f){	
				firstParent()->translate(s*speed, 0.f, 0.f);
				state = kWALK;
				meshTransform->scale(s,1,1, false);

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
			}else{
				state = kIDLE;
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

	// animation handling
	switch(state){
		case kWALK: setCurrentAnimation("walk"); break;
		case kSIP: setCurrentAnimation("sip"); break;
		case kRIP_AND_GRIP: setCurrentAnimation("rip&grip"); break;
		case kHURT: setCurrentAnimation("hurt"); break;
		case kIDLE:
		default:
			setCurrentAnimation("idle"); break;
	}

	Sprite::update(_step);
}

void MY_Player::unload() {

	Sprite::unload();
}

void MY_Player::load() {

	Sprite::load();
}

void MY_Player::spawnDemon() {
	spirit = new MY_DemonSpirit(shader, this);
	childTransform->addChild(spirit)->translate(spirit->origin);
	spirit->firstParent()->scale(0.1f);
	spirit->freezeTransformation();
}

void MY_Player::delayChange(float _seconds, PlayerState _state){
	stateChangeTimer->targetSeconds = _seconds;
	stateChangeTimer->restart();
	delayedState = _state;
}

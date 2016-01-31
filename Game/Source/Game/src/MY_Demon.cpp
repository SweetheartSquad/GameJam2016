#pragma once

#include <MY_Demon.h>
#include <NumberUtils.h>
#include <MY_ResourceManager.h>
#include <SpriteSheet.h>
#include <SpriteSheetAnimation.h>

#define NUM_DEMON_TEXTURES 2
#define TIMEOUT_TIME 1.0
#define DEMON_SCALE 10
#define RIPIT_SOUND_COUNT 2
#define GRIPIT_SOUND_COUNT 2
#define SIPIT_SOUND_COUNT 2

#define DEMON_POS_HEAD 0.9f
#define DEMON_POS_TORSO 0.7f
#define DEMON_POS_JOHHNY 0.5f


MY_DemonSpirit::MY_DemonSpirit(Shader * _shader, MY_Demon * _possessed) : 
	Sprite(_shader),
	speed(0),
	state(kIN),
	scaleAnim(3),
	origin(0, DEMON_SCALE, 0.2f),
	possessed(_possessed)
{

	int locIdx = sweet::NumberUtils::randomInt(1, 3);
	float loc = DEMON_POS_JOHHNY;
	if(locIdx == 1) {
		loc = DEMON_POS_HEAD;
	}else if(locIdx == 2) {
		loc = DEMON_POS_TORSO;
	}

	origin = glm::vec3(0.f, loc * DEMON_SCALE, 0.2f);

	setPrimaryTexture(MY_ResourceManager::globalAssets->getTexture("demon_spirit")->texture);

	stunTimer = new Timeout(1.f, [this](sweet::Event * _event){
		state = kOUT;
	});
	childTransform->addChild(stunTimer, false);

	mesh->setScaleMode(GL_NEAREST);

	idleScaleAnim = new Animation<float>(&scaleAnim.y);
	idleScaleAnim->tweens.push_back(new Tween<float>(0.4f, 2, Easing::kEASE_IN_OUT_CIRC));
	idleScaleAnim->tweens.push_back(new Tween<float>(0.2f, -2, Easing::kEASE_IN_OUT_CIRC));
	idleScaleAnim->hasStart = true;
	idleScaleAnim->startValue = scaleAnim.y;
	idleScaleAnim->loopType = Animation<float>::kLOOP;

	setVisible(false);
}

void MY_DemonSpirit::update(Step * _step){
	idleScaleAnim->update(_step);
	childTransform->scale(scaleAnim, false);

	// accelerate towards the possession target (which is at 0,0,0)
	glm::vec3 a = origin - firstParent()->getTranslationVector();
	float ma = glm::length(a);

	float accelMod = 0.f;
	float damping = 1.f;
	switch(state){
	case kIN:
		accelMod = 0.1f;
		damping = 0.1f;
		// if the spirit is far from the origin, they get ripped out
		if(ma > (firstParent()->getScaleVector().x + possessed->firstParent()->getScaleVector().x) * 2.5f){
			ripIt();
		}
		break;

	case kSTUNNED:
		accelMod = 0.001f;
		a += sweet::NumberUtils::randomVec3(glm::vec3(-5), glm::vec3(5));
		damping = 0.2f;
		break;

	case kOUT:
		accelMod = 0.001f;
		a += sweet::NumberUtils::randomVec3(glm::vec3(-5), glm::vec3(5));
		damping = 0.2f;
		// if the spirit is close to the origin, they can repossess the body
		if(ma < (firstParent()->getScaleVector().x + possessed->firstParent()->getScaleVector().x) * 0.5f){
			getBackInThere();
		}
		break;

	case kDEAD:
		accelMod = 0;
		damping = 1.f;
		stunTimer->stop();
		break;

	default: break;
	}
		
	a *= accelMod;
	speed += a;
	speed *= (1-damping); // damping
	firstParent()->translate(speed);

	float angle = glm::angle(childTransform->getOrientationQuat());
	
	if(state != kDEAD){
		angle *= -0.2f;
		angle += glm::length(speed)*36;
	}else{
		for(auto t : idleScaleAnim->tweens){
			t->deltaValue *= 0.9f;
		}
		idleScaleAnim->startValue *= 0.9f;
		angle -= idleScaleAnim->startValue*360;
		/*if(s > 0){
			if(s < 0.01f){
				s = 0;
			}
			std::cout << s << std::endl;
			childTransform->scale(s*0.8f);
		}*/
	}
	childTransform->rotate(angle, 0, 0, 1, kOBJECT);


	Sprite::update(_step);
}

void MY_DemonSpirit::ripIt(){
	std::cout << "demon ripped" << std::endl;
	state = kSTUNNED;
	int randRipitSound = sweet::NumberUtils::randomInt(1, RIPIT_SOUND_COUNT);
	MY_ResourceManager::globalAssets->getAudio("ripitSound" + std::to_string(randRipitSound))->sound->play();
	stunTimer->restart();
	possessed->state = MY_Demon::kSTUNNED;
	setVisible(true);
}

void MY_DemonSpirit::gripIt(){
	std::cout << "demon gripped" << std::endl;
	state = kSTUNNED;
	int randGripitSound = sweet::NumberUtils::randomInt(1, GRIPIT_SOUND_COUNT);
	MY_ResourceManager::globalAssets->getAudio("GRIPIT_SOUND_" + std::to_string(randGripitSound))->sound->play();
	stunTimer->restart();
	possessed->state = MY_Demon::kSTUNNED;
	setVisible(true);
}

void MY_DemonSpirit::sipIt(){
	std::cout << "demon sipped" << std::endl;
	state = kDEAD;
	int randRipitSound = sweet::NumberUtils::randomInt(1, SIPIT_SOUND_COUNT);
	MY_ResourceManager::globalAssets->getAudio("SIPIT_SOUND_" + std::to_string(randRipitSound))->sound->play();
	possessed->stateTimeout->stop();
	possessed->state = MY_Demon::kDEAD;
	possessed->currentAnimation->frameIndices.loopType = Animation<unsigned long int>::kCONSTANT;
	Timeout * t = new Timeout(0.75f, [this](sweet::Event * _event){
		possessed->setVisible(false);
	});
	possessed->childTransform->addChild(t);
	t->start();
	setVisible(true);
}

void MY_DemonSpirit::getBackInThere(){
	state = kIN;
	setVisible(false);
	possessed->state = MY_Demon::kIDLE;
	possessed->setCurrentAnimation("idle");
}

glm::vec3 MY_DemonSpirit::getGamePos(){
	return firstParent()->getTranslationVector() + possessed->firstParent()->getTranslationVector();
}

MY_Demon::MY_Demon(Shader * _shader, Transform * _target) :
	Sprite(_shader), 
	state(kIDLE),
	spirit(new MY_DemonSpirit(_shader, this)),
	speed(0.02f),
	damage(10.f),
	target(_target),
	scaleAnim(1)
{
	childTransform->addChild(spirit)->translate(spirit->origin);

	int demonTexId = sweet::NumberUtils::randomInt(1, NUM_DEMON_TEXTURES);
	
	spriteSheet = new SpriteSheet(MY_ResourceManager::globalAssets->getTexture("spritesheet_enemy_" + std::to_string(demonTexId))->texture);
	setPrimaryTexture(MY_ResourceManager::globalAssets->getTexture("enemy_" + std::to_string(demonTexId))->texture);
	
	auto anim = new SpriteSheetAnimation(0.4f);
	anim->pushFramesInRange(0, 1, 512, 1024, spriteSheet->texture->width, spriteSheet->texture->height);
	spriteSheet->addAnimation("idle", anim);

	anim = new SpriteSheetAnimation(0.1f);
	anim->pushFramesInRange(2, 3, 512, 1024, spriteSheet->texture->width, spriteSheet->texture->height);
	anim->frameIndices.loopType = Animation<unsigned long int>::kCONSTANT;
	spriteSheet->addAnimation("walk", anim);

	anim = new SpriteSheetAnimation(0.4f);
	anim->pushFramesInRange(4, 6, 512, 1024, spriteSheet->texture->width, spriteSheet->texture->height);
	anim->frameIndices.loopType = Animation<unsigned long int>::kCONSTANT;
	spriteSheet->addAnimation("die", anim);

	anim = new SpriteSheetAnimation(0.4f);
	anim->pushFramesInRange(0, 1, 512, 1024, spriteSheet->texture->width, spriteSheet->texture->height);
	anim->frameIndices.loopType = Animation<unsigned long int>::kCONSTANT;
	spriteSheet->addAnimation("stunned", anim);

	setSpriteSheet(spriteSheet, "idle");

	stateTimeout = new Timeout(TIMEOUT_TIME, [this](sweet::Event * _event){
		bool randState = spirit->state == MY_DemonSpirit::kIN && sweet::NumberUtils::randomBool();
		if(randState) {
			state = kWALK;
		}else {
			state = kIDLE;
		}
		stateTimeout->restart();
	});
	stateTimeout->start();

	eventManager.addEventListener("playerCollision", [this](sweet::Event * _event){
		// Player collision code
	});

	for(auto & v : mesh->vertices){
		v.y += 0.5f;
	}
	mesh->dirty = true;
	mesh->setScaleMode(GL_NEAREST);

	meshTransform->scale(glm::vec3(DEMON_SCALE));

	idleScaleAnim = new Animation<float>(&scaleAnim.y);
	idleScaleAnim->tweens.push_back(new Tween<float>(0.75f, 0.1f, Easing::kEASE_IN_OUT_CIRC));
	idleScaleAnim->tweens.push_back(new Tween<float>(0.65f, -0.1f, Easing::kEASE_IN_OUT_CIRC));
	idleScaleAnim->hasStart = true;
	idleScaleAnim->startValue = scaleAnim.y;
	idleScaleAnim->loopType = Animation<float>::kLOOP;
}

void MY_Demon::render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) {

	Sprite::render(_matrixStack, _renderOptions);
}

void MY_Demon::update(Step * _step) {
	idleScaleAnim->update(_step);
	childTransform->scale(scaleAnim, false);

	eventManager.update(_step);
	stateTimeout->update(_step);
	if(target != nullptr && state == kWALK){
		float targDir = target->getTranslationVector().x < firstParent()->getTranslationVector().x ? -1.f : 1.f;
		childTransform->scale(-targDir, 1, 1, false);
		firstParent()->translate(speed * targDir, 0.f, 0.f);
	}
	
	// animation handling
	switch(state){
		case kWALK: setCurrentAnimation("walk"); break;
		case kIDLE: setCurrentAnimation("idle"); break;
		case kDEAD: setCurrentAnimation("die"); break;
		case kSTUNNED: setCurrentAnimation("stunned"); break;
		default:
			setCurrentAnimation("idle"); break;
	}

	Sprite::update(_step);
}

void MY_Demon::unload() {

	Sprite::unload();
}

void MY_Demon::load() {
	
	Sprite::load();
}

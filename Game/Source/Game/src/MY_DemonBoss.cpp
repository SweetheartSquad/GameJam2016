#pragma once

#include <MY_DemonBoss.h>
#include <NumberUtils.h>
#include <MY_ResourceManager.h>
#include <SpriteSheet.h>
#include <SpriteSheetAnimation.h>

#define DEMON_SCALE 10
#define SPEWER_TIMER_MIN 1
#define SPEWER_TIMER_MAX 3
#define SPEWER_COLUMNS 3

MY_DemonBoss::MY_DemonBoss(Shader* _shader) :
	Sprite(_shader),
	spawnSpewerTimer(0),
	spawnSpewerTimerLength(sweet::NumberUtils::randomFloat(SPEWER_TIMER_MIN, SPEWER_TIMER_MAX))
{
	spriteSheet = new SpriteSheet(MY_ResourceManager::globalAssets->getTexture("spritesheet_enemy_1")->texture);
	setPrimaryTexture(MY_ResourceManager::globalAssets->getTexture("enemy_1")->texture);
	
	auto anim = new SpriteSheetAnimation(0.4f);
	anim->pushFramesInRange(0, 1, 512, 1024, spriteSheet->texture->width, spriteSheet->texture->height);
	spriteSheet->addAnimation("idle", anim);

	anim = new SpriteSheetAnimation(0.4f);
	anim->pushFramesInRange(0, 3, 512, 1024, spriteSheet->texture->width, spriteSheet->texture->height);
	anim->frameIndices.loopType = Animation<unsigned long int>::kCONSTANT;
	spriteSheet->addAnimation("die", anim);

	setSpriteSheet(spriteSheet, "idle");

	for(auto & v : mesh->vertices){
		v.y += 0.5f;
	}
	mesh->dirty = true;
	mesh->setScaleMode(GL_NEAREST);

	meshTransform->scale(glm::vec3(DEMON_SCALE));

	spewerTimeout = new Timeout(2.f, [this](sweet::Event * _event){
		sweet::Event * e = new sweet::Event("spawnSpewer");
		e->setIntData("column", 1);
		eventManager.triggerEvent(e);
	});
	spewerTimeout->start();

	childTransform->addChild(spewerTimeout);
}

void MY_DemonBoss::render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) {
	Sprite::render(_matrixStack, _renderOptions);
}

void MY_DemonBoss::update(Step* _step) {
	Sprite::update(_step);
	eventManager.update(_step);
}

void MY_DemonBoss::unload() {

	Sprite::unload();
}

void MY_DemonBoss::load() {

	Sprite::load();
}

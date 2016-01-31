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
	spawnSpewerTimerLength(sweet::NumberUtils::randomFloat(SPEWER_TIMER_MIN, SPEWER_TIMER_MAX)),
	hits(0)
{
	spriteSheet = new SpriteSheet(MY_ResourceManager::globalAssets->getTexture("spritesheet_boss")->texture);
	setPrimaryTexture(MY_ResourceManager::globalAssets->getTexture("boss")->texture);
	
	auto anim = new SpriteSheetAnimation(0.4f);
	anim->pushFramesInRange(0, 1, 1024, 1024, spriteSheet->texture->width, spriteSheet->texture->height);
	spriteSheet->addAnimation("idle", anim);

	anim = new SpriteSheetAnimation(0.4f);
	anim->pushFramesInRange(2, 3, 1024, 1024, spriteSheet->texture->width, spriteSheet->texture->height);
	spriteSheet->addAnimation("attack", anim);

	setSpriteSheet(spriteSheet, "idle");

	for(auto & v : mesh->vertices){
		v.y += 0.5f;
	}
	mesh->dirty = true;
	mesh->setScaleMode(GL_NEAREST);

	meshTransform->scale(glm::vec3(DEMON_SCALE)*1.5f);

	spewerTimeout = new Timeout(1.f, [this](sweet::Event * _event){
		enableSpewers();
	});

	spewerTimeout->start();
	childTransform->addChild(spewerTimeout);

	/*eventManager.addEventListener("spiritCollision", [this](sweet::Event * _event){
		// Take damage and stuff here
		ST_LOG_INFO("COLLIDE");
	});*/
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

void MY_DemonBoss::addSpewer(MY_Spewer * _spewer){
	spewers.push_back(_spewer);
	spewerIdx.push(spewers.size()-1);

	_spewer->eventManager.addEventListener("spewComplete", [this](sweet::Event * _event){
		disableSpewers(_event->getIntData("column"));
	});
}

void MY_DemonBoss::enableSpewers(){
	if(spewers.size() > 0){
		spewerIdx.shuffle();

		int n = sweet::NumberUtils::randomInt(1, spewers.size());
		for(int i = 0; i < n; ++i){
			int idx = spewerIdx.pop();
			enabledSpewers.push_back(idx);
			spewers.at(idx)->start();
			std::stringstream s;
			s << "ENABLE: " << idx;
			Log::info(s.str());
		}
	}
}

void MY_DemonBoss::disableSpewers(int _column){
	disableSpewer(_column);
	if(enabledSpewers.size() == 0){
		std::stringstream s;
		s << "ALL DISABLED";
		Log::info(s.str());
		spewerTimeout->restart();
	}else{
		std::stringstream s;
		s << "LEFT: " << enabledSpewers.size();
		Log::info(s.str());
	}
}

bool MY_DemonBoss::disableSpewer(int _column){
	for(int i = 0; i < enabledSpewers.size(); ++i){
		if(spewers.at(enabledSpewers.at(i))->column  == _column){
			enabledSpewers.erase(enabledSpewers.begin() + i);
			std::stringstream s;
			s << "DISABLE: " << _column;
			Log::info(s.str());
			return true;
		}
	}
	return false;
}
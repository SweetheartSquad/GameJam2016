#pragma once

#include <MY_Player.h>
#include <MY_ResourceManager.h>

MY_Player::MY_Player(Box2DWorld* _world) :
	Box2DSprite(_world)
{
	setPrimaryTexture(MY_ResourceManager::globalAssets->getTexture("DEFAULT")->texture);
}

void MY_Player::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) {
	
	Box2DSprite::render(_matrixStack, _renderOptions);
}

void MY_Player::update(Step * _step) {

	Box2DSprite::update(_step);
}

void MY_Player::unload() {

	Box2DSprite::unload();
}

void MY_Player::load() {

	Box2DSprite::load();
}

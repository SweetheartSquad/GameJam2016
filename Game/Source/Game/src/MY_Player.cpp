#pragma once

#include <MY_Player.h>
#include <MY_ResourceManager.h>

MY_Player::MY_Player(Shader * _shader) :
	Sprite(_shader)
{
	setPrimaryTexture(MY_ResourceManager::globalAssets->getTexture("DEFAULT")->texture);
}

void MY_Player::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) {
	
	Sprite::render(_matrixStack, _renderOptions);
}

void MY_Player::update(Step * _step) {

	Sprite::update(_step);
}

void MY_Player::unload() {

	Sprite::unload();
}

void MY_Player::load() {

	Sprite::load();
}

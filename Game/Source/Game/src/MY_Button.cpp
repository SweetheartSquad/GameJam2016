#pragma once

#include <MY_Button.h>

#include <sweet/UI.h>
#include <MY_ResourceManager.h>
#include <MeshFactory.h>

MY_Button::MY_Button(BulletWorld * _world, std::string _name) :
	NodeUI(_world, kENTITIES, true)
{
	std::stringstream ss;
	ss << _name;
	texNormal = MY_ResourceManager::globalAssets->getTexture(ss.str())->texture;
	texOver = MY_ResourceManager::globalAssets->getTexture(ss.str() + "Over")->texture;
	texDown = MY_ResourceManager::globalAssets->getTexture(ss.str() + "Down")->texture;

	background->mesh->pushTexture2D(texNormal);

	eventManager.addEventListener("click", [](sweet::Event * _event){
		MY_ResourceManager::globalAssets->getAudio("DEFAULT")->sound->play();
	});
}

void MY_Button::update(Step * _step){
	Texture * texNow = background->mesh->getTexture(0);
	Texture * texNew;
	if(isHovered){
		if(isDown){
			texNew = texDown;
		}else{
			texNew = texOver;
		}
	}else{
		texNew = texNormal;
	}
	if(texNow != texNew){
		invalidateRenderFrame();
		background->mesh->removeTextureAt(0);
		background->mesh->pushTexture2D(texNew);
	}

	NodeUI::update(_step);
}
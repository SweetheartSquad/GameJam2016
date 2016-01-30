#pragma once

#include <My_UI_Counter.h>

MY_UI_Counter::MY_UI_Counter(BulletWorld * _bulletWorld, Texture * _itemTexture, Texture * _itemTextureAlt) :
	HorizontalLinearLayout(_bulletWorld),
	bulletWorld(_bulletWorld),
	itemTexture(_itemTexture),
	itemTextureAlt(_itemTextureAlt)
{
}

MY_UI_Counter::~MY_UI_Counter(){
}

int MY_UI_Counter::getItemCount(){
	return items.size();
}

void MY_UI_Counter::setItemCount(unsigned int _count){
	while(items.size() > _count){
		decrement();
	}
	while(items.size() < _count){
		increment();
	}
}

void MY_UI_Counter::increment(){
	if(lostItems.size() > 0){
		items.push_back(lostItems.back());
		lostItems.pop_back();
		items.back()->setVisible(true);
		items.back()->background->mesh->replaceTextures(itemTexture);
	}else{
		NodeUI * item = new NodeUI(bulletWorld);
		item->background->mesh->pushTexture2D(itemTexture);
		item->setWidth(getHeight());
		item->setHeight(getHeight());
		items.push_back(item);
		addChild(item); // insert at index? But not really for this game
	}
}

void MY_UI_Counter::decrement(){
	lostItems.push_back(items.back());
	if(itemTextureAlt != nullptr){
		items.back()->background->mesh->replaceTextures(itemTextureAlt);
	}else{
		items.back()->setVisible(false); // or remove, or use transparent texture
	}
	items.pop_back();
}
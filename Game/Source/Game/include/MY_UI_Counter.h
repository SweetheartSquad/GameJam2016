#pragma once

#include <sweet\UI.h>

class MY_UI_Counter : public HorizontalLinearLayout {
private:
	BulletWorld * bulletWorld;

	std::vector<NodeUI *> items;
	std::vector<NodeUI *> lostItems;

	Texture * itemTexture;
	Texture * itemTextureAlt;

public:
	MY_UI_Counter(BulletWorld * _world, Texture * _itemTexture, Texture * _itemTextureAlt = nullptr);
	~MY_UI_Counter();

	int getItemCount();
	void setItemCount(unsigned int _count);
	void increment();
	void decrement();

};
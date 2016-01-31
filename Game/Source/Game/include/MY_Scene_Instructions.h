#pragma once

#include <MY_Scene_MenuBase.h>

class MY_Scene_Instructions : public MY_Scene_MenuBase{
public:
	MY_Scene_Instructions(Game * _game);
	Timeout * t;

	virtual void update(Step * _step) override;
};
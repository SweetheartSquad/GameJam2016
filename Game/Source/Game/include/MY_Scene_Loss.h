#pragma once

#include <MY_Scene_Base.h>

class MY_Scene_Loss : public MY_Scene_Base{
public:
	MY_Scene_Loss(Game * _game);

	virtual void update(Step * _step) override;
};
#pragma once

#include <MY_Scene_MenuBase.h>

class MY_Scene_Finale : public MY_Scene_MenuBase{
public:
	MY_Scene_Finale(Game * _game);

	virtual void update(Step * _step) override;
};
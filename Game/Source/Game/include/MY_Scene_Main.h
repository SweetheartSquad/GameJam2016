#pragma once

#include <Scene.h>

#include <Box2DWorld.h>
#include <Box2DSprite.h>
#include <Box2DMeshEntity.h>
#include "MY_Scene_Base.h"
#include "MY_Player.h"
#include "MY_Demon.h"

class MY_Scene_Main : public MY_Scene_Base{
public:

	MY_Player * player;

	MY_Demon * testDemon;

	explicit MY_Scene_Main(Game * _game);
	~MY_Scene_Main();

	virtual void update(Step * _step) override;
	
};

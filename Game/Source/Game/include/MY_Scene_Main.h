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

	std::vector<MY_Demon *> demons;
	MY_Demon * spawnDemon();

	PerspectiveCamera * mainCam;

	explicit MY_Scene_Main(Game * _game);
	~MY_Scene_Main();

	virtual void update(Step * _step) override;
	
	// overriden to add physics debug drawing
	virtual void enableDebug() override;
	// overriden to remove physics debug drawing
	virtual void disableDebug() override;

	Sprite * mouseIndicator;

	MY_DemonSpirit * getHovered();
	void collideEntities();
	bool isHoveredOverPossessed();
	bool isHoveredOverSpirit();

	void gripIt();
	void ripIt();
	void sipIt();

	float hoverRadius;
	float hoverRadius2;

	glm::vec2 distToHoverTarget;
	float distToHoverTargetMag;
	MY_DemonSpirit * hoverTarget;
	MY_DemonSpirit * ripTarget;
	MY_DemonSpirit * gripTarget;
};

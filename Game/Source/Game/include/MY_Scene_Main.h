#pragma once

#include <Scene.h>

#include <Box2DWorld.h>
#include <Box2DSprite.h>
#include <Box2DMeshEntity.h>
#include "MY_Scene_Base.h"
#include "MY_Player.h"
#include "MY_Demon.h"
#include <MY_Spewer.h>
#include "MY_Cam.h"
#include <MY_UI_Counter.h>

#include <MY_Scene_ScreenShaders.h>
#include <RenderSurface.h>
#include <StandardFrameBuffer.h>

class MY_DemonBoss;
class Room;
class MY_Game;

// number of demons that have to be captured to fight boss, also boss's health
#define MAX_DEMON_COUNT 2
// maximum number of demons per room (overriden by total maximum; i.e. if you're only one from the boss, it won't spawn two enemies)
#define MAX_SPAWNED_DEMON_COUNT 3

class MY_Scene_Main : public MY_Scene_Base{
public:
	Shader * screenSurfaceShader;
	RenderSurface * screenSurface;
	StandardFrameBuffer * screenFBO;

	float screenMagnitude;


	bool gameOver;
	bool started;
	bool isBossRoom;
	ComponentShaderBase * baseShaderWithDepth;

	MY_Player * player;
	
	Room * currentRoom;
	Room * previousRoom;

	std::vector<MY_Demon *> demons;
	MY_Demon * spawnDemon(Room * _room);
	MY_DemonBoss * spawnBoss(Room * _room);
	MY_Spewer * spawnSpewer(Room * _room, MY_DemonBoss * _boss, int _column);
	MY_Player * spawnPlayer(Room * _room);

	MY_Cam * mainCam;

	MY_UI_Counter * livesCounter;
	MY_UI_Counter * demonsCounter;

	// Boss room stuff
	MY_Demon * dummyDemon;
	MY_DemonBoss * boss;

	explicit MY_Scene_Main(MY_Game * _game);
	~MY_Scene_Main();

	virtual void update(Step * _step) override;
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
	
	// overriden to add physics debug drawing
	virtual void enableDebug() override;
	// overriden to remove physics debug drawing
	virtual void disableDebug() override;

	Sprite * mouseIndicator;

	MY_DemonSpirit * getHovered();
	void calcHover(MY_DemonSpirit * _demon);
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

	void addDummyDemon(Room * _room);

	Room * goToNewRoom();

	Timeout * roomTransition;
};

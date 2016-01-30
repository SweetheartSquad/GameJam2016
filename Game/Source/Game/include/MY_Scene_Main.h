#pragma once

#include <Scene.h>

#include <Box2DWorld.h>
#include <Box2DSprite.h>
#include <Box2DMeshEntity.h>
#include "MY_Scene_Base.h"

class MY_Scene_Main : public MY_Scene_Base{
public:

	// The scene's physics world
	Box2DWorld * box2dWorld;
	// used to draw wireframes showing physics colliders, transforms, etc
	Box2DDebugDrawer * box2dDebugDrawer;

	// A Box2DMeshSprite is a class which combines a Box2D physics node and a standard Sprite.
	// The physics node alters the position/orientation of the Sprite's childTransform
	// during the update loop to match the physics body's position/orientation within the Box2D world
	//
	// In this scene, we are using a Box2DSprite to represent a player object
	// In order to control the player object, we will be treating it as a rigid body and applying forces/impulses
	Box2DSprite * player;
	// Similar to the Box2DMeshSprite, Box2DMeshEntity is a class which combines a Box2D physics node and a standard MeshEntity
	//
	// In this scene, we are using a Box2DMeshEntity to represent a ground object
	// This object will be a static body
	Box2DMeshEntity * ground;


	PerspectiveCamera * mainCam;

	explicit MY_Scene_Main(Game * _game);
	~MY_Scene_Main();

	virtual void update(Step * _step) override;
	
	// overriden to add physics debug drawing
	virtual void enableDebug() override;
	// overriden to remove physics debug drawing
	virtual void disableDebug() override;


	Sprite * demonTest;
	Sprite * mouseIndicator;


	Sprite * getHovered();
	bool isHoveredOverPossessed();
	bool isHoveredOverSpirit();

	void gripIt();
	void ripIt();
	void sipIt();

	float hoverRadius;
	float hoverRadius2;

	glm::vec2 distToHoverTarget;
	float distToHoverTargetMag;
	Sprite * hoverTarget;
	Sprite * gripTarget;
};
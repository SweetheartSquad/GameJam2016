#pragma once

#include <MY_Scene_Box2D.h>

#include <Box2DWorld.h>
#include <Box2DDebugDrawer.h>
#include <Box2DMeshEntity.h>
#include <Box2DSprite.h>
#include "MY_Player.h"
#include <MeshFactory.h>

MY_Scene_Box2D::MY_Scene_Box2D(Game * _game) :
	MY_Scene_Base(_game),
	box2dWorld(new Box2DWorld(b2Vec2(0.f, -10.0f))),
	box2dDebugDrawer(new Box2DDebugDrawer(box2dWorld))
{
	// Setup the debug drawer and add it to the scene
	childTransform->addChild(box2dDebugDrawer, false);
	box2dDebugDrawer->drawing = false;
	box2dWorld->b2world->SetDebugDraw(box2dDebugDrawer);
	box2dDebugDrawer->AppendFlags(b2Draw::e_shapeBit);
	box2dDebugDrawer->AppendFlags(b2Draw::e_centerOfMassBit);
	box2dDebugDrawer->AppendFlags(b2Draw::e_jointBit);

	// Setup the ground
	ground = new Box2DMeshEntity(box2dWorld, MeshFactory::getCubeMesh(5), b2_staticBody, baseShader);
	ground->createFixture(); // when we're using a Box2DMeshEntity, createFixture will make a collider which matches the bounding box (or bounding circle, if you use the argument)
	childTransform->addChild(ground);
}

MY_Scene_Box2D::~MY_Scene_Box2D(){
	// we need to destruct the scene elements before the physics world to avoid memory issues
	deleteChildTransform();
	delete box2dWorld;
}


void MY_Scene_Box2D::update(Step * _step){
	// Physics update
	box2dWorld->update(_step);
	// Scene update
	MY_Scene_Base::update(_step);

	// player input
	player->applyLinearImpulseRight(controller->getAxis(controller->axisLeftX));
	if(controller->buttonJustDown(controller->faceButtonDown)){
		player->applyLinearImpulseUp(5);
	}
}

void MY_Scene_Box2D::enableDebug(){
	MY_Scene_Base::enableDebug();
	box2dDebugDrawer->drawing = true;
	childTransform->addChildAtIndex(box2dDebugDrawer, -1, false); // make sure the debug drawer is the last thing drawn
}
void MY_Scene_Box2D::disableDebug(){
	MY_Scene_Base::disableDebug();
	box2dDebugDrawer->drawing = false;
}
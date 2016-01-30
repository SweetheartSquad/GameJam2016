#pragma once

#include <MY_Scene_Main.h>
#include <Box2DDebugDrawer.h>
#include <MeshFactory.h>
#include <MY_Player.h>

MY_Scene_Main::MY_Scene_Main(Game * _game) :
	MY_Scene_Base(_game)
{
	// Setup the player
	player = new MY_Player(baseShader);
	childTransform->addChild(player);

	testDemon = new MY_Demon(baseShader, player->firstParent());
	childTransform->addChild(testDemon);
	testDemon->firstParent()->translate(5.0f, 0.f, 0.f);
}

MY_Scene_Main::~MY_Scene_Main(){
	// we need to destruct the scene elements before the physics world to avoid memory issues
	deleteChildTransform();
}


void MY_Scene_Main::update(Step * _step){
	// Scene update
	MY_Scene_Base::update(_step);
}

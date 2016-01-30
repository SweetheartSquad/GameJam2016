#pragma once

#include <MY_Scene_Main.h>
#include <Box2DDebugDrawer.h>
#include <MeshFactory.h>

MY_Scene_Main::MY_Scene_Main(Game * _game) :
	MY_Scene_Base(_game),
	box2dWorld(new Box2DWorld(b2Vec2(0.f, -10.0f))),
	box2dDebugDrawer(new Box2DDebugDrawer(box2dWorld)),
	mainCam(new PerspectiveCamera()),
	demonTest(new Sprite(MY_ResourceManager::globalAssets->getTexture("DEFAULT")->texture, baseShader)),
	hoverRadius(15),
	hoverRadius2(hoverRadius*hoverRadius),
	hoverTarget(nullptr),
	gripTarget(nullptr)
{
	// setup main camera
	cameras.push_back(mainCam);
	childTransform->addChild(mainCam);
	activeCamera = mainCam;
	//mainCam->childTransform->addChild(new CameraController(mainCam));
	mainCam->farClip = 1000.f;
	mainCam->firstParent()->translate(0.0f, 0.5f, 20.f);
	mainCam->yaw = 90.0f;
	mainCam->pitch = 0.0f;


	// setup demon test
	childTransform->addChild(demonTest);

	mouseIndicator = uiLayer->addMouseIndicator();


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

	// Setup the player
	player = new Box2DSprite(box2dWorld, b2_dynamicBody, baseShader, MY_ResourceManager::globalAssets->getTexture("DEFAULT")->texture, 1, 1);
	player->createFixture(); // when we're using a Box2DSprite, createFixture will make a collider which matches the provided width and height of the sprite (note that this is different from the actual texture size)
	childTransform->addChild(player);

	// when dealing with physics nodes, we use translatePhysical instead of editing the Transform nodes directly
	// this is because we need to inform the physics simulation of the change, not our Transform hierarchy
	// the physics node will handle the placement of its childTransform automatically later during the update loop
	player->translatePhysical(glm::vec3(0, 6, 0), false); 
}

MY_Scene_Main::~MY_Scene_Main(){
	// we need to destruct the scene elements before the physics world to avoid memory issues
	deleteChildTransform();
	delete box2dWorld;
}


void MY_Scene_Main::update(Step * _step){
	// Physics update
	box2dWorld->update(_step);
	// Scene update
	MY_Scene_Base::update(_step);

	// player input
	player->applyLinearImpulseRight(controller->getAxis(controller->axisLeftX));
	if(controller->buttonJustDown(controller->faceButtonDown)){
		player->applyLinearImpulseUp(5);
	}

	hoverTarget = getHovered();
	if(hoverTarget == nullptr){
		if(mouse->leftJustPressed()){
			gripTarget = hoverTarget;
		}
	}

	// if we're holding a demon
	if(gripTarget != nullptr){
		gripIt();
	}
}

void MY_Scene_Main::enableDebug(){
	MY_Scene_Base::enableDebug();
	box2dDebugDrawer->drawing = true;
	childTransform->addChildAtIndex(box2dDebugDrawer, -1, false); // make sure the debug drawer is the last thing drawn
}
void MY_Scene_Main::disableDebug(){
	MY_Scene_Base::disableDebug();
	box2dDebugDrawer->drawing = false;
}

Sprite * MY_Scene_Main::getHovered(){
	std::cout << "Mouse: " << mouse->mouseX() << " " << mouse->mouseY() << std::endl;
	glm::vec3 demonPos = demonTest->getWorldPos();
	glm::vec3 demonPosInScreen = mainCam->worldToScreen(demonPos, sweet::getWindowDimensions());
	std::cout << "Mouse: " << mouse->mouseX() << " " << mouse->mouseY() << std::endl;
	std::cout << "Demon: " << demonPosInScreen.x << " " << demonPosInScreen.y << " " << demonPosInScreen.z << std::endl;
	distToHoverTarget = glm::vec2(demonPosInScreen.x, demonPosInScreen.y) - glm::vec2(mouse->mouseX(), mouse->mouseY());
	std::cout << "Dist: " << distToHoverTarget.x << " " << distToHoverTarget.y << std::endl;
	distToHoverTargetMag = glm::length(distToHoverTarget);
	if(distToHoverTargetMag < hoverRadius){
		// return the found hoverTarget
		return demonTest;
	}
	// we didn't find a hoverTarget
	return nullptr;
}

bool MY_Scene_Main::isHoveredOverPossessed(){
	return false;
}

bool MY_Scene_Main::isHoveredOverSpirit(){
	return false;
}


void MY_Scene_Main::ripIt(){

}

void MY_Scene_Main::gripIt(){
	// demon pulls the mouse, mouse pulls the demon
	if(distToHoverTargetMag > 3){
		mouse->translate(distToHoverTarget*-0.5f);
		demonTest->firstParent()->translate(glm::vec3(distToHoverTarget.x, distToHoverTarget.y, 0)*0.5f);
	}
}

void MY_Scene_Main::sipIt(){

}
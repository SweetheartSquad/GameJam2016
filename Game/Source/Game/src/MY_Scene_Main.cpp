#pragma once

#include <MY_Scene_Main.h>
#include <Box2DDebugDrawer.h>
#include <MeshFactory.h>
#include <MY_Player.h>

MY_Scene_Main::MY_Scene_Main(Game * _game) :
	MY_Scene_Base(_game),
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

	// Setup the player
	player = new MY_Player(baseShader);
	childTransform->addChild(player);

	testDemon = new MY_Demon(baseShader, player->firstParent());
	childTransform->addChild(testDemon);
	testDemon->firstParent()->translate(5.0f, 0.f, 0.f);
	demons.push_back(testDemon);
}

MY_Scene_Main::~MY_Scene_Main(){
	// we need to destruct the scene elements before the physics world to avoid memory issues
	deleteChildTransform();
}


void MY_Scene_Main::update(Step * _step){
	// Scene update
	MY_Scene_Base::update(_step);

	collideEntities();

	hoverTarget = getHovered();
	if(hoverTarget != nullptr){
		if(mouse->leftJustPressed()){
			gripTarget = hoverTarget;
		}
	}

	if(mouse->leftJustReleased()){
		gripTarget = nullptr;
	}

	// if we're holding a demon
	if(gripTarget != nullptr){
		gripIt();
	}
}

void MY_Scene_Main::enableDebug(){
	MY_Scene_Base::enableDebug();
}
void MY_Scene_Main::disableDebug(){
	MY_Scene_Base::disableDebug();
}

void MY_Scene_Main::collideEntities() {
	auto  ptrans = player->firstParent()->getTranslationVector();
	float pMin = ptrans.x - (player->firstParent()->getScaleVector().x  * 0.5f);
	float pMax = ptrans.x + (player->firstParent()->getScaleVector().x  * 0.5f);
	
	for(auto demon : demons) {
		auto dtrans = demon->firstParent()->getTranslationVector();
		float dMin = dtrans.x - (demon->firstParent()->getScaleVector().x  * 0.5f);
		float dMax = dtrans.x + (demon->firstParent()->getScaleVector().x  * 0.5f);

		if((pMax >= dMin && pMin <= dMax) ||
			pMin <= dMax && pMax >= dMin) {
				player->eventManager.triggerEvent("demonCollision");
				demon->eventManager.triggerEvent("playerCollision");
		}
	}
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
	std::cout << "DistMag: " << distToHoverTargetMag << std::endl;
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
		float mouseResistance = 0.8;
		float demonResistance = 0.001f;
		mouse->translate(distToHoverTarget*mouseResistance);
		demonTest->firstParent()->translate(glm::vec3(distToHoverTarget.x, distToHoverTarget.y, 0)*-demonResistance);
	}
}

void MY_Scene_Main::sipIt(){

}

#pragma once

#include <MY_Scene_Main.h>
#include <Box2DDebugDrawer.h>
#include <MeshFactory.h>
#include <MY_Player.h>
#include <Room.h>

MY_Scene_Main::MY_Scene_Main(Game * _game) :
	MY_Scene_Base(_game),
	mainCam(new PerspectiveCamera()),
	hoverRadius(15),
	hoverRadius2(hoverRadius*hoverRadius),
	hoverTarget(nullptr),
	ripTarget(nullptr),
	gripTarget(nullptr)
{
	// setup main camera
	cameras.push_back(mainCam);
	childTransform->addChild(mainCam);
	activeCamera = mainCam;
	//mainCam->childTransform->addChild(new CameraController(mainCam));
	mainCam->farClip = 1000.f;
	mainCam->firstParent()->translate(0.0f, 0.5f, 5.f);
	mainCam->yaw = 90.0f;
	mainCam->pitch = 0.0f;

	mouseIndicator = uiLayer->addMouseIndicator();

	// Setup the player
	player = new MY_Player(baseShader);
	childTransform->addChild(player);

	room = new Room(baseShader, 50.f, 10.f, 10.f);

	childTransform->addChild(room);

	spawnDemon();

	spawnDemon();
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
			if(hoverTarget->state == kIN){
				ripTarget = hoverTarget;
			}else if(hoverTarget->state == kSTUNNED || hoverTarget->state == kOUT){
				gripTarget = hoverTarget;
			}
		}
	}

	// let go of any held demons
	if(mouse->leftJustReleased()){
		ripTarget = nullptr;
		gripTarget = nullptr;
	}
	
	// if we're holding a demon inside a body, try to rip it
	if(ripTarget != nullptr){
		if(ripTarget->state == kIN){
			ripIt();
		}else{
			// we've ripped it, so let go
			ripTarget = nullptr;
		}
	}
	// if we're holding a demon outside a body, try to grip it
	if(gripTarget != nullptr){
		gripIt();
		sipIt();
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
				sweet::Event * e = new sweet::Event("demonCollision");
				e->setFloatData("damage", demon->damage);
				player->eventManager.triggerEvent(e);

				demon->eventManager.triggerEvent("playerCollisoin");
		}
	}
}

MY_Demon * MY_Scene_Main::spawnDemon(){
	MY_Demon * d = new MY_Demon(baseShader, player->firstParent());
	childTransform->addChild(d)->translate(5.0f, 0.f, 0.f);
	demons.push_back(d);
	return d;
}

MY_DemonSpirit * MY_Scene_Main::getHovered(){
	for(auto d : demons){
		glm::vec3 demonPos = d->spirit->getWorldPos();
		glm::vec3 demonPosInScreen = mainCam->worldToScreen(demonPos, sweet::getWindowDimensions());
		distToHoverTarget = glm::vec2(demonPosInScreen.x, demonPosInScreen.y) - glm::vec2(mouse->mouseX(), mouse->mouseY());
		distToHoverTargetMag = glm::length(distToHoverTarget);

		/*std::cout << "Mouse: " << mouse->mouseX() << " " << mouse->mouseY() << std::endl;
		std::cout << "Demon: " << demonPosInScreen.x << " " << demonPosInScreen.y << " " << demonPosInScreen.z << std::endl;
		std::cout << "Dist: " << distToHoverTarget.x << " " << distToHoverTarget.y << std::endl;
		std::cout << "DistMag: " << distToHoverTargetMag << std::endl;*/

		if(distToHoverTargetMag < hoverRadius){
			// return the found hoverTarget
			return d->spirit;
		}
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
	// demon pulls the mouse, mouse pulls the demon
	if(distToHoverTargetMag > 3){
		float mouseResistance = 0.5f;
		float demonResistance = 0.01f;
		mouse->translate(distToHoverTarget*mouseResistance);
		ripTarget->firstParent()->translate(glm::vec3(distToHoverTarget.x, distToHoverTarget.y, 0)*-demonResistance);
	}
}

void MY_Scene_Main::gripIt(){
	// demon pulls the mouse, mouse pulls the demon
	if(distToHoverTargetMag > 3){
		float mouseResistance = 0.5f;
		float demonResistance = 0.001f;
		mouse->translate(distToHoverTarget*mouseResistance);
		gripTarget->firstParent()->translate(glm::vec3(distToHoverTarget.x, distToHoverTarget.y, 0)*-demonResistance);
		gripTarget->gripIt();
	}
}

void MY_Scene_Main::sipIt(){
	// if the demon gets close enough to the player, they get sipped
	glm::vec3 demPos = gripTarget->getGamePos();
	glm::vec3 playerPos = player->firstParent()->getTranslationVector();

	if(glm::distance(demPos, playerPos) < player->firstParent()->getScaleVector().x  * 0.5f){
		gripTarget->sipIt();
		gripTarget = nullptr;
	}
}

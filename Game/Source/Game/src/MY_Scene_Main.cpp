#pragma once

#include <MY_Scene_Main.h>
#include <Box2DDebugDrawer.h>
#include <MeshFactory.h>
#include <MY_Player.h>
#include <MY_Game.h>
#include <Room.h>

#include <shader\ComponentShaderBase.h>
#include <shader\ComponentShaderText.h>
#include <shader\ShaderComponentText.h>
#include <shader\ShaderComponentTexture.h>
#include <shader\ShaderComponentDepthOffset.h>
#include <shader\ShaderComponentDiffuse.h>
#include <shader\ShaderComponentMVP.h>

#include <NumberUtils.h>

MY_Scene_Main::MY_Scene_Main(MY_Game * _game) :
	MY_Scene_Base(_game),
	mainCam(new MY_Cam()),
	hoverRadius(50),
	hoverRadius2(hoverRadius*hoverRadius),
	hoverTarget(nullptr),
	ripTarget(nullptr),
	gripTarget(nullptr),
	gameOver(false),
	baseShaderWithDepth(new ComponentShaderBase(true)),
	currentRoom(nullptr),
	previousRoom(nullptr),
	started(false)
{
	
	baseShaderWithDepth->addComponent(new ShaderComponentMVP(baseShaderWithDepth));
	//baseShader->addComponent(new ShaderComponentDiffuse(baseShader));
	baseShaderWithDepth->addComponent(new ShaderComponentTexture(baseShaderWithDepth, 0.01f));
	baseShaderWithDepth->addComponent(new ShaderComponentDepthOffset(baseShaderWithDepth));
	baseShaderWithDepth->compileShader();


	// setup main camera
	cameras.push_back(mainCam);
	childTransform->addChild(mainCam);
	activeCamera = mainCam;
	//mainCam->childTransform->addChild(new CameraController(mainCam));
	mainCam->farClip = 1000.f;
	mainCam->firstParent()->translate(0.f, 5, 20);
	mainCam->yaw = 90.0f;
	mainCam->pitch = 0;
	mainCam->fieldOfView = 70;

	mouseIndicator = uiLayer->addMouseIndicator();


	// setup room transition
	roomTransition = new Timeout(1.f, [this](sweet::Event * _event){
		if(previousRoom != nullptr){
			childTransform->removeChild(previousRoom->firstParent());
			delete previousRoom->firstParent();
			previousRoom = nullptr;
		}
		// TODO: make the current room active
	});
	roomTransition->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float t = Easing::easeOutCirc(_event->getFloatData("progress"), 0, 1, 1.f);
		if(previousRoom != nullptr){
			previousRoom->firstParent()->translate(glm::vec3(-t-0.05f,0,0)*previousRoom->roomWidth, false);
		}
		currentRoom->firstParent()->translate(glm::vec3(1-t,0,0)*currentRoom->roomWidth, false);
	});

	childTransform->addChild(roomTransition, false);
	


	// Setup a room
	goToNewRoom();

	// setup UI
	livesCounter = new MY_UI_Counter(uiLayer->world, MY_ResourceManager::globalAssets->getTexture("DEFAULT")->texture, MY_ResourceManager::globalAssets->getTexture("demon_1")->texture);
	livesCounter->setBackgroundColour(1.f, 0.f, 0.f, 0.5f);
	livesCounter->setRationalWidth(1.f);
	livesCounter->setRationalHeight(1.f);
	livesCounter->setMarginRight(0.5f);
	livesCounter->setMarginBottom(0.9f);
	livesCounter->horizontalAlignment = kLEFT;

	demonsCounter = new MY_UI_Counter(uiLayer->world, MY_ResourceManager::globalAssets->getTexture("demon_spirit")->texture);
	demonsCounter->setBackgroundColour(0.f, 0.f, 1.f, 0.5f);
	demonsCounter->setRationalWidth(1.f);
	demonsCounter->setRationalHeight(1.f);
	demonsCounter->setMarginLeft(0.5f);
	demonsCounter->setMarginBottom(0.9f);
	demonsCounter->horizontalAlignment = kRIGHT;

	uiLayer->addChild(livesCounter);
	uiLayer->addChild(demonsCounter);
}

MY_Scene_Main::~MY_Scene_Main(){
	// we need to destruct the scene elements before the physics world to avoid memory issues
	deleteChildTransform();
}


Room * MY_Scene_Main::goToNewRoom(){
	previousRoom = currentRoom;

	// clear out old stuff
	if(previousRoom != nullptr){
		player->eventManager.listeners.clear();
		player->joystick = nullptr;

		demons.clear();
	}

	Room * res = currentRoom = new Room(baseShader);

	childTransform->addChild(res);
	
	res->placeBG();
	res->placeGG();

	
	player = spawnPlayer(res);
	spawnDemon(res);
	spawnDemon(res);

	currentRoom->placeFG();
	roomTransition->restart();

	
	// Setup the player and game events
	player->eventManager.addEventListener("tookDamage", [this](sweet::Event * _event){
		mainCam->shakeTimer->restart();
		livesCounter->decrement();
	});
	player->eventManager.addEventListener("invincibilityStart", [this](sweet::Event * _event){
		mainCam->shakeTimer->restart();
	});
	player->eventManager.addEventListener("gameOver", [this](sweet::Event * _event){
		gameOver = true;
		// gameOver stuff
		Log::info("GAME OVER");

		std::stringstream ss;
		ss << "game_" << glfwGetTime();
		game->scenes[ss.str()] = new MY_Scene_Main(dynamic_cast<MY_Game *>(game));
		game->switchScene(ss.str(), true);

		MY_Player::lives = MAX_LIVES;
	});
	player->eventManager.addEventListener("newroom", [this](sweet::Event * _event){
		goToNewRoom();
	});


	return res;
}

void MY_Scene_Main::update(Step * _step){
	// Scene update
	MY_Scene_Base::update(_step);

	if(keyboard->keyJustDown(GLFW_KEY_ESCAPE)){
		game->switchScene("menu", false);
		game->scenes["game"] = this;
	}

	if(!started){ // Because we don't have the UI square thing
		started = true;
		livesCounter->setItemCount(5);
	}
	// camera
	//mainCam->firstParent()->translate(glm::vec3(sin(_step->time), cos(_step->time), 0));
	
	if(keyboard->keyJustDown(GLFW_KEY_R)){
		player->eventManager.triggerEvent("gameOver");
	}
	if(keyboard->keyJustDown(GLFW_KEY_N)){
		if(roomTransition->complete){
			player->eventManager.triggerEvent("newroom");
		}
	}


	collideEntities();

	// Check enemy count
	if(demons.size() == 0){
		// unlock door
		currentRoom->unlock();
	}

	if(currentRoom->unlocked && player->firstParent()->getTranslationVector().x >= currentRoom->doorPos){
		// go to next room
		Log::info("go to next room");
		player->eventManager.triggerEvent("newroom");
	}

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

MY_Demon * MY_Scene_Main::spawnDemon(Room * _room){
	MY_Demon * d = new MY_Demon(baseShaderWithDepth, player->firstParent());
	_room->gameground->addChild(d)->translate(glm::vec3(sweet::NumberUtils::randomFloat(0, _room->doorPos*0.9f), 0, 0), false);
	demons.push_back(d);
	return d;
}

MY_Player * MY_Scene_Main::spawnPlayer(Room * _room){
	// Setup the player
	MY_Player * p = new MY_Player(baseShaderWithDepth);
	_room->gameground->addChild(p)->scale(10)->translate(glm::vec3(-_room->doorPos*0.9f, 0, 0), false);
	return p;
}

MY_DemonSpirit * MY_Scene_Main::getHovered(){
	for(auto d : demons){
		glm::vec3 demonPos = d->spirit->meshTransform->getWorldPos();
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
	if(distToHoverTargetMag > glm::length(ripTarget->scaleAnim)){
		float mouseResistance = 1.f;
		float demonResistance = 0.01f;
		mouse->translate(distToHoverTarget*mouseResistance);
		ripTarget->firstParent()->translate(glm::vec3(distToHoverTarget.x, distToHoverTarget.y, 0)*-demonResistance);
	}
}

void MY_Scene_Main::gripIt(){
	// demon pulls the mouse, mouse pulls the demon
	if(distToHoverTargetMag > glm::length(gripTarget->scaleAnim)){
		float mouseResistance = 1.f;
		float demonResistance = 0.01f;
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
		demonsCounter->increment();
		gripTarget = nullptr;
	}
}

#pragma once

#include <MY_Scene_Main.h>
#include <Box2DDebugDrawer.h>
#include <MeshFactory.h>
#include <MY_Player.h>
#include <MY_Game.h>
#include <Room.h>
#include <BossRoom.h>
#include <RenderOptions.h>

#include <shader\ComponentShaderBase.h>
#include <shader\ComponentShaderText.h>
#include <shader\ShaderComponentText.h>
#include <shader\ShaderComponentTexture.h>
#include <shader\ShaderComponentDepthOffset.h>
#include <shader\ShaderComponentDiffuse.h>
#include <shader\ShaderComponentMVP.h>

#include <NumberUtils.h>
#include <MY_DemonBoss.h>

MY_Scene_Main::MY_Scene_Main(MY_Game * _game) :
	MY_Scene_Base(_game),
	gameOver(false),
	started(false),
	isBossRoom(false),
	baseShaderWithDepth(new ComponentShaderBase(true)),
	currentRoom(nullptr),
	previousRoom(nullptr),
	mainCam(new MY_Cam()),
	hoverRadius(50),
	hoverRadius2(hoverRadius*hoverRadius),
	hoverTarget(nullptr),
	ripTarget(nullptr),
	gripTarget(nullptr),
	boss(nullptr),
	dummyDemon(nullptr),
	screenSurfaceShader(new Shader("assets/RenderSurface_1", false, true)),
	screenSurface(new RenderSurface(screenSurfaceShader)),
	screenFBO(new StandardFrameBuffer(true)),
	screenMagnitude(0)

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
	mainCam->firstParent()->translate(0.f, 7.5, 21);
	mainCam->yaw = 90.0f;
	mainCam->pitch = 0;
	mainCam->fieldOfView = 70;
	mainCam->interpolation = 0.1f;

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
		mainCam->yaw = t*90;
	});

	childTransform->addChild(roomTransition, false);
	
	// setup UI
	livesCounter = new MY_UI_Counter(uiLayer->world, MY_ResourceManager::globalAssets->getTexture("LIFE")->texture, MY_ResourceManager::globalAssets->getTexture("EMPTY_LIFE")->texture);
	livesCounter->setBackgroundColour(1.f, 0.f, 0.f, 0.f);
	livesCounter->setRationalWidth(1.f);
	livesCounter->setRationalHeight(1.f);
	livesCounter->setMarginRight(0.5f);
	livesCounter->setMarginBottom(0.9f);
	livesCounter->horizontalAlignment = kLEFT;

	demonsCounter = new MY_UI_Counter(uiLayer->world, MY_ResourceManager::globalAssets->getTexture("demon_spirit")->texture);
	demonsCounter->setBackgroundColour(0.f, 0.f, 1.f, 0.f);
	demonsCounter->setRationalWidth(1.f);
	demonsCounter->setRationalHeight(1.f);
	demonsCounter->setMarginLeft(0.1f);
	demonsCounter->setMarginRight(0.1f);
	
	float marginTop = 1 - (sweet::getWindowWidth() * 1.f/MAX_DEMON_COUNT * 0.8f) / sweet::getWindowHeight();
	demonsCounter->setMarginTop(marginTop);
	demonsCounter->horizontalAlignment = kRIGHT;

	uiLayer->addChild(livesCounter);
	uiLayer->addChild(demonsCounter);

	// Setup a room
	goToNewRoom();
	

	// memory management
	++screenSurface->referenceCount;
	++screenSurfaceShader->referenceCount;
	++screenFBO->referenceCount;
}

MY_Scene_Main::~MY_Scene_Main(){
	// we need to destruct the scene elements before the physics world to avoid memory issues
	deleteChildTransform();

	// memory management
	screenSurface->decrementAndDelete();
	screenSurfaceShader->decrementAndDelete();
	screenFBO->decrementAndDelete();
}


void MY_Scene_Main::addDummyDemon(Room * _room) {
	dummyDemon = new MY_Demon(baseShaderWithDepth, _room->gameground);
	_room->gameground->addChild(dummyDemon)->translate(glm::vec3(-_room->doorPos*0.9f, 0, 0), false);
	demons.push_back(dummyDemon);
	dummyDemon->spirit->state = MY_DemonSpirit::kOUT;
	dummyDemon->spirit->meshTransform->setVisible(true);
	dummyDemon->isDummy = true;
	dummyDemon->mesh->setVisible(false);

	demonsCounter->decrement();
}

Room * MY_Scene_Main::goToNewRoom(){
	previousRoom = currentRoom;

	// clear out old stuff
	if(previousRoom != nullptr){
		player->eventManager.listeners.clear();
		player->joystick = nullptr;
		player->voiceTimer->stop();

		demons.clear();
	}

	isBossRoom = !demonsCounter->getItemCount() < MAX_DEMON_COUNT || true;

	Room * res = currentRoom = !isBossRoom ? new Room(baseShader) : new BossRoom(baseShader);

	childTransform->addChild(res);
	
	res->placeBG();
	res->placeGG();

	player = spawnPlayer(res);

	if(isBossRoom) {
		boss = spawnBoss(res);
		addDummyDemon(res);
	} else{
		if(demonsCounter->getItemCount() < MAX_DEMON_COUNT){
			float numSpawnedDemons = sweet::NumberUtils::randomInt(1, MAX_DEMON_COUNT - demonsCounter->getItemCount() < MAX_SPAWNED_DEMON_COUNT ? MAX_DEMON_COUNT - demonsCounter->getItemCount() : MAX_SPAWNED_DEMON_COUNT);
			for(int i = 0; i < numSpawnedDemons; ++i){
				spawnDemon(res);
			}
		}
	}

	currentRoom->placeFG();
	roomTransition->restart();

	
	// Setup the player and game events
	player->eventManager.addEventListener("tookDamage", [this](sweet::Event * _event){
		mainCam->shakeTimer->restart();
		livesCounter->decrement();
		player->state = MY_Player::kHURT;
		gripTarget = nullptr;
		ripTarget = nullptr;
		player->delayChange(0.5f, MY_Player::kIDLE);
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

	if(isBossRoom){
		player->eventManager.addEventListener("hitBoss", [this](sweet::Event * _event){
			// BOSS LOGIC
			ST_LOG_INFO("COLLIDE");
			dummyDemon->kill(false);
			gripTarget = nullptr;

			++boss->hits;
			if(boss->hits < MAX_DEMON_COUNT){
				addDummyDemon(currentRoom);
			}else{
				Log::info("gj you beat the boss");
				// TODO: disable boss, enable walking into trigger
				boss->die();
				player->eventManager.addEventListener("newroom", [this](sweet::Event * _event){
					// go to finale instead of new room
					goToNewRoom();
				});
			}
		});
	}else{
		
		// anytime we're not in the boss room, going to the next room just means loading a new room
		player->eventManager.addEventListener("newroom", [this](sweet::Event * _event){
			goToNewRoom();
		});
	}


	return res;
}

void MY_Scene_Main::update(Step * _step){
	// Screen shader update
	// Screen shaders are typically loaded from a file instead of built using components, so to update their uniforms
	// we need to use the OpenGL API calls
	if(player->state == MY_Player::kRIP_AND_GRIP){
		screenMagnitude += 0.1f;
	}else{
		screenMagnitude *= 0.8f;
		screenMagnitude -= 0.1f;
		if(screenMagnitude < 0){
			screenMagnitude = 0;
		}
	}
	
	screenSurfaceShader->bindShader(); // remember that we have to bind the shader before it can be updated
	GLint test = glGetUniformLocation(screenSurfaceShader->getProgramId(), "time");
	checkForGlError(0,__FILE__,__LINE__);
	if(test != -1){
		glUniform1f(test, _step->time);
		checkForGlError(0,__FILE__,__LINE__);
	}
	test = glGetUniformLocation(screenSurfaceShader->getProgramId(), "magnitude");
	checkForGlError(0,__FILE__,__LINE__);
	if(test != -1){
		glUniform1f(test, screenMagnitude);
		checkForGlError(0,__FILE__,__LINE__);
	}


	// Scene update
	MY_Scene_Base::update(_step);

	if(keyboard->keyJustDown(GLFW_KEY_ESCAPE)){
		game->switchScene("menu", false);
		game->scenes["game"] = this;
	}

	if(!started){ // Because we don't have the UI square thing
		started = true;
		livesCounter->setItemCount(MY_Player::lives);
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
	if(keyboard->keyJustDown(GLFW_KEY_L)){
		screenSurfaceShader->unload();
		screenSurfaceShader->loadFromFile(screenSurfaceShader->vertSource, screenSurfaceShader->fragSource);
		screenSurfaceShader->load();
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

	if(gripTarget == nullptr && ripTarget == nullptr){
		hoverTarget = getHovered();
	}else{
		calcHover(hoverTarget);
	}
	if(mouse->leftJustPressed()){
		player->state = MY_Player::kRIP_AND_GRIP;
		if(hoverTarget != nullptr){
			if(hoverTarget->state == MY_DemonSpirit::kIN){
				ripTarget = hoverTarget;
			}else if(hoverTarget->state == MY_DemonSpirit::kSTUNNED || hoverTarget->state == MY_DemonSpirit::kOUT){
				gripTarget = hoverTarget;
			}
		}
	}

	if(mouse->leftDown()) {
		if(player->state == MY_Player::kRIP_AND_GRIP) {
			glm::vec3 playerPos = player->meshTransform->getWorldPos();
			glm::vec3 playerPosInScreen = mainCam->worldToScreen(playerPos, sweet::getWindowDimensions());
			if(mouse->mouseX() - playerPosInScreen.x > 0) {
				player->meshTransform->scale(1,1,1, false);
			}else {
				player->meshTransform->scale(-1,1,1, false);	
			}
		}
	}

	// let go of any held demons
	if(mouse->leftJustReleased()){
		ripTarget = nullptr;
		gripTarget = nullptr;
		if(player->state == MY_Player::kRIP_AND_GRIP){
			player->state = MY_Player::kIDLE;
		}
	}
	
	// if we're holding a demon inside a body, try to rip it
	if(ripTarget != nullptr){
		if(ripTarget->state == MY_DemonSpirit::kIN){
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




void MY_Scene_Main::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	// keep our screen framebuffer up-to-date with the game's viewport
	screenFBO->resize(game->viewPortWidth, game->viewPortHeight);

	// bind our screen framebuffer
	FrameBufferInterface::pushFbo(screenFBO);
	// render the scene
	_renderOptions->clear();
	Scene::render(_matrixStack, _renderOptions);
	// unbind our screen framebuffer, rebinding the previously bound framebuffer
	// since we didn't have one bound before, this will be the default framebuffer (i.e. the one visible to the player)
	FrameBufferInterface::popFbo();

	// render our screen framebuffer using the standard render surface
	screenSurface->render(screenFBO->getTextureId());

	// render the uiLayer after the screen surface in order to avoid hiding it through shader code
	uiLayer->render(_matrixStack, _renderOptions);
}

void MY_Scene_Main::enableDebug(){
	MY_Scene_Base::enableDebug();
}
void MY_Scene_Main::disableDebug(){
	MY_Scene_Base::disableDebug();
}

void MY_Scene_Main::collideEntities() {
	auto  ptrans = player->firstParent()->getTranslationVector();
	float pMin = ptrans.x - (player->firstParent()->getScaleVector().x  * 0.25f);
	float pMax = ptrans.x + (player->firstParent()->getScaleVector().x  * 0.25f);

	if(isBossRoom){
		if(!boss->isDead){
			if(gripTarget == dummyDemon->spirit){
				auto  btrans = boss->firstParent()->getTranslationVector();
				float bMin = btrans.x - (boss->firstParent()->getScaleVector().x  * 0.25f);
				float bMax = btrans.x + (boss->firstParent()->getScaleVector().x  * 0.25f);
	
				auto dtrans = gripTarget->meshTransform->getWorldPos();
				float dMin = dtrans.x - gripTarget->meshTransform->getScaleVector().x;
				float dMax = dtrans.x + gripTarget->meshTransform->getScaleVector().x;

				if((bMax >= dMin && bMin <= dMax) ||
					bMin <= dMax && bMax >= dMin) {
						boss->eventManager.triggerEvent("spiritCollision");
						player->eventManager.triggerEvent("hitBoss");
				}
			}

			float pMinY = ptrans.y;
			float pMaxY = ptrans.y + (player->firstParent()->getScaleVector().y);
		
			for(auto i : boss->enabledSpewers){
				Sprite * s = boss->spewers.at(i)->spew;
				if(s->isVisible()){
					auto strans = s->meshTransform->getWorldPos();
					float sMinX = strans.x - s->meshTransform->getScaleVector().x * 0.25f;
					float sMaxX = strans.x + s->meshTransform->getScaleVector().x * 0.25f;
					float sMinY = strans.y - s->mesh->calcBoundingBox().height;
					float sMaxY = strans.y;

					if( ((pMax >= sMinX && pMin <= sMaxX) || (pMin <= sMaxX && pMax >= sMinX)) &&
						((pMaxY >= sMinY && pMinY <= sMaxY) || (pMinY <= sMaxY && pMaxY >= sMinY))) {
							player->eventManager.triggerEvent("demonCollision");
					}
				}
			}
		}
	}else {
	
		for(auto demon : demons) {
			if(demon->state != MY_Demon::kDEAD && demon->spirit->state == MY_DemonSpirit::kIN){
				auto dtrans = demon->firstParent()->getTranslationVector();
				float dMin = dtrans.x - (demon->firstParent()->getScaleVector().x  * 0.25f);
				float dMax = dtrans.x + (demon->firstParent()->getScaleVector().x  * 0.25f);

				if((pMax >= dMin && pMin <= dMax) ||
					pMin <= dMax && pMax >= dMin) {
						sweet::Event * e = new sweet::Event("demonCollision");
						e->setFloatData("damage", demon->damage);
						player->eventManager.triggerEvent(e);

						demon->eventManager.triggerEvent("playerCollisoin");
				}
			}
		}
	}
}

MY_Demon * MY_Scene_Main::spawnDemon(Room * _room){
	MY_Demon * d = new MY_Demon(baseShaderWithDepth, player->firstParent());
	_room->gameground->addChild(d)->translate(glm::vec3(sweet::NumberUtils::randomFloat(0, _room->doorPos*0.9f), 0, 0), false);
	demons.push_back(d);
	return d;
}

MY_DemonBoss * MY_Scene_Main::spawnBoss(Room* _room) {
	MY_DemonBoss * boss = new MY_DemonBoss(baseShaderWithDepth);
	_room->gameground->addChild(boss);
	boss->firstParent()->translate(_room->doorPos*0.8f, 0.f, 0.f, false);

	// load spewers
	for(int i = 0; i < 3; ++i){
		spawnSpewer(_room, boss, i);
	}

	return boss;
}

MY_Spewer * MY_Scene_Main::spawnSpewer(Room * _room, MY_DemonBoss * _boss, int _column){
	float x = _room->doorPos * 2 * _column / 4 - _room->doorPos;
	glm::vec3 targetPos = glm::vec3(x, _room->mesh->calcBoundingBox().height, 0);

	MY_Spewer * spewer = new MY_Spewer(baseShaderWithDepth, _boss->firstParent()->getTranslationVector(), targetPos, _column);
	_boss->addSpewer(spewer);
	_room->gameground->addChild(spewer);

	return spewer;
}

MY_Player * MY_Scene_Main::spawnPlayer(Room * _room){
	// Setup the player
	MY_Player * p = new MY_Player(baseShaderWithDepth);
	_room->gameground->addChild(p)->scale(10)->translate(glm::vec3(-_room->doorPos*0.9f, 0, 0), false);
	p->bounds = _room->doorPos;
	return p;
}

MY_DemonSpirit * MY_Scene_Main::getHovered(){
	float min = hoverRadius;
	glm::vec3 demonPos, demonPosInScreen;
	glm::vec2 dist;
	float distMag;
	MY_DemonSpirit * res = nullptr;
	
	for(auto d : demons){
		if(d->state == MY_DemonSpirit::kDEAD){
			continue;
		}
		
		demonPos = d->spirit->meshTransform->getWorldPos();
		demonPosInScreen = mainCam->worldToScreen(demonPos, sweet::getWindowDimensions());
		dist = glm::vec2(demonPosInScreen.x, demonPosInScreen.y) - glm::vec2(mouse->mouseX(), mouse->mouseY());
		distMag = glm::length(dist);

		d->spirit->indicator->meshTransform->scale(glm::min(3.f,(100/distMag)), false);

		if(distMag < min){
			min = distMag;
			res = d->spirit;
		}

		if(!d->isDummy){
			demonPos = d->spiritFake1->meshTransform->getWorldPos();
			demonPosInScreen = mainCam->worldToScreen(demonPos, sweet::getWindowDimensions());
			dist = glm::vec2(demonPosInScreen.x, demonPosInScreen.y) - glm::vec2(mouse->mouseX(), mouse->mouseY());
			distMag = glm::length(dist);

			if(distMag < min){
				min = distMag;
				res = d->spiritFake1;
			}


			demonPos = d->spiritFake2->meshTransform->getWorldPos();
			demonPosInScreen = mainCam->worldToScreen(demonPos, sweet::getWindowDimensions());
			dist = glm::vec2(demonPosInScreen.x, demonPosInScreen.y) - glm::vec2(mouse->mouseX(), mouse->mouseY());
			distMag = glm::length(dist);

			if(distMag < min){
				min = distMag;
				res = d->spiritFake2;
			}
		}
	}
	
	
	// we didn't find a hoverTarget
	if(res != nullptr){
		calcHover(res);
	}
	return res;
}

void MY_Scene_Main::calcHover(MY_DemonSpirit * _demonSpirit){
	glm::vec3 demonPos = _demonSpirit->meshTransform->getWorldPos();
	glm::vec3 demonPosInScreen = mainCam->worldToScreen(demonPos, sweet::getWindowDimensions());
	distToHoverTarget = glm::vec2(demonPosInScreen.x, demonPosInScreen.y) - glm::vec2(mouse->mouseX(), mouse->mouseY());
	distToHoverTargetMag = glm::length(distToHoverTarget);
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
		float mouseResistance = 0.8f;
		float demonResistance = 0.01f;
		mouse->translate(distToHoverTarget*mouseResistance);
		ripTarget->firstParent()->translate(glm::vec3(distToHoverTarget.x, distToHoverTarget.y, 0)*-demonResistance);
	}
}

void MY_Scene_Main::gripIt(){
	// demon pulls the mouse, mouse pulls the demon
	if(distToHoverTargetMag > glm::length(gripTarget->scaleAnim)){
		float mouseResistance = 0.8f;
		float demonResistance = 0.01f;
		mouse->translate(distToHoverTarget*mouseResistance);
		gripTarget->firstParent()->translate(glm::vec3(distToHoverTarget.x, distToHoverTarget.y, 0)*-demonResistance);
		gripTarget->gripIt();
	}
}

void MY_Scene_Main::sipIt(){
	if(!isBossRoom){
		// if the demon gets close enough to the player, they get sipped
		glm::vec3 demPos = gripTarget->getGamePos();
		glm::vec3 playerPos = player->firstParent()->getTranslationVector();
		playerPos.y += player->firstParent()->getScaleVector().y*0.9;

		if(glm::distance(demPos, playerPos) < gripTarget->scaleAnim.y * 0.5f){
			gripTarget->sipIt();
			demonsCounter->increment();

			// TODO: trigger sip animation on player, pass out animation on enemy, remove spirit
			player->setCurrentAnimation("sip");
			player->state = MY_Player::kSIP;
			player->delayChange(1.f, MY_Player::kIDLE);

			gripTarget = nullptr;
		}
	}
}

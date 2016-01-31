#pragma once

#include <MY_Scene_Menu.h>
#include <shader/ComponentShaderText.h>
#include <sweet/UI.h>
#include <MY_Button.h>
#include <MY_Scene_Instructions.h>
#include <MY_Scene_Intro.h>

#include <RenderSurface.h>
#include <StandardFrameBuffer.h>

MY_Scene_Menu::MY_Scene_Menu(Game * _game) :
	MY_Scene_Base(_game),
	screenSurfaceShader(new Shader("assets/RenderSurface_2", false, true)),
	screenSurface(new RenderSurface(screenSurfaceShader)),
	screenFBO(new StandardFrameBuffer(true))
{

	// Create a linear layout to contain all of our menu items
	// remember that these elements are all going to exist in the uiLayer, so we pass in its physics world in the constructors
	NodeUI * layout = new NodeUI(uiLayer->world);
	// set the layout's size to 100% of its parent
	layout->setRationalHeight(1.f);
	layout->setRationalWidth(1.f);
	layout->setBackgroundColour(1.f, 1.f, 1.f);
	layout->background->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("menu_bg")->texture);
	layout->background->mesh->setScaleMode(GL_NEAREST);

	MY_Button * gameButton = new MY_Button(uiLayer->world, "menu_play");
	MY_Button * howButton = new MY_Button(uiLayer->world, "menu_howtoplay");
	MY_Button * exitButton = new MY_Button(uiLayer->world, "menu_quit");

	float gameBtnW = 0.128f;
	float gameBtnH = 0.15;
	float gameBtnX = 0.83f;
	float gameBtnY = 0.475f;

	float exitBtnW = 0.138f;
	float exitBtnH = 0.15f;
	float exitBtnX = 0.83f;
	float exitBtnY = 0.3f;

	gameButton->setRationalWidth(1.f, layout);
	gameButton->setRationalHeight(1.f, layout);
	howButton->setRationalWidth(1.f, layout);
	howButton->setRationalHeight(1.f, layout);
	exitButton->setRationalWidth(1.f, layout);
	exitButton->setRationalHeight(1.f, layout);
	
	gameButton->setMarginLeft(gameBtnX);
	gameButton->setMarginRight(1.f - gameBtnX - gameBtnW);
	gameButton->setMarginBottom(gameBtnY);
	gameButton->setMarginTop(1.f - gameBtnY - gameBtnH);
	howButton->setMarginLeft(exitBtnX);
	howButton->setMarginRight(1.f - exitBtnX - exitBtnW);
	howButton->setMarginBottom(exitBtnY);
	howButton->setMarginTop(1.f - exitBtnY - exitBtnH);
	exitButton->setMarginLeft(exitBtnX);
	exitButton->setMarginRight(1.f - exitBtnX - exitBtnW);
	exitButton->setMarginBottom((exitBtnY - 0.15f));
	exitButton->setMarginTop(1.f - (exitBtnY - 0.15f) - exitBtnH);
	
	// make the buttons clickable
	gameButton->setMouseEnabled(true);
	howButton->setMouseEnabled(true);
	exitButton->setMouseEnabled(true);

	// add listeners to each label, making them buttons that take the player to different scenes
	gameButton->eventManager.addEventListener("click", [&](sweet::Event * _event){
		game->scenes["intro"] = new MY_Scene_Intro(game);
		game->switchScene("intro", false);
	});
	howButton->eventManager.addEventListener("click", [&](sweet::Event * _event){
		game->scenes["instructions"] = new MY_Scene_Instructions(game);
		game->switchScene("instructions", false);
	});
	exitButton->eventManager.addEventListener("click", [&](sweet::Event * _event){
		game->exit();
	});

	// add the labels to the layout
	layout->addChild(gameButton);
	layout->addChild(howButton);
	layout->addChild(exitButton);

	// add the layout to the uiLayer
	uiLayer->addChild(layout);

	uiLayer->invalidateLayout();

	// add a mouse indicator (AKA a cursor) to the uiLayer so that the user can see what they're doing
	uiLayer->addMouseIndicator();
}

void MY_Scene_Menu::update(Step * _step){
	// Screen shader update
	// Screen shaders are typically loaded from a file instead of built using components, so to update their uniforms
	// we need to use the OpenGL API calls
	screenSurfaceShader->bindShader(); // remember that we have to bind the shader before it can be updated
	GLint test = glGetUniformLocation(screenSurfaceShader->getProgramId(), "time");
	checkForGlError(0,__FILE__,__LINE__);
	if(test != -1){
		glUniform1f(test, _step->time);
		checkForGlError(0,__FILE__,__LINE__);
	}
	
	if(keyboard->keyJustDown(GLFW_KEY_L)){
		screenSurfaceShader->unload();
		screenSurfaceShader->loadFromFile(screenSurfaceShader->vertSource, screenSurfaceShader->fragSource);
		screenSurfaceShader->load();
	}

	MY_Scene_Base::update(_step);
}
void MY_Scene_Menu::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	// keep our screen framebuffer up-to-date with the game's viewport
	screenFBO->resize(game->viewPortWidth, game->viewPortHeight);

	// bind our screen framebuffer
	FrameBufferInterface::pushFbo(screenFBO);
	// render the scene
	MY_Scene_Base::render(_matrixStack, _renderOptions);
	// unbind our screen framebuffer, rebinding the previously bound framebuffer
	// since we didn't have one bound before, this will be the default framebuffer (i.e. the one visible to the player)
	FrameBufferInterface::popFbo();

	// render our screen framebuffer using the standard render surface
	screenSurface->render(screenFBO->getTextureId());
}

#pragma once

#include <MY_Scene_Menu.h>
#include <shader/ComponentShaderText.h>
#include <sweet/UI.h>

MY_Scene_Menu::MY_Scene_Menu(Game * _game) :
	MY_Scene_Base(_game)
{
	// Create a linear layout to contain all of our menu items
	// remember that these elements are all going to exist in the uiLayer, so we pass in its physics world in the constructors
	VerticalLinearLayout * layout = new VerticalLinearLayout(uiLayer->world);
	layout->horizontalAlignment = kCENTER;
	layout->verticalAlignment = kMIDDLE;
	// set the layout's size to 100% of its parent
	layout->setRationalHeight(1.f);
	layout->setRationalWidth(1.f);

	// Create some text labels
	TextLabel * gameLabel = new TextLabel(uiLayer->world, font, textShader);
	TextLabel * exitLabel = new TextLabel(uiLayer->world, font, textShader);

	// set the text on the labels
	gameLabel->setText("Exorcise!");
	exitLabel->setText("Exit");

	// make the labels' background visible (by default both the scene's clear colour and the text colour will be black)
	gameLabel->setBackgroundColour(1,1,1,1);
	exitLabel->setBackgroundColour(1,1,1,1);

	// make the labels clickable
	gameLabel->setMouseEnabled(true);
	exitLabel->setMouseEnabled(true);

	// add listeners to each label, making them buttons that take the player to different scenes
	gameLabel->eventManager.addEventListener("click", [&](sweet::Event * _event){
		game->switchScene("game", false);
	});
	exitLabel->eventManager.addEventListener("click", [&](sweet::Event * _event){
		game->exit();
	});

	// add the labels to the layout
	layout->addChild(gameLabel);
	layout->addChild(exitLabel);

	// add the layout to the uiLayer
	uiLayer->addChild(layout);

	uiLayer->invalidateLayout();

	// add a mouse indicator (AKA a cursor) to the uiLayer so that the user can see what they're doing
	uiLayer->addMouseIndicator();
}
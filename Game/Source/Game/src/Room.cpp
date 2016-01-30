#pragma once

#include <Room.h>
#include <MeshFactory.h>
#include <MeshInterface.h>
#include <MY_ResourceManager.h>
#include <Resource.h>

Room::Room(Shader * _shader) :
	MeshEntity(Resource::loadMeshFromObj("assets/meshes/room.obj", true).at(0), _shader),
	roomWidth(mesh->calcBoundingBox().width),
	doorPos(roomWidth * 0.475f),
	foreground(new Transform()),
	gameground(new Transform()),
	background(new Transform()),
	unlocked(true)
{
	Texture * roomTex = new Texture("assets/textures/testUVAI.png", false, true, false);
	roomTex->load();
	mesh->pushTexture2D(roomTex);
	mesh->setScaleMode(GL_NEAREST);
	
	Texture * doorTex = new Texture("assets/textures/testDoorUV.png", false, true, false);
	doorTex->load();

	MeshEntity * entryDoor = new MeshEntity(Resource::loadMeshFromObj("assets/meshes/entryDoor.obj", true).at(0), getShader());
	entryDoor->mesh->pushTexture2D(doorTex);
	entryDoor->mesh->setScaleMode(GL_NEAREST);
	childTransform->addChild(entryDoor);
	
	MeshEntity * exitDoor = new MeshEntity(Resource::loadMeshFromObj("assets/meshes/exitDoor.obj", true).at(0), getShader());
	exitDoor->mesh->pushTexture2D(doorTex);
	exitDoor->mesh->setScaleMode(GL_NEAREST);
	childTransform->addChild(exitDoor);
}

void Room::placeFG(){
	/*childTransform->addChild(foreground);
	MeshEntity * testForeground = new MeshEntity(MeshFactory::getPlaneMesh(), getShader());
	testForeground->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("FG")->texture);
	testForeground->childTransform->scale(width, height * 0.2, 1.f);
	for (unsigned long int i = 0; i < testForeground->mesh->vertices.size(); ++i){
		testForeground->mesh->vertices.at(i).x += 0.5f;
		testForeground->mesh->vertices.at(i).y += 0.5f;
	}

	foreground->addChild(testForeground);*/
}

void Room::placeBG(){	
	/*childTransform->addChild(background);
	MeshEntity * testBackground = new MeshEntity(MeshFactory::getPlaneMesh(), getShader());
	testBackground->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("BG")->texture);
	testBackground->childTransform->scale(width, height * 0.8, 1.f);
	for (unsigned long int i = 0; i < testBackground->mesh->vertices.size(); ++i){
		testBackground->mesh->vertices.at(i).x += 0.5f;
		testBackground->mesh->vertices.at(i).y += 0.5f;
	}
	background->addChild(testBackground);
	background->translate(0, 0, -depth * 0.9);*/

	Texture * curtainTex = new Texture("assets/textures/curtainsTest.png", false, true, false);
	curtainTex->load();

	childTransform->addChild(background)->translate(0, 0, -0.9);
	MeshEntity * curtains = new MeshEntity(Resource::loadMeshFromObj("assets/meshes/curtains.obj", true).at(0), getShader());
	curtains->mesh->pushTexture2D(curtainTex);
	curtains->mesh->setScaleMode(GL_NEAREST);
	background->addChild(curtains);
}

void Room::placeGG(){	
	childTransform->addChild(gameground)->translate(0, 0, 2.75);
}

void Room::unlock(){
	if(!unlocked){
		unlocked = true;
		// door sound/animation?
	}
}

Room::~Room(){

}
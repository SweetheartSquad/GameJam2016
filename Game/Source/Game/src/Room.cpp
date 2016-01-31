#pragma once

#include <Room.h>
#include <MeshFactory.h>
#include <MeshInterface.h>
#include <MY_ResourceManager.h>
#include <Resource.h>

sweet::ShuffleVector<unsigned long int> Room::roomTexIdx;
sweet::ShuffleVector<unsigned long int> Room::furnitureSetIdx;

bool Room::staticInit(){
	for(unsigned long int i = 1; i <= 5; ++i){
		roomTexIdx.push(i);
	}
	for(unsigned long int i = 1; i <= 5; ++i){
		furnitureSetIdx.push(i);
	}
	return true;
}
bool Room::staticInitialized = Room::staticInit();

Room::Room(Shader * _shader) :
	MeshEntity(Resource::loadMeshFromObj("assets/meshes/room.obj", true).at(0), _shader),
	roomWidth(mesh->calcBoundingBox().width),
	doorPos(roomWidth * 0.475f),
	foreground(new Transform()),
	gameground(new Transform()),
	background(new Transform()),
	unlocked(true)
{
	mesh->pushTexture2D(getRoomTex());
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
	childTransform->addChild(background)/*->translate(0, 0, -mesh->calcBoundingBox().depth * 0.5 * 0.9)*/;

	background->addChild(getFurnitureSet());
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

Texture * Room::getRoomTex(){
	// grab a random floor texture
	std::stringstream ss;
	ss << "assets/textures/rooms/" << roomTexIdx.pop() << ".png";
	Texture * res = new Texture(ss.str(), false, true, true);
	res->load();
	return res;
}

MeshEntity * Room::getFurnitureSet(){
	// grab a random furniture set
	int idx = furnitureSetIdx.pop();

	std::stringstream ssTex;
	ssTex << "assets/textures/furnitureSets/" << idx << ".png";
	Texture * tex = new Texture(ssTex.str(), false, true, false);
	tex->load();
	
	std::stringstream ssObj;
	ssObj << "assets/meshes/furnitureSets/" << idx << ".obj";
	MeshEntity * res = new MeshEntity(Resource::loadMeshFromObj(ssObj.str(), true).at(0), getShader());
	res->mesh->pushTexture2D(tex);
	res->mesh->setScaleMode(GL_NEAREST);

	return res;
}
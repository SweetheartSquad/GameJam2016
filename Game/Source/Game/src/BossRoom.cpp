#pragma once

#include <BossRoom.h>
#include <MeshFactory.h>
#include <MeshInterface.h>
#include <MY_ResourceManager.h>
#include <Resource.h>

BossRoom::BossRoom(Shader * _shader) :
	Room(_shader)
{
	mesh->replaceTextures(getRoomTex());
}


BossRoom::~BossRoom(){

}

Texture * BossRoom::getRoomTex(){
	// grab a random floor texture
	std::stringstream ss;
	ss << "assets/textures/rooms/boss.png";//"assets/textures/rooms/" << roomTexIdx.pop() << ".png";
	Texture * res = new Texture(ss.str(), false, true, true);
	res->load();
	return res;
}

MeshEntity * BossRoom::getFurnitureSet(){
	// grab a random furniture set
	int idx = 1;

	/*
	std::stringstream ssTex;
	ssTex << "assets/textures/furnitureSets/" << idx << ".png";
	Texture * tex = new Texture(ssTex.str(), false, true, false);
	tex->load();
	*/

	std::stringstream ssObj;
	ssObj << "assets/meshes/furnitureSets/boss.obj";
	MeshEntity * res = new MeshEntity(MY_ResourceManager::globalAssets->getMesh("BOSS")->meshes.at(0), getShader());
	Texture * tex = new Texture("assets/textures/furnitureSets/boss.png", false, true, false);
	tex->load();

	res->mesh->pushTexture2D(tex);
	res->mesh->setScaleMode(GL_NEAREST);

	return res;
}
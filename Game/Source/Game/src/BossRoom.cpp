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
	ss << "assets/textures/demon_1.png";//"assets/textures/rooms/" << roomTexIdx.pop() << ".png";
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
	ssObj << "assets/meshes/furnitureSets/" << idx << ".obj";
	MeshEntity * res = new MeshEntity(Resource::loadMeshFromObj(ssObj.str(), true).at(0), getShader());
	res->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("DEFAULT")->texture);
	res->mesh->setScaleMode(GL_NEAREST);

	return res;
}
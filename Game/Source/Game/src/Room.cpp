#pragma once

#include <Room.h>
#include <MeshFactory.h>
#include <MeshInterface.h>
#include <MY_ResourceManager.h>

Room::Room(Shader * _shader, float _width, float _height, float _depth) :
	MeshEntity(Room::getRoomMesh(_width, _height, _depth), _shader),
	foreground(new Transform()),
	gameground(new Transform()),
	background(new Transform())
{
	mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("DEFAULT")->texture);
	
	childTransform->addChild(foreground);
	MeshEntity * testForeground = new MeshEntity(MeshFactory::getPlaneMesh(), _shader);
	testForeground->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("DEFAULT")->texture);
	testForeground->childTransform->scale(_width, _height * 0.2, 1.f);
	for (unsigned long int i = 0; i < testForeground->mesh->vertices.size(); ++i){
		testForeground->mesh->vertices.at(i).x += 0.5f;
		testForeground->mesh->vertices.at(i).y += 0.5f;
	}

	foreground->addChild(testForeground);

	childTransform->addChild(gameground);
	childTransform->addChild(background);
	MeshEntity * testBackground = new MeshEntity(MeshFactory::getPlaneMesh(), _shader);
	testBackground->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("DEFAULT")->texture);
	testBackground->childTransform->scale(_width, _height * 0.8, 1.f);
	for (unsigned long int i = 0; i < testBackground->mesh->vertices.size(); ++i){
		testBackground->mesh->vertices.at(i).x += 0.5f;
		testBackground->mesh->vertices.at(i).y += 0.5f;
	}
	background->addChild(testBackground);

	background->translate(0, 0, -_depth * 0.9);
	gameground->translate(0, 0, -_depth * 0.5);

	
}



Room::~Room(){

}

MeshInterface * Room::getRoomMesh(float _width, float _height, float _depth){
	// A Rectangular prism with its bottom left edge centered at the origin
	// Does not have a front side

	QuadMesh * m = new QuadMesh(true);
	//Top
	m->pushVert(Vertex(0, _height, _depth)); // left top front
	m->pushVert(Vertex(0, _height, -_depth)); // left top back
	m->pushVert(Vertex(_width, _height, -_depth)); // right top back
	m->pushVert(Vertex(_width, _height, _depth));  // right top front
	m->setNormal(0, 0.0, 1.0, 0.0);
	m->setNormal(1, 0.0, 1.0, 0.0);
	m->setNormal(2, 0.0, 1.0, 0.0);
	m->setNormal(3, 0.0, 1.0, 0.0);
	m->setUV(0, 0.33, 0.0);
	m->setUV(1, 0.33, 1.0);
	m->setUV(2, 0.66, 1.0);
	m->setUV(3, 0.66, 0.0);

	//Bottom
	m->pushVert(Vertex(0, 0, _depth));
	m->pushVert(Vertex(_width, 0, _depth));
	m->pushVert(Vertex(_width, 0, -_depth));
	m->pushVert(Vertex(0, 0, -_depth));
	m->setNormal(4, 0.0, -1.0, 0.0);
	m->setNormal(5, 0.0, -1.0, 0.0);
	m->setNormal(6, 0.0, -1.0, 0.0);
	m->setNormal(7, 0.0, -1.0, 0.0);
	m->setUV(4, 0.33, 0.0);
	m->setUV(5, 0.66, 1.0);
	m->setUV(6, 0.66, 1.0);
	m->setUV(7, 0.33, 0.0);
	
	//Back
	m->pushVert(Vertex(_width, _height, -_depth));
	m->pushVert(Vertex(0, _height, -_depth));
	m->pushVert(Vertex(0, 0, -_depth));
	m->pushVert(Vertex(_width, 0, -_depth));
	m->setNormal(8, 0.0, 0.0, -1.0);
	m->setNormal(9, 0.0, 0.0, -1.0);
	m->setNormal(10, 0.0, 0.0, -1.0);
	m->setNormal(11, 0.0, 0.0, -1.0);
	m->setUV(8, 0.66, 0.0);
	m->setUV(9, 0.33, 1.0);
	m->setUV(10, 0.33, 1.0);
	m->setUV(11, 0.66, 0.0);

	//Left
	m->pushVert(Vertex(0, _height, -_depth));
	m->pushVert(Vertex(0, _height, _depth));
	m->pushVert(Vertex(0, 0, _depth));
	m->pushVert(Vertex(0, 0, -_depth));
	m->setNormal(12, -1.0, 0.0, 0.0);
	m->setNormal(13, -1.0, 0.0, 0.0);
	m->setNormal(14, -1.0, 0.0, 0.0);
	m->setNormal(15, -1.0, 0.0, 0.0);
	m->setUV(12, 0.33, 0.0);
	m->setUV(13, 0.0, 1.0);
	m->setUV(14, 1.0, 1.0);
	m->setUV(15, 1.0, 0.0);

	//Right
	m->pushVert(Vertex(_width, _height, _depth));
	m->pushVert(Vertex(_width, _height, -_depth));
	m->pushVert(Vertex(_width, 0, -_depth));
	m->pushVert(Vertex(_width, 0, _depth));
	m->setNormal(16, 1.0, 0.0, 0.0);
	m->setNormal(17, 1.0, 0.0, 0.0);
	m->setNormal(18, 1.0, 0.0, 0.0);
	m->setNormal(19, 1.0, 0.0, 0.0);
	m->setUV(16, 0.0, 0.0);
	m->setUV(17, 0.0, 1.0);
	m->setUV(18, 1.0, 1.0);
	m->setUV(19, 1.0, 0.0);
	return m;
}
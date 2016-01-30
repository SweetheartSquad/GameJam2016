#pragma once

#include <MeshEntity.h>
class Entity;
class Shader;

class Room : public MeshEntity {
public:
	Transform * foreground;
	Transform * gameground;
	Transform * background;

	bool unlocked;
	float doorPos;

	std::vector<Entity *> demons;

	Room(Shader * _shader, float _width, float _height, float _depth = 10.f);
	~Room();

	void unlock();
	static MeshInterface * getRoomMesh(float _width, float _height, float _depth);
};
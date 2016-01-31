#pragma once
#include <Texture.h>
#include <OpenALSound.h>
#include <shader/ComponentShaderBase.h>
#include <shader/ShaderComponentAlpha.h>
class Slide {
public:
	Texture * tex;
	OpenAL_Sound * sound;

	Slide(Texture * _tex, float _length = 1.f, OpenAL_Sound * _sound = nullptr);
	~Slide();
};
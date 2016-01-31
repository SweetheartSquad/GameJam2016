#pragma once

#include <Slide.h>
#include <shader/ShaderComponentMVP.h>
#include <shader/ShaderComponentTexture.h>

Slide::Slide(Texture * _tex, float _length, OpenAL_Sound * _sound) :
	tex(_tex),
	sound(_sound)
{
}

Slide::~Slide(){

}
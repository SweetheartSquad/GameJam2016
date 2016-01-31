#pragma once

#include <MY_Scene_Base.h>
#include <shader\ShaderComponentHsv.h>

class MY_Scene_Menu : public MY_Scene_Base{
public:
	Shader * screenSurfaceShader;
	RenderSurface * screenSurface;
	StandardFrameBuffer * screenFBO;

	MY_Scene_Menu(Game * _game);

	virtual void update(Step * _step);
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
};
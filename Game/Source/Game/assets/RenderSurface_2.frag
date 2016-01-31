#version 150

in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D texFramebuffer;
uniform float time = 0;
uniform float magnitude = 0;
uniform float magnitude2 = 0;

// http://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}


vec4 standard(vec2 _uv){
	return texture(texFramebuffer, _uv);
}

vec4 mod(vec2 _uv){
	vec3 rgb = standard(_uv - vec2(cos(time*33)*0.0005,abs(sin(time*100))*0.005)).rgb;
	vec3 rgbs = standard(_uv).rgb;
	if(rgb.b < rgb.r){
		rgbs = rgb;
	}
	if(rgbs.b < rgbs.r){
		rgbs.r /= rand(vec2(rgb.r, _uv.x+sin(time)));
		rgbs.g /= rand(vec2(rgb.g, _uv.x+sin(time*3)));
		rgbs.b /= rand(vec2(rgb.b, _uv.x+sin(time*7)));
	}

	return vec4(rgbs, 1);
}

void main() {
    outColor = mod(Texcoord);
}

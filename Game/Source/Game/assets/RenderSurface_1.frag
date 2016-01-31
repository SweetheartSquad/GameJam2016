#version 150

in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D texFramebuffer;
uniform float time = 0;
uniform float magnitude = 0;

// http://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}


vec4 standard(vec2 _uv){
	return texture(texFramebuffer, Texcoord);
}

vec4 mod(vec2 _uv){
	vec3 rgb = texture(texFramebuffer, _uv).rgb;

	if(rgb.r > cos(time)*0.02 + 0.1){
		rgb.r = 1;
	}else{
		rgb.r = 0;
	}
	if(rgb.g > sin(time)*0.02 + 0.1){
		rgb.g = 1;
	}else{
		rgb.g = 0;
	}
	if(rgb.b > sqrt(time)*0.02 + 0.1){
		rgb.b = 1;
	}else{
		rgb.b = 0;
	}
	rgb *= texture(texFramebuffer, _uv).rgb;

	return vec4(rgb, 1);
}


vec4 mod2(vec2 _uv){
	_uv.x += rand(vec2(_uv.x*0.01, time)*0.001);
	_uv.y += rand(vec2(_uv.y*0.01, time)*0.001);
	_uv += (Texcoord - _uv) * (1 - min(magnitude,10)*0.001);

	vec3 rgb = texture(texFramebuffer, _uv).rgb;

	rgb.r +=  rand(vec2(rgb.r*0.01, time)*0.001);
	rgb.g +=  rand(vec2(rgb.g*0.01, time)*0.001);
	rgb.b +=  rand(vec2(rgb.b*0.01, time)*0.001);

	rgb += (texture(texFramebuffer, Texcoord).rgb - rgb) * (1 + max(0, (1-min(1-magnitude,1))) * 0.01 );

	return vec4(rgb, 1);
}


vec4 mod3(vec2 _uv){
	if(fract(_uv.x*100)  < rand(vec2(0.15,time))*magnitude){
		_uv.x += 0.005;
	}
	if(fract(_uv.y*100) < rand(vec2(0.15,time*0.3))*magnitude){
		_uv.y += 0.005;
	}
	_uv += (_uv - Texcoord);

	vec3 rgb = texture(texFramebuffer, _uv).rgb;

	return vec4(rgb, 1);
}


vec4 mod4(vec2 _uv){
	_uv.x += rand(vec2(_uv.x, time)*0.001);
	_uv.y += rand(vec2(_uv.y, time)*0.001);
	_uv += (Texcoord - _uv) * (1 - min(magnitude,5)*0.001);

	vec3 rgb = texture(texFramebuffer, _uv).rgb;

	return vec4(rgb, 1);
}


void main() {
	//if(Texcoord.y > 0.5){
    outColor = mod2(Texcoord);
    //}else{
    //outColor = standard(Texcoord);
    //}//outColor += vec4(Texcoord.x + sin(time*5), Texcoord.y + cos(time*10), 0, 1);//vec4(texture(texFramebuffer, Texcoord));
}

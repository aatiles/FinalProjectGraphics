#version 330 core
in vec4 vColor;
in vec2 texCoord;

out vec4 fragColorOut;

uniform sampler2D tex;
uniform float rez;
uniform float radius;
uniform vec2 dir;
uniform vec3 color;

void main() {
	vec4 sum = vec4(0.0);
	vec2 temps = texCoord;
	float blur = radius/rez;
	
	float hStep = dir.x;
	float vStep = dir.y;
	
	sum += texture2D(tex, vec2(temps.x - 4.0*blur*hStep, temps.y - 4.0*blur*vStep)) * 0.0162162162;
	sum += texture2D(tex, vec2(temps.x - 3.0*blur*hStep, temps.y - 3.0*blur*vStep)) * 0.0540540541;
	sum += texture2D(tex, vec2(temps.x - 2.0*blur*hStep, temps.y - 2.0*blur*vStep)) * 0.1216216216;	
	sum += texture2D(tex, vec2(temps.x - 1.0*blur*hStep, temps.y - 1.0*blur*vStep)) * 0.1945945946;	
	
	sum += texture2D(tex, vec2(temps.x, temps.y)) * 0.2270270270;
	
	sum += texture2D(tex, vec2(temps.x + 1.0*blur*hStep, temps.y + 1.0*blur*vStep)) * 0.1945945946;	
	sum += texture2D(tex, vec2(temps.x + 2.0*blur*hStep, temps.y + 2.0*blur*vStep)) * 0.1216216216;
	sum += texture2D(tex, vec2(temps.x + 3.0*blur*hStep, temps.y + 3.0*blur*vStep)) * 0.0540540541;
	sum += texture2D(tex, vec2(temps.x + 4.0*blur*hStep, temps.y + 4.0*blur*vStep)) * 0.0162162162;
	
  fragColorOut = vec4(sum.rgb, 1.0) * vec4(color, 1);
}

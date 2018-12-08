/*
 *   Fragment Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

//in vec3 theColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
in vec3 FragPos;
out vec4 fragColorOut;
in vec3 Normal;

void main() {

    //*****************************************
    //******* Final Color Calculations ********
    //*****************************************
   
    float specularStrength = 0.5;
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	vec3 specular = pow(max(dot(viewDir, reflectDir),0.0), 32)*vec3(1.0, 1.0, 1.0);
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;
	vec3 result = (ambient + diffuse + specular);
	//result = vec3(1.0, 1.0, 1.0) * (dot(normalize(Normal), viewPos-FragPos) + 0.3);

	fragColorOut = vec4(result, 1.0);

	//fragColorOut = vec4(1.0 , 1.0 ,1.0 ,1.0 );
	
	//fragColorOut = vec4(theColor, 1.0);
}

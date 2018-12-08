/*
 *   Fragment Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

//in vec3 theColor;
uniform vec3 lightPos;
uniform vec3 lightPos1;
uniform vec3 lightPos2;
uniform vec3 viewPos;
in vec3 FragPos;
out vec4 fragColorOut;
in vec3 Normal;

void main() {

    //*****************************************
    //******* Final Color Calculations ********
    //*****************************************
   
  
   //FIRST LIGHT
    float specularStrength = 0.5;
	vec3 lightColor = vec3(1.0, 0.0, 0.0);
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	vec3 specular = pow(max(dot(viewDir, reflectDir),0.0), 32)*vec3(1.0, 1.0, 1.0);
	float ambientStrength = 0.4;
	vec3 ambient = ambientStrength * lightColor;
	vec3 result = (ambient + diffuse + specular);

	//SECOND LIGHT
	lightColor = vec3(0.0, 0.0, 1.0);
	lightDir = normalize(lightPos1 - FragPos);
	diff = max(dot(norm, lightDir), 0.0);
	diffuse = diff * lightColor;
	viewDir = normalize(viewPos - FragPos);
	reflectDir = reflect(-lightDir, norm);
	specular = pow(max(dot(viewDir, reflectDir),0.0), 32)*vec3(1.0, 1.0, 1.0);
	ambient = ambientStrength * lightColor;
	vec3 result1 = (ambient + diffuse + specular);
	
	//THIRD LIGHT
	lightColor = vec3(0.0, 1.0, 0.0);
	lightDir = normalize(lightPos2 - FragPos);
	diff = max(dot(norm, lightDir), 0.0);
	diffuse = diff * lightColor;
	viewDir = normalize(viewPos - FragPos);
	reflectDir = reflect(-lightDir, norm);
	specular = pow(max(dot(viewDir, reflectDir),0.0), 32)*vec3(1.0, 1.0, 1.0);
	ambient = ambientStrength * lightColor;
	vec3 result2 = (ambient + diffuse + specular);
	
	fragColorOut = vec4((result+result1+result2)/3, 1.0);

}

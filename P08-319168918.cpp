/*
Práctica 7: Iluminación 1 
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;


Texture octaedro;

Model LamparaM;
Model CarroAudi;
Model CofreAudi2;
Model LlantaDD;
Model LlantaID;
Model LlantaDA;
Model LlantaIA;
Model Sign;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	bool pointLightsOn = true;

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();

	octaedro = Texture("Textures/Octaedro.png");
	octaedro.LoadTextureA();

	LamparaM = Model();
	LamparaM.LoadModel("Models/lamp.obj");

	CarroAudi = Model();
	CarroAudi.LoadModel("Models/CarroAudi.obj");

	CofreAudi2 = Model();
	CofreAudi2.LoadModel("Models/CofreAudi2.obj");

	LlantaDD = Model();
	LlantaDD.LoadModel("Models/LlantaDD.obj");

	LlantaID = Model();
	LlantaID.LoadModel("Models/LlantaID.obj");

	LlantaDA = Model();
	LlantaDA.LoadModel("Models/LlantaDA.obj");

	LlantaIA = Model();
	LlantaIA.LoadModel("Models/LlantaIA.obj");

	Sign = Model();
	Sign.LoadModel("Models/Sign.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 1.0f, 1.0f,
		1.5f, 2.2f,
		12.0f, 16.0f, 6.0f,
		0.5f, 0.2f, 0.1f);
	pointLightCount++;

	pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,
		1.5f, 2.2f,
		18.0f, 16.0f, 6.0f,
		0.5f, 0.2f, 0.1f);
	pointLightCount++;

	pointLights[2] = PointLight(0.7f, 0.4f, 2.0f,
		1.5f, 2.2f,
		0.0f, -1.0f, 40.0,
		0.5f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	// Faro del coche (azul)
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		0.0f, 2.8f, 6.0f,
		0.0f, 5.0f, 0.0f,
		0.8f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	// Luz del cofre
	spotLights[2] = SpotLight(
		0.0f, 1.0f, 0.0f,       
		1.0f, 2.0f,             
		0.0f, 0.0f, -10.0f,     
		0.0f, 0.0f, 1.0f,      
		0.5f, 0.1f, 0.01f,      
		15.0f                  
	);
	spotLightCount++;

	// Faro delantero del coche (spotlight frontal)
	spotLights[3] = SpotLight(
		1.0f, 0.7f, 0.4f,       
		5.0f, 2.0f,             
		0.0f, 0.0f, 5.0f,      
		0.0f, 0.0f, -1.0f,       
		0.8f, 0.1f, 0.01f,      
		15.0f
	);
	spotLightCount++;

	// Luz trasera del coche (spotlight trasero)
	spotLights[4] = SpotLight(
		1.0f, 0.0f, 0.0f,       
		5.0f, 2.0f,             
		0.0f, 0.0f, 0.0f,       
		0.0f, 0.0f, 1.0f,       
		0.8f, 0.1f, 0.01f,     
		15.0f              
	);
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	glm::vec3 worldDir;
	glm::vec3 worldPos;
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		
		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
			glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		//Lampara
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(15.0f, -1.0f, 6.0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LamparaM.RenderModel();

		//Señal
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 40.0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Sign.RenderModel();

		// Control de luces
		bool lampLightOn = mainWindow.getIsLampLightOn();  
		bool signalLightOn = mainWindow.getIsLampLightOn2();

		PointLight activeLights[MAX_POINT_LIGHTS];
		int activeLightCount = 0;

		if (lampLightOn) {
			activeLights[activeLightCount++] = pointLights[0];
			activeLights[activeLightCount++] = pointLights[1];
		}

		if (signalLightOn) {
			activeLights[activeLightCount++] = pointLights[2];
		}

		shaderList[0].SetPointLights(activeLights, activeLightCount);

		// Carro Audi

		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(mainWindow.getCarro2(), 12.25f, mainWindow.getCarro1()));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CarroAudi.RenderModel();

		//Luz azul
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.5f, 2.8f, 31.0f));
		worldPos = model[3];
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		worldDir = glm::normalize(glm::vec3(model * glm::vec4(0.0f, -0.2f, -1.0f, 0.0f)));
		spotLights[1].SetFlash(worldPos, worldDir);


		// Cofre

		model = modelaux;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 6.25f, 22.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CofreAudi2.RenderModel();

		// Luz cofre
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		glm::vec3 worldPos = glm::vec3(model[3]);

		// Rotación del cofre
		bool isTrunkBeyondThreshold = (mainWindow.getarticulacion5() < -15.0f);

		if (isTrunkBeyondThreshold) {
			glm::mat4 originalModel = model;
			model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(1.0f, 0.0f, 0.0f));
			glm::vec3 lightDirection = glm::normalize(glm::vec3(model * glm::vec4(0.0f, -0.3f, 1.0f, 0.0f)));
			spotLights[2].SetFlash(worldPos, lightDirection);
		}
		else {
			spotLights[2].SetFlash(glm::vec3(0.0f, -1000.0f, 0.0f), glm::vec3(0.0f));
		}

		// LlantaDD
		model = modelaux;
		model = glm::translate(model, glm::vec3(-25.5f, -5.5f, 10.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaDD.RenderModel();

		// LlantaID
		model = modelaux;
		model = glm::translate(model, glm::vec3(25.5f, -5.5f, 10.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaID.RenderModel();

		// LlantaDA
		model = modelaux;
		model = glm::translate(model, glm::vec3(-25.5f, -5.5f, -67.0));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaDA.RenderModel();

		// LlantaIA
		model = modelaux;
		model = glm::translate(model, glm::vec3(25.5f, -5.5f, -67.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaIA.RenderModel();

		bool isMovingForward = mainWindow.isMovingForward();
		bool isMovingBackward = mainWindow.isMovingBackward();

		// Spotlight frontal (solo activo al retroceder)
		if (isMovingBackward && mainWindow.isFrontLightOn()) {
			model = modelaux;
			model = glm::translate(model, glm::vec3(0.0f, 2.8f, 35.0f));
			worldPos = model[3];
			worldDir = glm::normalize(glm::vec3(model * glm::vec4(0.0f, -0.1f, -1.0f, 0.0f)));
			spotLights[3].SetFlash(worldPos, worldDir);
		}
		else {
			spotLights[3].SetFlash(glm::vec3(0.0f, -1000.0f, 0.0f), glm::vec3(0.0f));
		}

		// Spotlight trasero (solo activo al avanzar)
		if (isMovingForward && mainWindow.isRearLightOn()) {
			model = modelaux;
			model = glm::translate(model, glm::vec3(0.0f, 2.8f, -35.0f));
			worldPos = model[3];
			worldDir = glm::normalize(glm::vec3(model * glm::vec4(0.0f, -0.1f, 1.0f, 0.0f)));
			spotLights[4].SetFlash(worldPos, worldDir);
		}
		else {
			spotLights[4].SetFlash(glm::vec3(0.0f, -1000.0f, 0.0f), glm::vec3(0.0f));
		}

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
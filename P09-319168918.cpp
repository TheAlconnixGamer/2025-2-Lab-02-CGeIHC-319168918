/*
Animación:
Sesión 1:
Simple o básica:Por banderas y condicionales (más de 1 transformación geométrica se ve modificada
Sesión 2
Compleja: Por medio de funciones y algoritmos.
Textura Animada
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

//variables para animación
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza;
float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float angulovaria = 0.0f;

static float tiempoCambio = 0.0f;
static bool mostrarNumero1 = true;
float velocidadCambio = 20.0f;

static float anguloAleteo = 0.0f;
float velocidadAleteo = 0.15f;  
float amplitudAleteo = 10.0f;  


static bool avanzaDragon = true;
static float moveDragonX = 0.0f;
float movOffsetX = 0.5f;
float limiteXSuperior = 300.0f;
float limiteXInferior = -300.0f;

static float breathAnimTime = 0.0f;

// Variables globales para controlar el tiempo
static float tiempo = 0.0f;


Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;

Texture NumerosTexture;
Texture Numero1Texture;
Texture Numero2Texture;

Texture Entrada;

Model Dragon_M;
Model AlaD1;
Model AlaD2;
Model AlaA1;
Model AlaA2;

Model CabezaRoja;
Model CabezaBlanca;
Model CabezaAzul;
Model CabezaCafe;
Model CabezaVerde;

Model CuelloRojo;
Model CuelloBlanco;
Model CuelloAzul;
Model CuelloCafe;
Model CuelloVerde;

Model Puerta;

Model Tiamat_M;
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



//cálculo del promedio de las normales para sombreado de Phong
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
	

	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,			0.0f, -1.0f, 0.0f,

	};

	unsigned int letreroIndices[] = {
	0, 1, 2,
	0, 2, 3
	};

	GLfloat letreroVertices[] = {
	 -0.5f, 0.0f,  0.5f,       0.4225f, 0.0280f,     0.0f, -1.0f, 0.0f,  
	 0.5f, 0.0f,  0.5f,        0.6098f, 0.0280f,     0.0f, -1.0f, 0.0f,  
	 0.5f, 0.0f, -0.5f,        0.6098f, 0.1802f,     0.0f, -1.0f, 0.0f,  
	-0.5f, 0.0f, -0.5f,        0.4225f, 0.1802f,     0.0f, -1.0f, 0.0f  
	};

	GLfloat fuegoVertices[] = {
		// Posición X/Y/Z        // Coord. UV    // Normal
		-0.5f, 0.0f,  0.0f,     0.0f, 0.0f,    0.0f, -1.0f, 0.0f,  // Centro izquierda
		-0.3f, 0.0f,  0.3f,     0.2f, 0.3f,    0.0f, -1.0f, 0.0f,  // Punto intermedio 1
		 0.0f, 0.0f,  0.5f,     0.5f, 1.0f,    0.0f, -1.0f, 0.0f,  // Punta central
		 0.3f, 0.0f,  0.3f,     0.8f, 0.3f,    0.0f, -1.0f, 0.0f,  // Punto intermedio 2
		 0.5f, 0.0f,  0.0f,     1.0f, 0.0f,    0.0f, -1.0f, 0.0f,  // Centro derecha
		 0.3f, 0.0f, -0.3f,     0.8f, -0.3f,   0.0f, -1.0f, 0.0f,  // Punto intermedio 3
		 0.0f, 0.0f, -0.5f,     0.5f, -1.0f,   0.0f, -1.0f, 0.0f,  // Punta trasera
		-0.3f, 0.0f, -0.3f,     0.2f, -0.3f,   0.0f, -1.0f, 0.0f   // Punto intermedio 4
	};

	unsigned int fuegoIndices[] = {
		0, 1, 2,  // Primer triángulo
		0, 2, 3,  // Segundo triángulo
		0, 3, 4,  // ...
		0, 4, 5,
		0, 5, 6,
		0, 6, 7,
		0, 7, 1   // Triángulo final
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

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6);

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7);

	Mesh* objLetrero = new Mesh();
	objLetrero->CreateMesh(letreroVertices, letreroIndices, 32, 6);
	meshList.push_back(objLetrero);

	Mesh* objFuego = new Mesh();
	objFuego->CreateMesh(fuegoVertices, fuegoIndices, 32, 6);
	meshList.push_back(objFuego);

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

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	FlechaTexture = Texture("Textures/flechas.tga");
	FlechaTexture.LoadTextureA();

	NumerosTexture = Texture("Textures/numerosbase.tga");
	NumerosTexture.LoadTextureA();
	Numero1Texture = Texture("Textures/numero1.tga");
	Numero1Texture.LoadTextureA();
	Numero2Texture = Texture("Textures/numero2.tga");
	Numero2Texture.LoadTextureA();

	Entrada = Texture("Textures/Entrada.tga");
	Entrada.LoadTextureA();



	Dragon_M = Model();
	Dragon_M.LoadModel("Models/Dragonbase.obj");
	AlaD1 = Model();
	AlaD1.LoadModel("Models/AlaD1.obj");
	AlaD2 = Model();
	AlaD2.LoadModel("Models/AlaD2.obj");
	AlaA1 = Model();
	AlaA1.LoadModel("Models/AlaA1.obj");
	AlaA2 = Model();
	AlaA2.LoadModel("Models/AlaA2.obj");

	CabezaRoja = Model();
	CabezaRoja.LoadModel("Models/DragonCabezaRojo.obj");
	CabezaBlanca = Model();
	CabezaBlanca.LoadModel("Models/DragonCabezaBlanco.obj");
	CabezaAzul = Model();
	CabezaAzul.LoadModel("Models/DragonCabezaAzul.obj");
	CabezaCafe = Model();
	CabezaCafe.LoadModel("Models/DragonCabezaCafe.obj");
	CabezaVerde = Model();
	CabezaVerde.LoadModel("Models/DragonCabezaVerde.obj");

	CuelloRojo = Model();
	CuelloRojo.LoadModel("Models/DragonCuelloRojo.obj");
	CuelloBlanco = Model();
	CuelloBlanco.LoadModel("Models/DragonCuelloBlanco.obj");
	CuelloAzul = Model();
	CuelloAzul.LoadModel("Models/DragonCuelloAzul.obj");
	CuelloCafe = Model();
	CuelloCafe.LoadModel("Models/DragonCuelloCafe.obj");
	CuelloVerde = Model();
	CuelloVerde.LoadModel("Models/DragonCuelloVerde.obj");

	Puerta = Model();
	Puerta.LoadModel("Models/Puerta.obj");



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

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;



	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset=0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	bool alientoRojo = false;
	bool alientoAzul = false;
	bool alientoVerde = false;
	bool alientoBlanco = false;
	bool alientoCafe = false;
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		angulovaria += 0.5f*deltaTime;
	


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
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);



		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::mat4 modelaux2(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
		
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//Puerta
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		modelaux = model;
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Puerta.RenderModel();


		toffsetnumerocambiau += 0.001;
		if (toffsetnumerocambiau > 1.0)
			toffsetnumerocambiau = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerocambiau, toffsetnumerov);
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 24.0f, 0.45f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.5f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Entrada.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[7]->RenderMesh();


		//Aleteo
		anguloAleteo += deltaTime * velocidadAleteo;
		float movimientoAlas = sin(anguloAleteo) * amplitudAleteo;
		static float angulovaria = 0.0f;
		float velocidadOscilacion = 60.0f; 
		angulovaria += velocidadOscilacion * deltaTime;

		//Adelante y atras

		if (avanzaDragon) {
			if (moveDragonX > limiteXInferior) {
				moveDragonX -= movOffsetX * deltaTime;
			}
			else {
				avanzaDragon = false;
			}
		}
		else {
			if (moveDragonX < limiteXSuperior) {
				moveDragonX += movOffsetX * deltaTime;
			}
			else {
				avanzaDragon = true;
			}
		}

		//Dragon
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(moveDragonX, 5.0f + 7 * sin(glm::radians(0.1 * angulovaria)), 6.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Dragon_M.RenderModel();

		//Ala D1
		model = modelaux;
		model = glm::rotate(model, movimientoAlas * toRadians, glm::vec3(-1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-0.5f, 4.0f, 2.25f));
		modelaux2 = model;
		model = glm::rotate(model, 180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));

		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AlaD1.RenderModel();

		////Ala D2
		model = modelaux2;
		model = glm::rotate(model, movimientoAlas  * toRadians, glm::vec3(-1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-0.665f, 2.85f, 2.95f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AlaD2.RenderModel();

		//Ala A1
		model = modelaux;
		model = glm::rotate(model, -movimientoAlas * toRadians, glm::vec3(-1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-0.5f, 4.0f, -2.25f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));

		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AlaA1.RenderModel();
		modelaux2 = model;


		//Ala A2
		model = modelaux2;
		model = glm::rotate(model, -movimientoAlas * toRadians, glm::vec3(-1.0f,0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-0.665f, -2.85f, 2.95f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AlaA2.RenderModel();

		//Cuellos & Cabezas
		tiempo += 0.01f; // Incrementa el tiempo cada frame

		//Cuello rojo (Movimiento senoidal)
		model = modelaux;
		model = glm::translate(model, glm::vec3(-3.0f, 3.3f + 0.1f * sin(tiempo * 1.2f), -0.2f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuelloRojo.RenderModel();
		modelaux2 = model;


		// Cabeza Roja: Movimiento senoidal
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-1.5f, -0.5f + 0.2f * sin(tiempo * 1.2f), 0.225f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CabezaRoja.RenderModel();

	
		// Cuello Blanco Espiral de Arquímedes
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.7f, 2.75f + 0.05f * cos(tiempo * 1.2f), 0.775f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuelloBlanco.RenderModel();
		modelaux2 = model;

		// Cabeza Blanca Espiral de Arquímedes
		model = modelaux2;
		float radioEspiral = 0.8f + 0.05f * tiempo;
		float anguloEspiral = tiempo * 0.5f;
		model = glm::translate(model, glm::vec3(-1.3f + radioEspiral * cos(anguloEspiral), 0.45f, -0.75f + radioEspiral * sin(anguloEspiral)));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CabezaBlanca.RenderModel();

		//Cuello Azul Lemniscata
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.4f, 4.0f + 0.08f * sin(tiempo * 0.8f), 0.45f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuelloAzul.RenderModel();
		modelaux2 = model;

		// Cabeza Azul Lemniscata
		model = modelaux2;
		float lemniX = 0.25f * sin(tiempo) / (1 + pow(sin(tiempo), 2));
		float lemniZ = 0.25f * sin(tiempo) * cos(tiempo) / (1 + pow(sin(tiempo), 2));
		model = glm::translate(model, glm::vec3(-0.85f + lemniX, -0.95f, -0.7f + lemniZ));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CabezaAzul.RenderModel();

		//Cuello Cafe Círculo
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.95f, 2.77f + 0.07f * cos(tiempo * 1.1f), -1.25f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuelloCafe.RenderModel();
		modelaux2 = model;

		// Cabeza Cafe Círculo
		model = modelaux2;
		float radioCircular = 0.3f;
		model = glm::translate(model, glm::vec3(-1.25f + radioCircular * cos(tiempo * 1.5f), -0.4f, 0.75f + radioCircular * sin(tiempo * 1.5f)));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CabezaCafe.RenderModel();

		//Cuello Verde Figura de 8
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.35f, 4.12f + 0.06f * sin(tiempo * 1.3f), -1.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuelloVerde.RenderModel();
		modelaux2 = model;

		// Cabeza Verde Figura de 8
		model = modelaux2;
		float figura8X = 0.35f * sin(2.0f * tiempo);
		float figura8Z = 0.35f * sin(tiempo);
		model = glm::translate(model, glm::vec3(-0.85f + figura8X, -0.9f, 0.75f + figura8Z));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CabezaVerde.RenderModel();


		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}

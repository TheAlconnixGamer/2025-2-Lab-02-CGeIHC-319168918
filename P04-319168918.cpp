/*Práctica 4: Modelado Jerárquico.
Se implementa el uso de matrices adicionales para almacenar información de transformaciones geométricas que se quiere 
heredar entre diversas instancias para que estén unidas
Teclas de la F a la K para rotaciones de articulaciones
*/
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include <gtc\random.hpp>
//clases para dar orden y limpieza al còdigo
#include"Mesh.h"
#include"Shader.h"
#include"Sphere.h"
#include"Window.h"
#include"Camera.h"
//tecla E: Rotar sobre el eje X
//tecla R: Rotar sobre el eje Y
//tecla T: Rotar sobre el eje Z
using std::vector;
//Dimensiones de la ventana
const float toRadians = 3.14159265f/180.0; //grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

// Pirámide triangular regular
void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
			0,1,2,
			1,3,2,
			3,0,2,
			1,0,3

	};
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3

	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);

}
/*
Crear cilindro y cono con arreglos dinámicos vector creados en el Semestre 2023 - 1 : por Sánchez Pérez Omar Alejandro
*/
void CrearCilindro(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//ciclo for para crear los vértices de las paredes del cilindro
	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		//caso para terminar el círculo
		else {
			x = R * cos((0)*dt);
			z = R * sin((0)*dt);
		}
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			case 3:
				vertices.push_back(x);
				break;
			case 4:
				vertices.push_back(0.5);
				break;
			case 5:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia inferior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(-0.5f);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia superior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(0.5);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//Se generan los indices de los vértices
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);

	//se genera el mesh del cilindro
	Mesh *cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

//función para crear un cono
void CrearCono(int res,float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;
	
	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//caso inicial para crear el cono
	vertices.push_back(0.0);
	vertices.push_back(0.5);
	vertices.push_back(0.0);
	
	//ciclo for para crear los vértices de la circunferencia del cono
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);


	for (i = 0; i < res+2; i++) indices.push_back(i);

	//se genera el mesh del cono
	Mesh *cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}

//función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,4

	};
	vector<GLfloat> piramidecuadrangular_vertices = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh *piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}

void CrearPrismaRectangular()
{
	vector<unsigned int> prisma_indices = {
		
		 0, 1, 5,
		 5, 4, 0,

		 0, 3, 7,
		 7, 4, 0,

		 3, 2, 6,
		 6, 7, 3,

		 1, 2, 6,
		 6, 5, 1,

		 4, 5, 6,
		 6, 7, 4,

		 0, 1, 2,
		 2, 3, 0
	};

	vector<GLfloat> prisma_vertices = {
		 0.5f, -0.25f,  1.0f,  
		-0.5f, -0.25f,  1.0f,  
		-0.5f, -0.25f, -1.0f,  
		 0.5f, -0.25f, -1.0f,  

		  0.5f,  0.25f,  1.0f,  
		 -0.5f,  0.25f,  1.0f,  
		 -0.5f,  0.25f, -1.0f,  
		  0.5f,  0.25f, -1.0f   
	};

	Mesh* prisma = new Mesh();
	prisma->CreateMeshGeometry(prisma_vertices, prisma_indices, 24, 36);
	meshList.push_back(prisma);
}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

}


int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();
	//Cilindro y cono reciben resolución (slices, rebanadas) y Radio de circunferencia de la base y tapa

	CrearCubo();//índice 0 en MeshList
	CrearPiramideTriangular();//índice 1 en MeshList
	CrearCilindro(5, 1.0f);//índice 2 en MeshList
	CrearCono(25, 2.0f);//índice 3 en MeshList
	CrearPiramideCuadrangular();//índice 4 en MeshList
	CrearPrismaRectangular();
	CreateShaders();
	
	

	/*Cámara se usa el comando: glm::lookAt(vector de posición, vector de orientación, vector up));
	En la clase Camera se reciben 5 datos:
	glm::vec3 vector de posición,
	glm::vec3 vector up,
	GlFloat yaw rotación para girar hacia la derecha e izquierda
	GlFloat pitch rotación para inclinar hacia arriba y abajo
	GlFloat velocidad de desplazamiento,
	GlFloat velocidad de vuelta o de giro
	Se usa el Mouse y las teclas WASD y su posición inicial está en 0,0,1 y ve hacia 0,0,-1.
	*/
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.2f, 0.2f);
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f)	,mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	//glm::mat4 projection = glm::ortho(-1, 1, -1, 1, 1, 10);
	
	//Loop mientras no se cierra la ventana
	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader

	glm::mat4 model(1.0);//Inicializar matriz de Modelo 4x4
	glm::mat4 modelaux(1.0);//Inicializar matriz de Modelo 4x4 auxiliar para la jerarquía
	glm::mat4 modelbase(1.0);
	glm::mat4 modelA1(1.0);
	glm::mat4 modelA2(1.0);
	glm::mat4 modelA3(1.0);
	glm::mat4 modelA4(1.0);
	glm::mat4 modelA5(1.0);


	glm::vec3 color = glm::vec3(0.0f,0.0f,0.0f); //inicializar Color para enviar a variable Uniform;

	while (!mainWindow.getShouldClose())
	{

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		//Recibir eventos del usuario
		glfwPollEvents();
		//Cámara
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Limpiar la ventana
		/*glClearColor(1.0f, 1.0f, 1.0f, 1.0f);*/ // Para la grua
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Para la mascota
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Se agrega limpiar el buffer de profundidad
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();


		//// Creando el brazo de una grúa
		////articulacion1 hasta articulación5 sólo son puntos de rotación o articulación, en este caso no dibujaremos esferas que los representen

		//////para reiniciar la matriz de modelo con valor de la matriz identidad
		//model = glm::mat4(1.0);
		////AQUÍ SE DIBUJA LA CABINA, LA BASE, LAS 4 LLANTAS

		////Cabina

		//color = glm::vec3(0.8f, 0.8f, 0.8f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 4.0f));
		//model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		//modelaux = model;
		//modelbase = model;

		//meshList[0]->RenderMesh();
		//
		//// Base
		//model = modelbase;
		//model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f));
		//modelbase = model;
		//model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));

		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		//glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		//color = glm::vec3(1.0f, 1.0f, 0.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//model = modelbase;

		//meshList[4]->RenderMesh();

		////Ruedas delanteras
		//
		////Izquierda 
		//color = glm::vec3(0.0f, 0.0f, 0.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		//model = modelbase;
		//model = glm::translate(model, glm::vec3(-1.2f, -1.9f, -1.0f));
		//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(0.0f, 1.0f, 0.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		//glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));


		//meshList[2]->RenderMeshGeometry();

		////Derecha

		//model = modelbase;
		//model = glm::translate(model, glm::vec3(-1.2f, -1.9f, 1.0f));
		//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		//glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		//meshList[2]->RenderMeshGeometry();

		////Ruedas traseras

		////Izquierda

		//model = modelbase;
		//model = glm::translate(model, glm::vec3(1.2f, -1.9f, -1.0f));
		//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0.0f, 1.0f, 0.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		//glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//meshList[2]->RenderMeshGeometry();

		////Derecha

		//model = modelbase;
		//model = glm::translate(model, glm::vec3(1.2f, -1.9f, 1.0f));
		//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(0.0f, 1.0f, 0.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		//glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//meshList[2]->RenderMeshGeometry();


		//// SE EMPIEZA EL DIBUJO DEL BRAZO
		////articulación 1
		//model = modelaux;
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		////rotación alrededor de la articulación que une con la cabina
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f));
		//modelaux = model;
		//
		////primer brazo que conecta con la cabina
		//// //Traslación inicial para posicionar en -Z a los objetos
		////model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
		////otras transformaciones para el objeto
		//model = glm::translate(model, glm::vec3(-1.0f, 2.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		//modelaux = model;
		//model = glm::scale(model, glm::vec3(5.0f, 1.0f, 1.0f));
		//
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));		
		////la línea de proyección solo se manda una vez a menos que en tiempo de ejecución
		////se programe cambio entre proyección ortogonal y perspectiva
		//glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		//glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//color = glm::vec3(1.0f, 1.0f, 0.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		//meshList[0]->RenderMesh(); //dibuja cubo, pirámide triangular, pirámide base cuadrangular
		////meshList[2]->RenderMeshGeometry(); //dibuja las figuras geométricas cilindro y cono

		////para descartar la escala que no quiero heredar se carga la información de la matrix auxiliar
		//model = modelaux;
		////articulación 2
		//model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, -1.0f));
		//modelaux = model;
		////dibujar una pequeña esfera
		//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(0.3f, 0.3f, 0.3f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); 
		//sp.render();

		//model = modelaux;
		////segundo brazo
		//model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));

		//modelaux = model;
		//model = glm::scale(model, glm::vec3(1.0f, 5.0f, 1.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(1.0f, 1.0f, 0.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		//meshList[0]->RenderMesh(); //dibuja cubo y pirámide triangular

		//model = modelaux;

		////articulación 3 extremo derecho del segundo brazo
		//model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
		//modelaux = model;


		////dibujar una pequeña esfera
		//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(0.3f, 0.3f, 0.3f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		//sp.render();

		//// Crear instancias para completar el brazo y  la cabina. Imporante considerar que la cabina es el nodo padre. 
		////La cabina y el brazo deben de estar unidos a la cabina 
		//

		////tercer brazo
		//model = modelaux;
		//model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));

		//modelaux = model;
		//model = glm::scale(model, glm::vec3(1.0f, 5.0f, 1.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(1.0f, 1.0f, 0.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		//meshList[0]->RenderMesh(); //dibuja cubo y pirámide triangular

		//model = modelaux;

		////articulación 3
		//
		//model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(1.0f, 0.0f, 0.0f));
		//modelaux = model;
		//
		////Cabina superior
		//model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f));

		//modelaux = model;
		//model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(1.0f, 1.0f, 0.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//meshList[0]->RenderMesh(); 

		//Mascota
		//Nucleo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 4.0f));
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		modelaux = model;
		modelbase = model;
		//Cabeza
		color = glm::vec3(0.8f, 0.8f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//AA1
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		//Cuello
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(2.0f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));		
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		meshList[0]->RenderMesh(); 
		//AA2
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.2f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 0.0f, -1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();
		model = modelaux;
		//Cabeza Superior
		model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f));
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 
		modelaux = model;
		//O1.1
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.2f, -0.625f, -0.2f));
		model = glm::scale(model, glm::vec3(0.1f, 0.05f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();
		//O2.1
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.2f, -0.625f, 0.2f));
		model = glm::scale(model, glm::vec3(0.1f, 0.05f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();
		//Oreja 1.1
		color = glm::vec3(0.7f, 0.7f, 0.7f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.6f, -0.2f, -0.5f));
		model = glm::scale(model, glm::vec3(0.175f, 0.175f, 0.175f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();
		//Oreja 2.1
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.6f, -0.2f, 0.5f));
		model = glm::scale(model, glm::vec3(0.175f, 0.175f, 0.175f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();
		//O1
		color = glm::vec3(0.4f, 0.8f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.2f, -0.5f, -0.2f));
		model = glm::scale(model, glm::vec3(0.2f, 0.15f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();
		//O2
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.2f, -0.5f, 0.2f));
		model = glm::scale(model, glm::vec3(0.2f, 0.15f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();
		//Oreja 1
		color = glm::vec3(0.6f, 0.6f, 0.6f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.6f, -0.1f, -0.5f));
		model = glm::scale(model, glm::vec3(0.2f, 0.6f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[3]->RenderMeshGeometry();
		//Oreja 2
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.6f, -0.1f, 0.5f));
		model = glm::scale(model, glm::vec3(0.2f, 0.6f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[3]->RenderMeshGeometry();
		//Boca

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.2f, -0.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f, 0.35f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		//Nariz
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		color = glm::vec3(0.8f, 0.8f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.2f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f, 0.35f, 0.25f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[4]->RenderMesh();

		//Base
		model = modelbase;
		model = glm::translate(model, glm::vec3(1.1f, -0.1f, 0.0f));
		modelbase = model;
		model = glm::scale(model, glm::vec3(4.0f, 3.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = modelbase;
		meshList[5]->RenderMesh();

		//Patas delanteras

		//A1

		model = modelbase;
		model = glm::translate(model, glm::vec3(-1.5f, -0.9f, -0.8f));
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelA1 = modelbase;

		//Izquierda 
		modelA1 = model;
		modelA1 = glm::translate(modelA1, glm::vec3(0.0f, -0.2f, 0.0f));
		modelA1 = glm::scale(modelA1, glm::vec3(0.8f, 2.0f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelA1));
		meshList[0]->RenderMesh();

		modelA1 = model;

		//A1.1
		model = modelA1;
		modelA1 = glm::translate(modelA1, glm::vec3(0.0f, -1.5f, 0.0f));
		modelA1 = glm::rotate(modelA1, glm::radians(mainWindow.getarticulacion7()), glm::vec3(0.0f, 0.0f, 1.0f));

		//P1.1
		modelA1 = glm::translate(modelA1, glm::vec3(0.0f, -0.1f, 0.0f));
		model = modelA1;
		modelA1 = glm::scale(modelA1, glm::vec3(0.8f, 1.0f, 0.8f));
		modelA1 = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelA1));
		meshList[2]->RenderMeshGeometry();

		//A2
		model = modelbase;
		model = glm::translate(model, glm::vec3(-1.5f, -0.9f, 0.8f));
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelA2 = modelbase;

		//Derecha

		modelA2 = model;
		modelA2 = glm::translate(modelA2, glm::vec3(0.0f, -0.2f, 0.0f));
		modelA2 = glm::scale(modelA2, glm::vec3(0.8f, 2.0f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelA2));
		meshList[0]->RenderMesh();

		modelA2 = model;

		//A1.2
		model = modelA2;
		modelA2 = glm::translate(modelA2, glm::vec3(0.0f, -1.5f, 0.0f));
		modelA2 = glm::rotate(modelA2, glm::radians(mainWindow.getarticulacion8()), glm::vec3(0.0f, 0.0f, 1.0f));

		//P2.1
		modelA2 = glm::translate(modelA2, glm::vec3(0.0f, -0.1f, 0.0f));
		model = modelA2;
		modelA2 = glm::scale(modelA2, glm::vec3(0.8f, 1.0f, 0.8f));
		modelA2 = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelA2));
		meshList[2]->RenderMeshGeometry();

		//Patas traseras

		//A3
		model = modelbase;
		model = glm::translate(model, glm::vec3(1.5f, -0.9f, -0.8f));
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelA3 = modelbase;

		//Izquierda

		modelA3 = model;
		modelA3 = glm::translate(modelA3, glm::vec3(0.0f, -0.2f, 0.0f));
		modelA3 = glm::scale(modelA3, glm::vec3(0.8f, 2.0f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelA3));
		meshList[0]->RenderMesh();

		modelA3 = model;


		//A1.3
		model = modelA3;
		modelA3 = glm::translate(modelA3, glm::vec3(0.0f, -1.5f, 0.0f));
		modelA3 = glm::rotate(modelA3, glm::radians(mainWindow.getarticulacion9()), glm::vec3(0.0f, 0.0f, 1.0f));

		//P3.1
		modelA3 = glm::translate(modelA3, glm::vec3(0.0f, -0.1f, 0.0f));
		model = modelA3;
		modelA3 = glm::scale(modelA3, glm::vec3(0.8f, 1.0f, 0.8f));
		modelA3 = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelA3));
		meshList[2]->RenderMeshGeometry();

		//A4
		model = modelbase;
		model = glm::translate(model, glm::vec3(1.5f, -0.9f, 0.8f));
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelA4 = modelbase;
	
		//Derecha

		modelA4 = model;
		modelA4 = glm::translate(modelA4, glm::vec3(0.0f, -0.2f, 0.0f));
		modelA4 = glm::scale(modelA4, glm::vec3(0.8f, 2.0f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelA4));
		meshList[0]->RenderMesh();

		modelA4 = model;

		//A1.4
		model = modelA4;
		modelA4 = glm::translate(modelA4, glm::vec3(0.0f, -1.5f, 0.0f));
		modelA4 = glm::rotate(modelA4, glm::radians(mainWindow.getarticulacion10()), glm::vec3(0.0f, 0.0f, 1.0f));

		//P4.1
		modelA4 = glm::translate(modelA4, glm::vec3(0.0f, -0.1f, 0.0f));
		model = modelA4;
		modelA4 = glm::scale(modelA4, glm::vec3(0.8f, 1.0f, 0.8f));
		modelA4 = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelA4));
		meshList[2]->RenderMeshGeometry();

		//Cola
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		model = modelbase;
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.75f, 0.4f, 0.4f));
		modelA5 = modelbase;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();
		modelA5 = model;
		//A5

		model = modelA5;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0.0f, 0.0f, 1.0f)); 
		modelA5 = model;

		//Cola Next
		modelA5 = glm::translate(modelA5, glm::vec3(1.0f, 0.0f, 0.0f));
		model = modelA5;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelA5));
		meshList[0]->RenderMesh();
		modelA5 = model;

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}

	
		
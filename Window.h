#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	GLfloat getCarro1() { return carro1; }
	GLfloat getCarro2() { return carro2; }
	GLfloat LightIntensity() { return lightIntensity; }
	GLfloat getarticulacion1() { return articulacion1; }
	GLfloat getarticulacion2() { return articulacion2; }
	GLfloat getarticulacion3() { return articulacion3; }
	GLfloat getarticulacion4() { return articulacion4; }
	GLfloat getarticulacion5() { return articulacion5; }
	void setMovingForward(bool state) { movingForward = state; }
	void setMovingBackward(bool state) { movingBackward = state; }
	bool isMovingForward() const { return movingForward; }
	bool isMovingBackward() const { return movingBackward; }
	void toggleFrontLight() { frontLightOn = !frontLightOn; }
	void toggleRearLight() { rearLightOn = !rearLightOn; }
	bool isFrontLightOn() const { return frontLightOn; }
	bool isRearLightOn() const { return rearLightOn; }
	bool isManualControl() const { return manualLightControl; }
	void setAutoLightMode() { manualLightControl = false; }

	bool getIsLampLightOn() const { return isLampLightOn; }
	bool getIsLampLightOn2() const { return isLampLightOn2; }

	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	
	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	bool isLampLightOn = true;
	bool isLampLightOn2 = true;
	bool FrontLight = true;
	bool RearLight = true;
	bool movingForward;
	bool movingBackward;
	bool frontLightOn = false;
	bool rearLightOn = false;
	bool manualLightControl = false; 
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex, carro1, carro2, articulacion1, articulacion2, articulacion3, articulacion4, articulacion5, helicop1, helicop2, lightIntensity;
	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};


#include <Windows.h>
#include <codecvt>

#include <stdlib.h> 
#include <stdio.h>
#include <math.h> 

#include <GL/glew.h>

#include <GLM.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <glfw3.h>

#include"Model.h"
#include"Shader.h"
#include"Camera.h"

#include <iostream>

#pragma comment (lib, "glfw3dll.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "OpenGL32.lib")

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

std::string currentPath;

glm::vec3 lightPos(0.0f, 0.0f, 10.0f);

GLuint cubeVAO, lightVAO, VBO;
GLuint VertexShaderId, FragmentShaderId, ProgramId;
GLuint ProjMatrixLocation, ViewMatrixLocation, WorldMatrixLocation;
GLuint VAOID, VBOID, ColorBufferID, IBOID;
Camera* pCamera = nullptr;

float g_fKA = 0.2f;
float g_fKD = 0.5f;
float g_fKS = 0.5f;

Shader ShaderProgram;
Shader lampShader;
Shader objShader;

std::vector<Model> models;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

double deltaTime = 0.0f;
double lastFrame = 0.0f;

void CreateVBO()
{
	// indexurile cubului
	unsigned int Indices[] = {
	 0,1,2,
	 0,2,3,
	 1,5,6,
	 1,6,2,
	 5,4,7,
	 5,7,6,
	 4,0,3,
	 4,3,7,
	 0,5,1,
	 0,4,5,
	 3,2,6,
	 3,6,7
	};

	// varfurile cubului
	GLfloat Vertices[] = {
	 0.0f, 0.0f, 1.0f,
	 1.0f, 0.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	 0.0f, 1.0f, 1.0f,
	 0.0f, 0.0f, 0.0f,
	 1.0f, 0.0f, 0.0f,
	 1.0f, 1.0f, 0.0f,
	 0.0f, 1.0f, 0.0f
	};
	// culorile, ca atribute ale varfurilor
	GLfloat Colors[] = {
	 1.0f, 0.0f, 0.0f,
	 0.0f, 1.0f, 0.0f,
	 0.0f, 0.0f, 1.0f,
	 1.0f, 0.0f, 0.0f,
	 0.0f, 1.0f, 0.0f,
	 0.0f, 0.0f, 1.0f,
	 1.0f, 0.0f, 0.0f,
	 0.0f, 1.0f, 0.0f
	};

	// se creeaza un buffer nou
	glGenBuffers(1, &VBOID);
	// este setat ca buffer curent
	glBindBuffer(GL_ARRAY_BUFFER, VBOID);
	// punctele sunt "copiate" in bufferul curent
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	// se creeaza / se leaga un VAO (Vertex Array Object) - util cand se utilizeaza mai multe VBO
	glGenVertexArrays(1, &VAOID);
	glBindVertexArray(VAOID);

	// se activeaza lucrul cu atribute; atributul 0 = pozitie
	glEnableVertexAttribArray(0);
	//
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// un nou buffer, pentru culoare
	glGenBuffers(1, &ColorBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
	// atributul 1 =  culoare
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// un nou buffer pentru indexuri
	glGenBuffers(1, &IBOID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void DestroyVBO()
{
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &ColorBufferID);
	glDeleteBuffers(1, &VBOID);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAOID);
}

void RenderCube()
{
	glBindVertexArray(VAOID);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBOID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferID);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOID);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
}

void RenderFrame()
{
	double currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	float lightSpeed = currentFrame * 1.f;
	float lightRadius = 2.f; // distanta 

	lightPos.x = lightRadius * glm::sin(glm::radians(lightSpeed));
	lightPos.y = lightRadius * glm::sin(glm::radians(lightSpeed));
	lightPos.z = lightRadius * glm::cos(glm::radians(lightSpeed));

	glm::vec3 cubePositions[] = {
	 glm::vec3(0.0f,  0.0f,   0.0f),
	 glm::vec3(-5.0f,  5.0f,  5.0f),
	 glm::vec3(-5.0f, -5.0f,  5.0f),
	 glm::vec3(5.0f, -5.0f,  5.0f),
	 glm::vec3(5.0f,  5.0f,  5.0f),
	 glm::vec3(-5.0f,  5.0f, -5.0f),
	 glm::vec3(-5.0f, -5.0f, -5.0f),
	 glm::vec3(5.0f, -5.0f, -5.0f),
	 glm::vec3(5.0f,  5.0f, -5.0f),
	};

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ShaderProgram.Use();
	ShaderProgram.Use();
	ShaderProgram.SetVec3("objectColor", 0.5f, 1.0f, 0.31f);
	ShaderProgram.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);
	ShaderProgram.SetVec3("lightPos", lightPos);
	ShaderProgram.SetFloat("KA", g_fKA);
	ShaderProgram.SetFloat("KD", g_fKD);
	ShaderProgram.SetFloat("KS", g_fKS);

	ShaderProgram.SetVec3("viewPos", pCamera->GetPosition());

	ShaderProgram.SetMat4("projection", pCamera->GetProjectionMatrix());
	ShaderProgram.SetMat4("view", pCamera->GetViewMatrix());

	glm::mat4 model = glm::scale(glm::mat4(1.0), glm::vec3(1.0f));
	ShaderProgram.SetMat4("model", model);

	glm::mat4 projection = pCamera->GetProjectionMatrix();
	ShaderProgram.SetMat4("projection", projection);

	//astea se comenteaza si se invarte
	glm::mat4 view = pCamera->GetViewMatrix();
	ShaderProgram.SetMat4("view", view);

	//asta face invartirea
	/*glm::mat4 view;
	float radius = 10.0f;
	float camX = (float)sin(glfwGetTime()) * radius;
	float camZ = (float)cos(glfwGetTime()) * radius;
	view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(ViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(view));*/

	for (unsigned int i = 0; i < sizeof(cubePositions) / sizeof(cubePositions[0]); i++) {
		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 worldTransf = glm::translate(glm::mat4(1.0), cubePositions[i]);
		ShaderProgram.SetMat4("WorldMatrix", worldTransf);

		RenderCube();
	}

	//Here we render the models

	objShader.Use();
	objShader.SetVec3("objectColor", 0.5f, 1.0f, 0.31f);
	objShader.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);
	objShader.SetVec3("lightPos", lightPos);
	objShader.SetFloat("KA", g_fKA);
	objShader.SetFloat("KD", g_fKD);
	objShader.SetFloat("KS", g_fKS);
	objShader.SetInt("texture_diffuse1", 0);

	objShader.SetVec3("viewPos", pCamera->GetPosition());
	objShader.SetMat4("projection", pCamera->GetProjectionMatrix());
	objShader.SetMat4("view", pCamera->GetViewMatrix());

	//rendering a pirate
	glm::mat4 pirateModel = glm::mat4(1.0);
	pirateModel = glm::translate(pirateModel, glm::vec3(2.f, -1.f, 0.f));
	pirateModel = glm::scale(pirateModel, glm::vec3(.5f));
	objShader.SetMat4("model", pirateModel);
	models[0].Draw(objShader);

	//Here we render the light source

	lampShader.Use();
	lampShader.SetMat4("projection", pCamera->GetProjectionMatrix());
	lampShader.SetMat4("view", pCamera->GetViewMatrix());
	model = glm::translate(glm::mat4(1.0), lightPos);
	model = glm::scale(model, glm::vec3(0.05f)); // a smaller cube
	lampShader.SetMat4("model", model);

	glBindVertexArray(lightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Initialize()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // culoarea de fond a ecranului
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	ShaderProgram.Create("PhongLight.vs", "PhongLight.fs");
	CreateVBO();

	// Create camera
	pCamera = new Camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.5, 0.5, 10));
}


void Cleanup()
{
	delete pCamera;

	DestroyVBO();
}

void processInput(GLFWwindow* window);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	wchar_t buffer[MAX_PATH];

	GetCurrentDirectoryW(MAX_PATH, buffer);

	std::wstring executablePath(buffer);
	std::wstring wscurrentPath = executablePath.substr(0, executablePath.find_last_of(L"\\/"));

	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	currentPath = converter.to_bytes(wscurrentPath);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lab 7", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewInit();

	glEnable(GL_DEPTH_TEST);

	CreateVBO();

	ShaderProgram.Create("PhongLight.vs", "PhongLight.fs");
	lampShader.Create("Lamp.vs", "Lamp.fs");
	objShader.Create("PhongLightWithTexture.vs", "PhongLightWithTexture.fs");

	pCamera = new Camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0, 0.0, 3.0));

	//Models loading

	std::string PiratePath = currentPath + "\\Models\\Pirat\\Pirat.obj";
	models.emplace_back(PiratePath, false);

	while (!glfwWindowShouldClose(window)) {
		//double currentFrame = glfwGetTime();
		//deltaTime = currentFrame - lastFrame;
		//lastFrame = currentFrame;

		processInput(window);
  //  
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		//float lightSpeed = currentFrame * 30.f;
		//float lightRadius = 1.f; // distanta 

		//lightPos.x = lightRadius * glm::sin(glm::radians(lightSpeed));
		//lightPos.y = lightRadius * glm::sin(glm::radians(lightSpeed));
		//lightPos.z = lightRadius * glm::cos(glm::radians(lightSpeed));

		//
		//lightingShader.Use();
		//lightingShader.SetVec3("objectColor", 0.5f, 1.0f, 0.31f);
		//lightingShader.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);
		//lightingShader.SetVec3("lightPos", lightPos);
		//lightingShader.SetFloat("KA", g_fKA);
		//lightingShader.SetFloat("KD", g_fKD);
		//lightingShader.SetFloat("KS", g_fKS);
		//
		//lightingShader.SetVec3("viewPos", pCamera->GetPosition());
		//lightingShader.SetMat4("projection", pCamera->GetProjectionMatrix());
		//lightingShader.SetMat4("view", pCamera->GetViewMatrix());

		//glm::mat4 model = glm::scale(glm::mat4(1.0), glm::vec3(1.0f));
		//lightingShader.SetMat4("model", model);

		//RenderCube();

		//lampShader.Use();
		//lampShader.SetMat4("projection", pCamera->GetProjectionMatrix());
		//lampShader.SetMat4("view", pCamera->GetViewMatrix());
		//model = glm::translate(glm::mat4(1.0), lightPos);
		//model = glm::scale(model, glm::vec3(0.05f)); // a smaller cube
		//lampShader.SetMat4("model", model);

		/*glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);*/

		RenderFrame();
    
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Cleanup();

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	pCamera->Reshape(width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	pCamera->MouseControl((float)xpos, (float)ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yOffset)
{
	pCamera->ProcessMouseScroll((float)yOffset);
}


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		pCamera->ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		pCamera->ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		pCamera->ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		pCamera->ProcessKeyboard(RIGHT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
		pCamera->ProcessKeyboard(UP, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
		pCamera->ProcessKeyboard(DOWN, (float)deltaTime);

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		pCamera->Reset(width, height);

	}
}


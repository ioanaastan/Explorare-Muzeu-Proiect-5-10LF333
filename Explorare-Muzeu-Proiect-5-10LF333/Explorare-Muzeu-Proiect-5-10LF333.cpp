/*
TUTORIAL PLASARE OBIECTE:

1. te duci in main si adaugi un nou model in vectorul de modele, ai acolo continuarea la tutorial cu cum se face asta

	//Models loading

	//aici am initializat un model, in cazul asta e un pirat, dar poti sa adaugi oricate modele vrei, doar sa ai fisierele alea in folderul Models, fiecare model sa aiba un folder al lui cu denumire intuitiva
	//va rog sa respectati formatul cu \\, altfel nu o sa mearga
	std::string PiratePath = currentPath + "\\Models\\Pirat\\Pirat.obj";
	
	//aici adaugi modelul in vectorul de modele, in cazul asta e models, dar poti sa il numesti cum vrei
	//primul parametru e calea catre model, al doilea e mereu false, simplu
	models.emplace_back(PiratePath, false);


2. in functia RenderFrame() adaugi un nou apel de Draw pentru modelul adaugat, din nou, continuarea e acolo

	//rendering a pirate

	//pentru toate modelele exista intai linia asta ca sa ii setezi pozitie, rotatie, scalare si din astea, aici initializezi pozitia lui
	glm::mat4 pirateModel = glm::mat4(1.0);

	//cu translate ii setezi pozitia, translate returneaza matrice in 4 dimensiuni cu chestiile noi, tot ce e de stiut e ca ii dai intai variabila pe care vrei sa o modifice si apoi vectorul cu coordonatele
	//coordonatele alea reprezinta x,y,z, unde x = stanga-, dreapta+, y = jos-, sus+, z = fata-, spate+
	pirateModel = glm::translate(pirateModel, glm::vec3(2.f, -1.f, 0.f));
	
	//scale e pentru scalare, adica cat de mare vrei sa fie modelul, in cazul asta e 0.5, adica jumatate din marimea originala
	pirateModel = glm::scale(pirateModel, glm::vec3(.5f));
	
	//aici ii setezi modelul, spui la shader ce urmeaza sa deseneze, in cazul asta e primul model din vectorul de modele
	objShader.SetMat4("model", pirateModel);
	
	//aici desenezi modelul, destul de bine explicat sper
	models[0].Draw(objShader);


3. daca este nevoie faci un folder de modele de un anumit tip pentru a le pastra separat. Ex:(Models_Objects, Models_Characters, Models_Fences, etc)

	//Asa trebuie initializat un vector de modele, destul de basic
	std::vector<Model> models;
*/

//to not forget, walk animation made with sin function and a timer that goes while moving only
//make sun go orange and red the closer it gets to 0 and 180 degrees but white-yellow-ish after 5-10 degrees

#include <Windows.h>
#include <codecvt>

#include <stdlib.h> 
#include <stdio.h>
#include <math.h> 
#include <stb_image.h>


#include <GL/glew.h>

#include <GLM.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <glfw3.h>

#include"Model.h"
#include"Shader.h"
#include"Camera.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

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

bool isCursorVisible = false;

Shader ShaderProgram;
Shader lampShader;
Shader objShader;

//Asa trebuie initializat un vector de modele, destul de basic
std::vector<Model> models;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

double deltaTime = 0.0f;
double lastFrame = 0.0f;


//GLuint skyboxVAO, skyboxVBO;
//Shader skyboxShader;
//CubemapTexture skybox;
//
//void InitSkybox() {
//
//	float skyboxVertices[] = {
//		-1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f,
//		1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f, -1.0f,
//		-1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
//		1.0f, -1.0f,  1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,
//		-1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
//		1.0f,  1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
//	};
//
//
//	glGenVertexArrays(1, &skyboxVAO);
//	glGenBuffers(1, &skyboxVBO);
//
//	glBindVertexArray(skyboxVAO);
//
//	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
//
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(0);
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	glBindVertexArray(0);
//}
//
//void RenderSkybox() {
//	glBindVertexArray(skyboxVAO);
//	glDrawArrays(GL_TRIANGLES, 0, 36);
//	glBindVertexArray(0);
//}
//
//class CubemapTexture {
//public:
//	CubemapTexture(const std::string& Directory,
//		const std::string& PosXFilename,
//		const std::string& NegXFilename,
//		const std::string& PosYFilename,
//		const std::string& NegYFilename,
//		const std::string& PosZFilename,
//		const std::string& NegZFilename) {
//		m_fileNames[0] = Directory + "\\Models\\Skybox\\Box_Right";
//		m_fileNames[1] = Directory + "\\Models\\Skybox\\Box_Left";
//		m_fileNames[2] = Directory + "\\Models\\Skybox\\Box_Top";
//		m_fileNames[3] = Directory + "\\Models\\Skybox\\Box_Bottom";
//		m_fileNames[4] = Directory + "\\Models\\Skybox\\Box_Back";
//		m_fileNames[5] = Directory + "\\Models\\Skybox\\Box_Front";
//	}
//
//	~CubemapTexture() {
//		glDeleteTextures(1, &m_textureObj);
//	}
//
//	bool Load() {
//		glGenTextures(1, &m_textureObj);
//		glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureObj);
//
//		for (unsigned int i = 0; i < 6; i++) {
//			int width, height, channels;
//			unsigned char* data = stbi_load(m_fileNames[i].c_str(), &width, &height, &channels, STBI_rgb_alpha);
//
//			if (data == nullptr) {
//				std::cout << "Error loading texture '" << m_fileNames[i] << "': " << stbi_failure_reason() << std::endl;
//				return false;
//			}
//
//			glTexImage2D(types[i], 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//			stbi_image_free(data);
//		}
//
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//
//		return true;
//	}
//	void Bind(GLenum TextureUnit) {
//		glActiveTexture(TextureUnit);
//		glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureObj);
//	}
//
//private:
//	GLuint m_textureObj;
//	std::string m_fileNames[6];
//	GLenum types[6] = {
//		GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
//		GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
//		GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
//	};
//};
//

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
	glClearColor(0.5f, 0.7f, 1.0f, 1.0f); // Light blue background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	double currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	time_t now = time(NULL);
	tm localTime;
	localtime_s(&localTime, &now);

	float lightSpeed = (localTime.tm_sec + (localTime.tm_hour * 60 + localTime.tm_min) * 60) * .0416f * 0.048f;
	float lightRadius = 10.f; // distanta 

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

	//for (unsigned int i = 0; i < sizeof(cubePositions) / sizeof(cubePositions[0]); i++) {
	//	// calculate the model matrix for each object and pass it to shader before drawing
	//	glm::mat4 worldTransf = glm::translate(glm::mat4(1.0), cubePositions[i]);
	//	ShaderProgram.SetMat4("WorldMatrix", worldTransf);

	//	RenderCube();
	//}

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
	//pentru toate modelele exista intai linia asta ca sa ii setezi pozitie, rotatie, scalare si din astea, aici initializezi pozitia lui
	glm::mat4 pirateModel = glm::mat4(1.0);
	//cu translate ii setezi pozitia, translate returneaza matrice in 4 dimensiuni cu chestiile noi, tot ce e de stiut e ca ii dai intai variabila pe care vrei sa o modifice si apoi vectorul cu coordonatele
	//coordonatele alea reprezinta x,y,z, unde x = stanga-, dreapta+, y = jos-, sus+, z = fata-, spate+
	pirateModel = glm::translate(pirateModel, glm::vec3(2.f, -1.f, 0.f));
	//scale e pentru scalare, adica cat de mare vrei sa fie modelul, in cazul asta e 0.5, adica jumatate din marimea originala
	pirateModel = glm::scale(pirateModel, glm::vec3(.5f));
	//aici ii setezi modelul, spui la shader ce urmeaza sa deseneze, in cazul asta e primul model din vectorul de modele
	objShader.SetMat4("model", pirateModel);
	//aici desenezi modelul, destul de bine explicat sper
	models[0].Draw(objShader);

	glm::mat4 giraffeModel = glm::mat4(1.0);
	giraffeModel = glm::translate(giraffeModel, glm::vec3(5.0f, -1.0f, -2.0f));
	giraffeModel = glm::scale(giraffeModel, glm::vec3(1.5f));
	objShader.SetMat4("model", giraffeModel);
	models[1].Draw(objShader);
	
	glm::mat4 SeaLionModel = glm::mat4(1.0);
	SeaLionModel = glm::translate(SeaLionModel, glm::vec3(-2.0f, -1.0f, 0.0f));
	SeaLionModel = glm::scale(SeaLionModel, glm::vec3(0.01f));  // Changed from 0.3f to 0.01f

	// Set the model matrix in the shader
	objShader.SetMat4("model", SeaLionModel);
	models[2].Draw(objShader);


	glm::mat4 wolfModel = glm::mat4(1.0);
	wolfModel = glm::translate(wolfModel, glm::vec3(2.f, -1.f, 0.5f));
	wolfModel = glm::scale(wolfModel, glm::vec3(1.f));
	objShader.SetMat4("model", wolfModel);
	models[3].Draw(objShader);

//<<<<<<< HEAD
	
	glm::mat4 treeModel = glm::mat4(1.0);
treeModel = glm::translate(treeModel, glm::vec3(2.f, -1.f, 2.5f));
treeModel = glm::scale(treeModel, glm::vec3(0.2f));
objShader.SetMat4("model", treeModel);
models[4].Draw(objShader);


	for (int i = 0; i < 10; i++) { // Place 10 instances
		glm::mat4 grassModel = glm::mat4(1.0);
		// Translate the model along the X-axis based on the loop index
		grassModel = glm::translate(grassModel, glm::vec3(2.f + (i * 3.f), -1.2f, 0.5f));
		// Rotate the model
		grassModel = glm::rotate(grassModel, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		if (i % 2 == 0)
			grassModel = glm::rotate(grassModel, glm::radians(270.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		// Scale the model
		grassModel = glm::scale(grassModel, glm::vec3(0.01f));
		// Set the transformation matrix
		objShader.SetMat4("model", grassModel);
		// Draw the model
		models[5].Draw(objShader);
	}


	glm::mat4 catModel = glm::mat4(1.0);
	catModel = glm::translate(catModel, glm::vec3(0.f, -1.f, 2.5f));
	catModel = glm::scale(catModel, glm::vec3(0.01f));
	//catModel = glm::rotate(catModel, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));// iti roteste invers modelul
	//catModel = glm::rotate(catModel, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));// iti roteste introparte  modelul
	catModel = glm::rotate(catModel, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));// iti roteste introparte  modelul
	objShader.SetMat4("model", catModel);
	models[6].Draw(objShader);

	glm::mat4 tree2Model = glm::mat4(1.0);
	tree2Model = glm::translate(tree2Model, glm::vec3(0.f, -1.f, 2.5f));
	//tree2Model = glm::scale(tree2Model, glm::vec3(0.3f));
	objShader.SetMat4("model", tree2Model);
	models[7].Draw(objShader);
	//Here we render the light source

	glm::mat4 tree3Model = glm::mat4(1.0);
	tree3Model = glm::translate(tree3Model, glm::vec3(0.f, -3.f, 0.f));
	tree3Model = glm::scale(tree3Model, glm::vec3(0.5f));
	objShader.SetMat4("model", tree3Model);
	models[8].Draw(objShader);

	glm::mat4 condorModel = glm::mat4(1.0);
	// Position the condor - adjust these values to place it where you want
	condorModel = glm::translate(condorModel, glm::vec3(10.0f, 2.0f, 0.0f));  // Placed slightly higher since it's a flying bird
	// Scale the condor - adjust based on its original size
	condorModel = glm::scale(condorModel, glm::vec3(0.5f));
	// You can add rotation if needed, for example to make it face a certain direction:
	condorModel = glm::rotate(condorModel, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	// Set the model matrix in the shader
	objShader.SetMat4("model", condorModel);
	// Draw the condor - the index should be the position where you added it in the models vector
	models[24].Draw(objShader);  // Assuming it's the 18th model (index 17)

	//glm::mat4 horseModel = glm::mat4(1.0);
	//horseModel = glm::translate(horseModel, glm::vec3(0.f, -1.f, 2.5f));
	//horseModel = glm::scale(horseModel, glm::vec3(100.0f));
	//objShader.SetMat4("model", horseModel);
	//models[9].Draw(objShader);

	bool move = false;
	for ( int i = 0;i<5;i++)
	{
		glm::mat4 deerModel = glm::mat4(1.0);
		if (i % 2 == 0)
			deerModel = glm::translate(deerModel, glm::vec3(i * 0.5f + 5.f, -1.f, 0.5f + move));
		else
			deerModel = glm::translate(deerModel, glm::vec3(i * 0.5f + 5.f, -1.f, 1.f + move));

		deerModel = glm::scale(deerModel, glm::vec3(0.03f));
		deerModel = glm::rotate(deerModel, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		deerModel = glm::rotate(deerModel, glm::radians(270.0f), glm::vec3(0.f, 0.0f, 1.0f));
		objShader.SetMat4("model", deerModel);
		models[10].Draw(objShader);
		if (i%3==0)
		move = !move;
	}


	for (int i = 0; i < 6; i++)
	{
		glm::mat4 fenceModel = glm::mat4(1.0);
		fenceModel = glm::translate(fenceModel, glm::vec3(11.f, -1.5f, 12.f + (i * 4.5f)));
		fenceModel = glm::scale(fenceModel, glm::vec3(0.7f));
		fenceModel = glm::rotate(fenceModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		fenceModel = glm::rotate(fenceModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		objShader.SetMat4("model", fenceModel);
		models[11].Draw(objShader);
	}

	double aux = 0.0;
	for (int i = 0; i < 3; i++)
	{
		glm::mat4 fenceModel = glm::mat4(1.0);
		fenceModel = glm::translate(fenceModel, glm::vec3(13.3f + (i * 4.5f), -1.5f + aux, 37.f));
		aux += 0.2;
		fenceModel = glm::scale(fenceModel, glm::vec3(0.7f));
		fenceModel = glm::rotate(fenceModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		objShader.SetMat4("model", fenceModel);
		models[11].Draw(objShader);
	}

	//glm::mat4 fenceModel = glm::mat4(1.0);
	//fenceModel = glm::translate(fenceModel, glm::vec3(10.f, -1.5f, 12.f + (9 * 4.5f)));
	//fenceModel = glm::scale(fenceModel, glm::vec3(0.7f));
	//fenceModel = glm::rotate(fenceModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//fenceModel = glm::rotate(fenceModel, glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//objShader.SetMat4("model", fenceModel);
	//models[11].Draw(objShader);

	for (int i = 0; i < 8; i++)
	{
		glm::mat4 fenceModel = glm::mat4(1.0);
		fenceModel = glm::translate(fenceModel, glm::vec3(2.f, -1.5f, 12.f + (i * 4.5f)));
		fenceModel = glm::scale(fenceModel, glm::vec3(0.7f));
		fenceModel = glm::rotate(fenceModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		objShader.SetMat4("model", fenceModel);
		models[11].Draw(objShader);
	}


	glm::mat4 lampModel = glm::mat4(1.0);
	lampModel = glm::translate(lampModel, glm::vec3(2.f, -1.2f, 2.6f));
	lampModel = glm::scale(lampModel, glm::vec3(0.1f));
	objShader.SetMat4("model", lampModel);
	models[12].Draw(objShader);


	glm::mat4 elephantModel = glm::mat4(1.0);
	elephantModel = glm::translate(elephantModel, glm::vec3(15.f, -1.f, 0.f));
	elephantModel = glm::scale(elephantModel, glm::vec3(10.f));
	objShader.SetMat4("model", elephantModel);
	models[13].Draw(objShader);


	glm::mat4 lionModel = glm::mat4(1.0);
	lionModel = glm::translate(lionModel, glm::vec3(18.f, -1.f, 0.f));
	lionModel = glm::scale(lionModel, glm::vec3(10.f));
	objShader.SetMat4("model", lionModel);
	models[14].Draw(objShader);

	glm::mat4 bearModel = glm::mat4(1.0);
	bearModel = glm::translate(bearModel, glm::vec3(20.f, -1.f, 0.f));
	bearModel = glm::scale(bearModel, glm::vec3(15.f));
	objShader.SetMat4("model", bearModel);
	models[15].Draw(objShader);

	for (int i = 0; i < 10; i++) {
		glm::mat4 zebraModel = glm::mat4(1.0);
		zebraModel = glm::translate(zebraModel, glm::vec3(15.f + (i * 3.2f) , -1.f, 5.f));
		zebraModel = glm::scale(zebraModel, glm::vec3(10.f));
		objShader.SetMat4("model", zebraModel);
		models[16].Draw(objShader);

	}

	glm::mat4 apeModel = glm::mat4(1.0);
	apeModel = glm::translate(apeModel, glm::vec3(10.f, -1.f, 1.f));
	apeModel = glm::scale(apeModel, glm::vec3(15.f));
	objShader.SetMat4("model", apeModel);
	models[17].Draw(objShader);

	glm::mat4 ape2Model = glm::mat4(1.0);
	ape2Model = glm::translate(ape2Model, glm::vec3(11.f, -1.f, 1.f));
	ape2Model = glm::scale(ape2Model, glm::vec3(2.f));
	objShader.SetMat4("model", ape2Model);
	models[18].Draw(objShader);


	glm::mat4 kangarooModel = glm::mat4(1.0);
	kangarooModel = glm::translate(kangarooModel, glm::vec3(13.f, -1.f, 1.f));
	kangarooModel = glm::scale(kangarooModel, glm::vec3(1.f));
	objShader.SetMat4("model", kangarooModel);
	models[19].Draw(objShader);

	// ********************************************************************************************************************

	glm::mat4 hayBaleModel = glm::mat4(1.0);
	hayBaleModel = glm::translate(hayBaleModel, glm::vec3(7.f, -1.f, 2.f));
	hayBaleModel = glm::scale(hayBaleModel, glm::vec3(0.5f));
	objShader.SetMat4("model", hayBaleModel);
	models[20].Draw(objShader);


	glm::mat4 bush2Model = glm::mat4(1.0);
	bush2Model = glm::translate(bush2Model, glm::vec3(11.f, -1.f, 2.f));
	bush2Model = glm::scale(bush2Model, glm::vec3(1.f));
	objShader.SetMat4("model", bush2Model);
	models[21].Draw(objShader);

	glm::mat4 bushModel = glm::mat4(1.0);
	bushModel = glm::translate(bushModel, glm::vec3(10.f, -1.f, 2.f));
	bushModel = glm::scale(bushModel, glm::vec3(0.3f));
	objShader.SetMat4("model", bushModel);
	models[22].Draw(objShader);

	glm::mat4 stationModel = glm::mat4(1.0);
	stationModel = glm::translate(stationModel, glm::vec3(-5.f, -1.f, 5.f));
	stationModel = glm::rotate(stationModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	stationModel = glm::scale(stationModel, glm::vec3(1.f));
	objShader.SetMat4("model", stationModel);
	models[23].Draw(objShader);


	//glm::mat4 WallModel = glm::mat4(1.0);
	//WallModel = glm::translate(WallModel, glm::vec3(8.0f, 0.0f, 10.0f));
	//WallModel = glm::rotate(WallModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//WallModel = glm::scale(WallModel, glm::vec3(4.f));
	//objShader.SetMat4("model", WallModel);
	//models[25].Draw(objShader);



	// ********************************************************************************************************************
	// the grround must be placed at the end 
	int index = models.size() - 1;

	glm::mat4 terrainModel1 = glm::mat4(1.0);
	terrainModel1 = glm::translate(terrainModel1, glm::vec3(8.0f, -2.0f, 10.0f));
	terrainModel1 = glm::scale(terrainModel1, glm::vec3(5.0f));
	terrainModel1 = glm::rotate(terrainModel1, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	objShader.SetMat4("model", terrainModel1);

	models[index].Draw(objShader);
	glm::mat4 terrainModel = glm::mat4(1.0);
	terrainModel = glm::translate(terrainModel, glm::vec3(8.0f, -2.0f, 40.f));
	terrainModel = glm::scale(terrainModel, glm::vec3(-5.0f, 5.0f, 5.0f));
	objShader.SetMat4("model", terrainModel);
	models[index].Draw(objShader);

	glm::mat4 terrainModel2 = glm::mat4(1.0);
	terrainModel2 = glm::translate(terrainModel2, glm::vec3(8.0f, -2.0f, 70.0f));
	terrainModel2 = glm::scale(terrainModel2, glm::vec3(5.0f));
	terrainModel2 = glm::rotate(terrainModel2, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	objShader.SetMat4("model", terrainModel2);
	models[index].Draw(objShader);

	glm::mat4 terrainModel3 = glm::mat4(1.0);
	terrainModel3 = glm::translate(terrainModel3, glm::vec3(8.0f, -2.0f, 100.f));
	terrainModel3 = glm::scale(terrainModel3, glm::vec3(-5.0f, 5.0f, 5.0f));
	//terrainModel3 = glm::rotate(terrainModel3, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	objShader.SetMat4("model", terrainModel3);
	models[index].Draw(objShader);


	// ********************************************************************************************************************

	lampShader.Use();
	lampShader.SetMat4("projection", pCamera->GetProjectionMatrix());
	lampShader.SetMat4("view", pCamera->GetViewMatrix());
	model = glm::translate(glm::mat4(1.0), lightPos);
	model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
	lampShader.SetMat4("model", model);

	RenderCube();
	//glBindVertexArray(lightVAO);
	//glDrawArrays(GL_TRIANGLES, 0, 36);
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
//update
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
	currentPath = converter.to_bytes(wscurrentPath) + "\\Explorare-Muzeu-Proiect-5-10LF333";

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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewInit();

	glEnable(GL_DEPTH_TEST);

	CreateVBO();



	ShaderProgram.Create("PhongLight.vs", "PhongLight.fs");
	lampShader.Create("Lamp.vs", "Lamp.fs");
	objShader.Create("PhongLightWithTexture.vs", "PhongLightWithTexture.fs");

	pCamera = new Camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0, 0.0, 3.0));

	//Models loading
	//aici am initializat un model, in cazul asta e un pirat, dar poti sa adaugi oricate modele vrei, doar sa ai fisierele alea in folderul Models, fiecare model sa aiba un folder al lui cu denumire intuitiva
	//va rog sa respectati formatul cu \\, altfel nu o sa mearga
	std::string PiratePath = currentPath + "\\Models\\Pirat\\Pirat.obj";
	//aici adaugi modelul in vectorul de modele, in cazul asta e models, dar poti sa il numesti cum vrei
	//primul parametru e calea catre model, al doilea e mereu false, simplu
	models.emplace_back(PiratePath, false);


	std::string GiraffePath = currentPath + "\\Models\\Giraffe\\CIL1PYJ81IH0BT4B9ME2F53L7.obj";
	//std::string GiraffeTexturePath = currentPath + "\\Models\\Giraffe\\Giraffe.jpg";
	models.emplace_back(GiraffePath, false);

	std::string SeaLionPath = currentPath + "\\Models\\SeaLion\\10041_sealion_v1_L3.obj";
	models.emplace_back(SeaLionPath, false);

	std::string WolfPath = currentPath + "\\Models\\Wolf\\Wolf_One_obj.obj";
	models.emplace_back(WolfPath, false);
	std::string TreePath = currentPath + "\\Models\\Tree\\Tree2.obj";
	models.emplace_back(TreePath, false);
	std::string GrassPath = currentPath + "\\Models\\Grass\\Grass.obj";
	models.emplace_back(GrassPath, false);

	std::string CatPath = currentPath + "\\Models\\Cat\\Cat.obj";
	models.emplace_back(CatPath, false);
	std::string Tree2Path = currentPath + "\\Models\\Tree2\\Tree.obj";
	models.emplace_back(Tree2Path, false);
	std::string Tree3Path = currentPath + "\\Models\\Tree3\\tree 1.obj";
	models.emplace_back(Tree3Path, false);

	std::string HorsePath = currentPath + "\\Models\\Horse\\horse.obj";
	models.emplace_back(HorsePath, false);

	std::string DeerPath = currentPath + "\\Models\\Deer\\deer.obj";
	models.emplace_back(DeerPath, false);

	std::string FencePath = currentPath + "\\Models\\Fence\\Fence.obj";
	models.emplace_back(FencePath, false);

	std::string LampPath = currentPath + "\\Models\\Lamp\\lamp.obj";
	models.emplace_back(LampPath, false);

	std::string ElephantPath = currentPath + "\\Models\\Elephant\\elephant.obj";
	models.emplace_back(ElephantPath, false);

	std::string lionPath = currentPath + "\\Models\\lion\\lion.obj";
	models.emplace_back(lionPath, false);
	std::string bearPath = currentPath + "\\Models\\bear\\bear.obj";
	models.emplace_back(bearPath, false);

	std::string zebraPath = currentPath + "\\Models\\zebra\\zebra.obj";
	models.emplace_back(zebraPath, false);

	

	std::string apePath = currentPath + "\\Models\\ape\\ape.obj";
	models.emplace_back(apePath, false);

	std::string ape2Path = currentPath + "\\Models\\ape2\\ape2.obj";
	models.emplace_back(ape2Path, false);

	std::string kangarooPath = currentPath + "\\Models\\kangaroo\\kangaroo.obj";
	models.emplace_back(kangarooPath, false);


	std::string hayBalePath = currentPath + "\\Models\\hayBale\\hayBale.obj";
	models.emplace_back(hayBalePath, false);

	std::string bush2Path = currentPath + "\\Models\\bush2\\bush2.obj";
	models.emplace_back(bush2Path, false);

	std::string bushPath = currentPath + "\\Models\\bush\\bush.obj";
	models.emplace_back(bushPath, false);

	std::string stationPath = currentPath + "\\Models\\station\\station.obj";
	models.emplace_back(stationPath, false);

	std::string CondorPath = currentPath + "\\Models\\Condor\\CONDOR.OBJ";
	models.emplace_back(CondorPath, false);

	std::string WallPath = currentPath + "\\Models\\Wall\\Wall.obj";
	models.emplace_back(WallPath, false);


	//********************************************************************************************************************
	// Load the terrain model
	std::string Terrain = currentPath + "\\Models\\Terrain\\terrainBlender.obj";
	models.emplace_back(Terrain, false);

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

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		if (isCursorVisible)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		isCursorVisible = !isCursorVisible;
	}
}


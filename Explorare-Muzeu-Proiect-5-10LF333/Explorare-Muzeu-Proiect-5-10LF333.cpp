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

float g_fKA = 0.5f;
float g_fKD = 0.8f;
float g_fKS = 0.8f;

bool isCursorVisible = false;

Shader ShaderProgram;
Shader lampShader;
Shader objShader;

//Asa trebuie initializat un vector de modele, destul de basic
std::vector<Model> structures;
std::vector<Model> animals;
std::vector<Model> characters;
std::vector<Model> plants;
std::unique_ptr<Model> terrain;

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


std::vector<glm::vec3> treePostions{

{13.300000, -0.2, 83.5},
{14.000000, -1.338057, 99.466332},
{16.100000, -1.436723, 96.326366},

{20.100000, -0.298570, 86.346414},
{20.100000, -0.298570, 86.346414},
{23.500000, -0.083629, 82.822970},
{20.300000, 0., 78.000000},
{21.100000, 0., 76.349912},



	{2.f, -1.f, 2.5f},
	{2.f, -1.f, 6.5f},
	{0.f, -1.5f, 12.f},
	{0.f, -1.5f, 9.f},
	{0.f, -1.5f, 10.5f},
{-10.000000, -1.346646, 18.111554},
{-9.800000, -1.122772, 9.551964},
{-9.600000, -0.211960, 34.342767},
{-9.200000, -0.838879, 22.943361},
{-8.200000, -0.719646, 14.492779},
{-8.000000, -0.846338, 34.066363},
{-7.600000, -1.171284, 27.976573},
{-6.800000, -0.556925, 23.946574},
{-6.600000, 0.099945, 22.466574},
{-5.800000, -1.076061, 29.462464},
{-5.400000, -0.882336, 18.744073},
{-5.200000, -1.127739, 18.716882},
{-5.000000, -2.026368, 9.691571},
{-4.800000, -0.599670, 9.398526},
{-4.600000, -1.396008, 30.895869},
{-4.400000, -1.731190, 26.174317},
{-4.000000, -0.251676, 25.789227},
{-3.000000, -1.390358, 33.677923},
{-2.200000, -1.250061, 13.035035},
{-2.000000, -1.007390, 26.420884},
{-1.800000, -1.482487, 25.764499},
{-1.600000, -1.113837, 18.960118},
{-1.400000, -1.767291, 15.338562},
{-1.000000, -2.591976, 31.895439},
{-0.800000, -0.676334, 29.351990},
{-0.600000, -0.640694, 36.621535},
{-0.200000, -1.138618, 23.607306},
{-0.000000, -0.632756, 25.645149},
{0.200000, -1.114014, 9.620693},
{0.400000, -1.637895, 15.416776},


{13.500000, -1.379707, 21.194847},
{13.700000, -1.267365, 16.204368},
{14.100000, -1.420285, 35.919143},
{14.900000, -0.967572, 23.212594},
{15.100000, -0.811116, 9.390420},
{15.300000, -0.101410, 15.934450},
{15.500000, -0.487200, 27.786048},
{15.900000, -0.399102, 15.612950},
{16.900000, -0.449054, 12.585816},
{17.300000, 0.092728, 26.180203},
{18.300000, 0.165961, 30.290041},
{18.700000, -0.601062, 18.048049},
{19.100000, -0.592329, 36.269791},
{19.300000, -0.731511, 35.569631},
{19.500000, -0.966829, 36.560820},
{19.900000, -0.559012, 28.737279},
{20.100000, -0.303764, 30.441375},
{20.900000, 0.000665, 11.021459},
{21.300000, -0.840972, 16.095096},
{21.900000, -0.306067, 31.991234},
{22.100000, 0.200000, 29.264241},
{22.300000, 0.043170, 15.914124},
{22.700000, -0.877967, 12.781286},
{23.300000, -0.459367, 31.284772},
{23.500000, -0.711440, 13.636387},
{23.700000, -0.514812, 27.155240},
{24.900000, -0.317980, 15.070709},
{25.500000, -0.862291, 22.655835},
{26.300000, -0.479031, 35.299343},
{26.500000, -0.012589, 9.878562},


{-6.800000, -0.425941, 103.904138},

{-11.000000, -0.847032, 96.675356},
{-8.200000, -0.214220, 96.471143},

{-11.800000, -0.041814, 92.068269},
{-7.600000, -0.699712, 84.},
{-9.600000, -0.782750, 82.656487},
{-3.400000, -1.023838, 82.330119},
{-3.600000, -1.110696, 77.183173},
{-8.800000, -0.092506, 66.411127},
{-6.400000, 0.094728, 66.010068},
{-6.200000, -0.586335, 65.863078},
{-3.000000, -1.449471, 62.978949},


};

void generateTrees()
{
	double scale = 0.05;
	for (int i = 0; i < treePostions.size(); i++)
	{
		if (i < 42)
		{
			glm::mat4 treeModel = glm::mat4(1.0);
			treeModel = glm::translate(treeModel, glm::vec3(treePostions[i]));
			
			if (i <2)
				treeModel = glm::scale(treeModel, glm::vec3(0.3f));
			else
				treeModel = glm::scale(treeModel, glm::vec3(0.2f));
			objShader.SetMat4("model", treeModel);
			plants[0].Draw(objShader);
		}
		else
		{
			glm::mat4 tree2Model = glm::mat4(1.0);
			tree2Model = glm::translate(tree2Model, glm::vec3(treePostions[i]));
			objShader.SetMat4("model", tree2Model);
			plants[2].Draw(objShader);
		}
	}

	glm::mat4 tree2Model = glm::mat4(1.0);
	tree2Model = glm::translate(tree2Model, glm::vec3(-8.f, -0.5f, 55.442307f));
	tree2Model = glm::scale(tree2Model, glm::vec3(1.2f));
	objShader.SetMat4("model", tree2Model);
	plants[2].Draw(objShader);
}

void generateFences()
{
	for (int i = 0; i < 6; i++)
	{
		glm::mat4 fenceModel = glm::mat4(1.0);
		fenceModel = glm::translate(fenceModel, glm::vec3(11.f, -1.5f, 12.f + (i * 4.5f)));
		fenceModel = glm::scale(fenceModel, glm::vec3(0.7f));
		fenceModel = glm::rotate(fenceModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		fenceModel = glm::rotate(fenceModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		objShader.SetMat4("model", fenceModel);
		structures[0].Draw(objShader);
	}

	for (int i = 12; i < 21; i++)
	{
		glm::mat4 fenceModel = glm::mat4(1.0);
		fenceModel = glm::translate(fenceModel, glm::vec3(11.f, -1.5f, 12.f + (i * 4.5f)));
		fenceModel = glm::scale(fenceModel, glm::vec3(0.7f));
		fenceModel = glm::rotate(fenceModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		fenceModel = glm::rotate(fenceModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		objShader.SetMat4("model", fenceModel);
		structures[0].Draw(objShader);
	}

	double aux = 0.1;
	double endFence = 36.8;
	while (endFence > 0)
	{
		for (int i = 0; i < 3; i++)
		{
			double x = 13.3f + (i * 4.5f);
			double y = -1.5f + aux;
			double z = endFence;
			aux += 0.5;

			glm::mat4 fenceModel = glm::mat4(1.0);
			fenceModel = glm::translate(fenceModel, glm::vec3(x, y, z));
			fenceModel = glm::scale(fenceModel, glm::vec3(0.7f));
			fenceModel = glm::rotate(fenceModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			objShader.SetMat4("model", fenceModel);
			structures[0].Draw(objShader);
		}
		endFence -= 13.5;
		aux = 0.1;
	}

	endFence = 104.3;
	aux = 0.1;
	for (int i = 0; i < 4; i++)
	{
		for (int i = 0; i < 3; i++)
		{
			double x = 13.3f + (i * 4.5f);
			double y = -1.5f + aux;
			double z = endFence;
			aux += 0.5;

			glm::mat4 fenceModel = glm::mat4(1.0);
			fenceModel = glm::translate(fenceModel, glm::vec3(x, y, z));
			fenceModel = glm::scale(fenceModel, glm::vec3(0.7f));
			fenceModel = glm::rotate(fenceModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			objShader.SetMat4("model", fenceModel);
			structures[0].Draw(objShader);
		}
		endFence -= 13.5;
		
		aux = 0.1;
	}


	//glm::mat4 fenceModel = glm::mat4(1.0);
	//fenceModel = glm::translate(fenceModel, glm::vec3(10.f, -1.5f, 12.f + (9 * 4.5f)));
	//fenceModel = glm::scale(fenceModel, glm::vec3(0.7f));
	//fenceModel = glm::rotate(fenceModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//fenceModel = glm::rotate(fenceModel, glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//objShader.SetMat4("model", fenceModel);
	//models[11].Draw(objShader);

	// right fence
	for (int i = 0; i < 21; i++)
	{
		glm::mat4 fenceModel = glm::mat4(1.0);
		fenceModel = glm::translate(fenceModel, glm::vec3(2.f, -1.5f, 12.f + (i * 4.5f)));
		fenceModel = glm::scale(fenceModel, glm::vec3(0.7f));
		fenceModel = glm::rotate(fenceModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		objShader.SetMat4("model", fenceModel);
		structures[0].Draw(objShader);
	}


	// erase the lat 2
	endFence = 104,3;
	aux = 0.1;
	while (endFence > 0)
	{

		for (int i = 0; i < 3; i++)
		{
			double x = -0.3f - (i * 4.5f);
			double y = -1.5f + aux;
			double z = endFence;

			glm::mat4 fenceModel = glm::mat4(1.0);
			fenceModel = glm::translate(fenceModel, glm::vec3(x, y, z));
			aux += 0.3;
			fenceModel = glm::scale(fenceModel, glm::vec3(0.7f));
			fenceModel = glm::rotate(fenceModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			objShader.SetMat4("model", fenceModel);
			structures[0].Draw(objShader);
		}
		endFence -= 27;
		aux = 0.1;
	}

	for (int i = 0; i < 3; i++)
	{
		double x = -0.3f - (i * 4.5f);
		double y = -1.5f + aux;
		double z = endFence + 13.5;

		glm::mat4 fenceModel = glm::mat4(1.0);
		fenceModel = glm::translate(fenceModel, glm::vec3(x, y, z));
		aux += 0.3;
		fenceModel = glm::scale(fenceModel, glm::vec3(0.7f));
		fenceModel = glm::rotate(fenceModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		objShader.SetMat4("model", fenceModel);
		structures[0].Draw(objShader);
	}

}

void generateWalls() {
	for (int i = 0; i < 26; i++)
	{
		glm::mat4 WallModel = glm::mat4(1.0);
		WallModel = glm::translate(WallModel, glm::vec3(28.f, 2.f, 0.f + (i * 4.5f)));
		WallModel = glm::rotate(WallModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		WallModel = glm::scale(WallModel, glm::vec3(1.6f));
		objShader.SetMat4("model", WallModel);
		structures[3].Draw(objShader);
	}

	for (int i = 0; i < 9; i++)
	{
		glm::mat4 WallModel = glm::mat4(1.0);
		WallModel = glm::translate(WallModel, glm::vec3(26.5f - (i * 4.5f), 1.9f, 114.5f));
		WallModel = glm::rotate(WallModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		WallModel = glm::scale(WallModel, glm::vec3(1.7f));
		objShader.SetMat4("model", WallModel);
		structures[3].Draw(objShader);
	}

	for (int i = 0; i < 26; i++)
	{
		glm::mat4 WallModel = glm::mat4(1.0);
		WallModel = glm::translate(WallModel, glm::vec3(-12.f, 2.f, 0.f + (i * 4.5f)));
		WallModel = glm::rotate(WallModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		WallModel = glm::scale(WallModel, glm::vec3(1.6f));
		objShader.SetMat4("model", WallModel);
		structures[3].Draw(objShader);
	}

	for (int i = 0; i < 9; i++)
	{
		glm::mat4 WallModel = glm::mat4(1.0);
		WallModel = glm::translate(WallModel, glm::vec3(26.5f - (i * 4.5f), 1.8f, -4.7f));
		WallModel = glm::rotate(WallModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		WallModel = glm::scale(WallModel, glm::vec3(1.7f));
		objShader.SetMat4("model", WallModel);
		structures[3].Draw(objShader);
	}

}

void placeDeer()
{
	static double moveforword = 0.0;
	static float moveCircle = 0.0;
	static bool leftDirection = true;

	static int waitingTime = 30;
	// Place deer1
	glm::mat4 deerModel = glm::mat4(1.0);
	deerModel = glm::translate(deerModel, glm::vec3(0.6f + 12.f, -1.f, 14.5f));
	deerModel = glm::scale(deerModel, glm::vec3(0.04f));
	deerModel = glm::rotate(deerModel, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	deerModel = glm::rotate(deerModel, glm::radians(270.0f), glm::vec3(0.f, 0.0f, 1.0f));
	deerModel = glm::rotate(deerModel, glm::radians(-90.0f), glm::vec3(0.f, 0.0f, 1.0f));
    deerModel = glm::rotate(deerModel, glm::radians(0.0f + moveCircle), glm::vec3(0.f, 0.0f, 1.0f));
	objShader.SetMat4("model", deerModel);
	animals[5].Draw(objShader);


	// Place deer2
	glm::mat4 deer2Model = glm::mat4(1.0);
	deer2Model = glm::translate(deer2Model, glm::vec3(2.f + 12.f - moveforword, -1.f, 18.0f + moveforword));
	deer2Model = glm::scale(deer2Model, glm::vec3(0.04f));
	deer2Model = glm::rotate(deer2Model, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	deer2Model = glm::rotate(deer2Model, glm::radians(15.0f), glm::vec3(0.f, 0.0f, 1.0f));
	deer2Model = glm::rotate(deer2Model, glm::radians(270.0f), glm::vec3(0.f, 0.0f, 1.0f));
	deer2Model = glm::rotate(deer2Model, glm::radians(-90.0f), glm::vec3(0.f, 0.0f, 1.0f));
	if (moveforword <= 2)
		moveforword += 0.01;
	else 
		deer2Model = glm::rotate(deer2Model, glm::radians(0.0f - moveCircle), glm::vec3(0.f, 0.0f, 1.0f));
	objShader.SetMat4("model", deer2Model);
	animals[5].Draw(objShader);
	

	if (moveCircle < 30 && leftDirection)
		moveCircle += 1;
	else
		if (moveCircle > 0 && !leftDirection)
			moveCircle -= 1;
		else
			if (waitingTime == 0)
			{
				leftDirection = !leftDirection;
				waitingTime = 60;
			}
			else waitingTime--;

	// Place deer3
	glm::mat4 deer3Model = glm::mat4(1.0);
	deer3Model = glm::translate(deer3Model, glm::vec3(2.f + 12.f, -1.f, 19.5f));
	deer3Model = glm::scale(deer3Model, glm::vec3(0.04f));
	deer3Model = glm::rotate(deer3Model, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	deer3Model = glm::rotate(deer3Model, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	deer3Model = glm::rotate(deer3Model, glm::radians(270.0f), glm::vec3(0.f, 0.0f, 1.0f));
	deer3Model = glm::rotate(deer3Model, glm::radians(-90.0f), glm::vec3(0.f, 0.0f, 1.0f));
	objShader.SetMat4("model", deer3Model);
	animals[5].Draw(objShader);
}

void placeKangaroo() {

	glm::mat4 kangarooModel1 = glm::mat4(1.0);
	kangarooModel1 = glm::translate(kangarooModel1, glm::vec3(16.f, -1.f, 100.f));
	kangarooModel1 = glm::rotate(kangarooModel1, glm::radians(-90.f), glm::vec3(0.f, 1.0f, 0.0f));
	kangarooModel1 = glm::scale(kangarooModel1, glm::vec3(1.f));
	objShader.SetMat4("model", kangarooModel1);
	animals[12].Draw(objShader);


	glm::mat4 kangarooModel2 = glm::mat4(1.0);
	kangarooModel2 = glm::translate(kangarooModel2, glm::vec3(14.f, -1.f, 98.f)); 
	kangarooModel2 = glm::rotate(kangarooModel2, glm::radians(0.f), glm::vec3(0.f, 1.0f, 0.0f)); 
	kangarooModel2 = glm::scale(kangarooModel2, glm::vec3(1.f));
	objShader.SetMat4("model", kangarooModel2);
	animals[12].Draw(objShader);

	glm::mat4 kangarooModel3 = glm::mat4(1.0);
	kangarooModel3 = glm::translate(kangarooModel3, glm::vec3(18.f, 0.0f, 93.f)); 
	kangarooModel3 = glm::rotate(kangarooModel3, glm::radians(90.f), glm::vec3(0.f, 1.0f, 0.0f)); 
	kangarooModel3 = glm::scale(kangarooModel3, glm::vec3(1.f));
	objShader.SetMat4("model", kangarooModel3);
	animals[12].Draw(objShader);
}

void placeWolf()
	{
		glm::mat4 wolf1Model = glm::mat4(1.0);
		wolf1Model = glm::translate(wolf1Model, glm::vec3(-7.f, -0.5f, 30.5f));
		wolf1Model = glm::scale(wolf1Model, glm::vec3(2.5f));
		objShader.SetMat4("model", wolf1Model);
		animals[2].Draw(objShader);

		glm::mat4 wolf2Model = glm::mat4(1.0);
		wolf2Model = glm::translate(wolf2Model, glm::vec3(-2.f, -1.f, 42.5f));
		wolf2Model = glm::scale(wolf2Model, glm::vec3(2.5f));
		wolf2Model = glm::rotate(wolf2Model, glm::radians(90.0f), glm::vec3(0.f, 1.0f, 0.0f));
		objShader.SetMat4("model", wolf2Model);
		animals[2].Draw(objShader);

		glm::mat4 wolf3Model = glm::mat4(1.0);
		wolf3Model = glm::translate(wolf3Model, glm::vec3(-8.f, 0.f, 40.5f));
		wolf3Model = glm::scale(wolf3Model, glm::vec3(2.5f));
		wolf3Model = glm::rotate(wolf3Model, glm::radians(180.0f), glm::vec3(0.f, 1.0f, 0.0f));
		objShader.SetMat4("model", wolf3Model);
		animals[2].Draw(objShader);

		glm::mat4 wolf4Model = glm::mat4(1.0);
		wolf4Model = glm::translate(wolf4Model, glm::vec3(-5.f, -0.5f, 28.5f));
		//wolf4Model = glm::rotate(wolf4Model, glm::radians(15.0f), glm::vec3(0.f, 0.0f, 1.0f));
		wolf4Model = glm::scale(wolf4Model, glm::vec3(2.5f));
		objShader.SetMat4("model", wolf4Model);
		animals[2].Draw(objShader);
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
	characters[0].Draw(objShader);

	// Hay bale for lion's cage
	glm::mat4 lionHayModel = glm::mat4(1.0);
	lionHayModel = glm::translate(lionHayModel, glm::vec3(16.f, -1.f, 65.f));
	lionHayModel = glm::scale(lionHayModel, glm::vec3(0.7f));
	objShader.SetMat4("model", lionHayModel);
	plants[4].Draw(objShader);

	// Second hay bale for lion's cage
	glm::mat4 lionHayModel2 = glm::mat4(1.0);
	lionHayModel2 = glm::translate(lionHayModel2, glm::vec3(12.f, -1.f, 69.f));
	lionHayModel2 = glm::scale(lionHayModel2, glm::vec3(0.6f));
	objShader.SetMat4("model", lionHayModel2);
	plants[4].Draw(objShader);

	// Hay bale for giraffe's area
	glm::mat4 giraffeHayModel = glm::mat4(1.0);
	giraffeHayModel = glm::translate(giraffeHayModel, glm::vec3(-5.f, -1.f, 51.f));
	giraffeHayModel = glm::scale(giraffeHayModel, glm::vec3(0.8f));
	objShader.SetMat4("model", giraffeHayModel);
	plants[4].Draw(objShader);

	// Second hay bale for giraffe's area
	glm::mat4 giraffeHayModel2 = glm::mat4(1.0);
	giraffeHayModel2 = glm::translate(giraffeHayModel2, glm::vec3(-1.f, -1.f, 54.f));
	giraffeHayModel2 = glm::scale(giraffeHayModel2, glm::vec3(0.7f));
	objShader.SetMat4("model", giraffeHayModel2);
	plants[4].Draw(objShader);

	glm::mat4 giraffeModel = glm::mat4(1.0);
	giraffeModel = glm::translate(giraffeModel, glm::vec3( - 3.f, 1.2f, 53.0f));
	giraffeModel = glm::rotate(giraffeModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	giraffeModel = glm::scale(giraffeModel, glm::vec3(1.9f));
	objShader.SetMat4("model", giraffeModel);
	animals[0].Draw(objShader);

	glm::mat4 babygiraffeModel = glm::mat4(1.0);
	babygiraffeModel = glm::translate(babygiraffeModel, glm::vec3(-3.5f, -0.1f, 53.0f));
	babygiraffeModel = glm::rotate(babygiraffeModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	babygiraffeModel = glm::rotate(babygiraffeModel, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	babygiraffeModel = glm::scale(babygiraffeModel, glm::vec3(0.8f));
	objShader.SetMat4("model", babygiraffeModel);
	animals[0].Draw(objShader);


	glm::mat4 elephantModel = glm::mat4(1.0);
	elephantModel = glm::translate(elephantModel, glm::vec3(-3.f, -1.f, 85.0f));
	elephantModel = glm::rotate(elephantModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	elephantModel = glm::scale(elephantModel, glm::vec3(12.f));
	objShader.SetMat4("model", elephantModel);
	animals[6].Draw(objShader);


	glm::mat4 SeaLionModel = glm::mat4(1.0);
	SeaLionModel = glm::translate(SeaLionModel, glm::vec3(-2.0f, -1.0f, 0.0f));
	SeaLionModel = glm::scale(SeaLionModel, glm::vec3(0.01f));  // Changed from 0.3f to 0.01f
	// Set the model matrix in the shader
	objShader.SetMat4("model", SeaLionModel);
	animals[1].Draw(objShader);

	placeWolf();

	generateTrees();



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
		plants[1].Draw(objShader);
	}


	glm::mat4 catModel = glm::mat4(1.0);
	catModel = glm::translate(catModel, glm::vec3(0.f, -1.f, 2.5f));
	catModel = glm::scale(catModel, glm::vec3(0.01f));
	//catModel = glm::rotate(catModel, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));// iti roteste invers modelul
	//catModel = glm::rotate(catModel, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));// iti roteste introparte  modelul
	catModel = glm::rotate(catModel, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));// iti roteste introparte  modelul
	objShader.SetMat4("model", catModel);
	animals[3].Draw(objShader);


	glm::mat4 tree2Model = glm::mat4(1.0);
	tree2Model = glm::translate(tree2Model, glm::vec3(0.f, -1.f, 2.5f));
	//tree2Model = glm::scale(tree2Model, glm::vec3(0.3f));
	objShader.SetMat4("model", tree2Model);
	plants[2].Draw(objShader);

	glm::mat4 tree3Model = glm::mat4(1.0);
	tree3Model = glm::translate(tree3Model, glm::vec3(0.f, -3.f, 0.f));
	tree3Model = glm::scale(tree3Model, glm::vec3(0.5f));
	objShader.SetMat4("model", tree3Model);
	plants[3].Draw(objShader);

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
	animals[13].Draw(objShader);  // Assuming it's the 14th model (index 13)

	//glm::mat4 horseModel = glm::mat4(1.0);
	//horseModel = glm::translate(horseModel, glm::vec3(0.f, -1.f, 2.5f));
	//horseModel = glm::scale(horseModel, glm::vec3(100.0f));
	//objShader.SetMat4("model", horseModel);
	//models[9].Draw(objShader);

	//bool move = false;
	//for (int i = 0; i < 5; i++)
	//{
	//	glm::mat4 deerModel = glm::mat4(1.0);
	//	if (i % 2 == 0)
	//		deerModel = glm::translate(deerModel, glm::vec3(i * 0.6f + 12.f, -1.f, 15.5f + move));
	//	else
	//		deerModel = glm::translate(deerModel, glm::vec3(i * 0.6f + 12.f, -1.f, 15.f + move));

	//	deerModel = glm::scale(deerModel, glm::vec3(0.04f));
	//	deerModel = glm::rotate(deerModel, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//	deerModel = glm::rotate(deerModel, glm::radians(270.0f), glm::vec3(0.f, 0.0f, 1.0f));
	//	deerModel = glm::rotate(deerModel, glm::radians(-90.0f), glm::vec3(0.f, 0.0f, 1.0f));
	//	objShader.SetMat4("model", deerModel);
	//	animals[5].Draw(objShader);
	//	if (i % 3 == 0)
	//		move = !move;
	//}

	placeDeer();
	generateFences();

	for (int i = 1;i <=3;i++) {
		glm::mat4 lampModel = glm::mat4(1.0);
		lampModel = glm::translate(lampModel, glm::vec3(10.f, -1.4f,i* 10.6f));
		lampModel = glm::scale(lampModel, glm::vec3(0.1f));

		glm::vec3 lightPos = glm::vec3(10.0f, -1.4f, i * 10.6f);
		objShader.SetVec3("lightPos", lightPos);
		objShader.SetVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		objShader.SetMat4("model", lampModel);
		structures[1].Draw(objShader);
	}

	for (int i = 1; i <= 3; i++) {
		glm::mat4 benchModel = glm::mat4(1.0);
		// Position to the right of the lamp (+2 units on x axis), same z position as lamps
		benchModel = glm::translate(benchModel, glm::vec3(10.f, -1.4f, (i * 10.6f) - 1.5f));
		// Rotate 270 degrees so the bench faces away from the fence
		benchModel = glm::rotate(benchModel, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		// Reduce the scale to make it smaller
		benchModel = glm::scale(benchModel, glm::vec3(0.8f));
		objShader.SetMat4("model", benchModel);
		structures[structures.size() - 1].Draw(objShader);
	}

	

	glm::mat4 lionModel = glm::mat4(1.0);
	lionModel = glm::translate(lionModel, glm::vec3(14.f, -0.8f, 67.f));
	lionModel = glm::scale(lionModel, glm::vec3(7.f));
	lionModel = glm::rotate(lionModel, glm::radians(-60.f), glm::vec3(0.f, 1.0f, 0.0f));
	objShader.SetMat4("model", lionModel);
	animals[7].Draw(objShader);//14

	glm::mat4 bearModel = glm::mat4(1.0);
	bearModel = glm::translate(bearModel, glm::vec3(-8.f, -0.5f, 13.f));
	bearModel = glm::scale(bearModel, glm::vec3(10.f));
	objShader.SetMat4("model", bearModel);
	animals[8].Draw(objShader);

	for (int i = 1; i <= 6; i+=2) {
			glm::mat4 zebraModel = glm::mat4(1.0);
			zebraModel = glm::translate(zebraModel, glm::vec3(i * 0.6f + 12.f, -1.f, 25.5f + i*2));
			zebraModel = glm::scale(zebraModel, glm::vec3(10.f));
			zebraModel = glm::rotate(zebraModel, glm::radians(-35.0f*i), glm::vec3(0.f, 1.0f, 0.0f));
			objShader.SetMat4("model", zebraModel);
			animals[9].Draw(objShader);
	}

	glm::mat4 apeModel = glm::mat4(1.0);
	apeModel = glm::translate(apeModel, glm::vec3(15.f, -1.f, 80.f));
	apeModel = glm::scale(apeModel, glm::vec3(11.f));
	objShader.SetMat4("model", apeModel);
	animals[10].Draw(objShader);

	glm::mat4 ape2Model = glm::mat4(1.0);
	ape2Model = glm::translate(ape2Model, glm::vec3(17.f, -0.8f, 85.f));
	ape2Model = glm::rotate(ape2Model, glm::radians(-90.f), glm::vec3(0.f, 1.0f, 0.0f));
	ape2Model = glm::scale(ape2Model, glm::vec3(1.6f));
	objShader.SetMat4("model", ape2Model);
	animals[11].Draw(objShader);


	placeKangaroo();


	// ********************************************************************************************************************

	glm::mat4 hayBaleModel = glm::mat4(1.0);
	hayBaleModel = glm::translate(hayBaleModel, glm::vec3(7.f, -1.f, 2.f));
	hayBaleModel = glm::scale(hayBaleModel, glm::vec3(0.5f));
	objShader.SetMat4("model", hayBaleModel);
	plants[4].Draw(objShader);


	glm::mat4 bush2Model = glm::mat4(1.0);
	bush2Model = glm::translate(bush2Model, glm::vec3(11.f, -1.f, 2.f));
	bush2Model = glm::scale(bush2Model, glm::vec3(1.f));
	objShader.SetMat4("model", bush2Model);
	plants[5].Draw(objShader);

	glm::mat4 bushModel = glm::mat4(1.0);
	bushModel = glm::translate(bushModel, glm::vec3(10.f, -1.f, 2.f));
	bushModel = glm::scale(bushModel, glm::vec3(0.3f));
	objShader.SetMat4("model", bushModel);
	plants[6].Draw(objShader);

	glm::mat4 stationModel = glm::mat4(1.0);
	stationModel = glm::translate(stationModel, glm::vec3(20.5f, -1.2f, 50.f));
	stationModel = glm::rotate(stationModel, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	stationModel = glm::scale(stationModel, glm::vec3(3.f));
	objShader.SetMat4("model", stationModel);
	structures[2].Draw(objShader);

	generateWalls();

	glm::mat4 crocodileModel = glm::mat4(1.0);
	// Keep same position
	crocodileModel = glm::translate(crocodileModel, glm::vec3(3.0f, -1.0f, 0.0f));
	// Keep same scale
	crocodileModel = glm::scale(crocodileModel, glm::vec3(2.0f));
	// Let's do a single rotation sequence to get it oriented correctly
	crocodileModel = glm::rotate(crocodileModel, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // Turn it to face forward
	objShader.SetMat4("model", crocodileModel);
	animals[14].Draw(objShader);


	//glm::mat4 fence2Model = glm::mat4(1.0);
	//fence2Model = glm::translate(fence2Model, glm::vec3(1.f, -1.5f, 40.f));
	//fence2Model = glm::rotate(fence2Model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	//fence2Model = glm::scale(fence2Model, glm::vec3(0.01f));
	//objShader.SetMat4("model", fence2Model);
	//models[26].Draw(objShader);



	// ********************************************************************************************************************
	// the grround must be placed at the end 

	glm::mat4 terrainModel1 = glm::mat4(1.0);
	terrainModel1 = glm::translate(terrainModel1, glm::vec3(8.0f, -2.0f, 10.0f));
	terrainModel1 = glm::scale(terrainModel1, glm::vec3(5.0f));
	terrainModel1 = glm::rotate(terrainModel1, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	objShader.SetMat4("model", terrainModel1);

	terrain->Draw(objShader);
	glm::mat4 terrainModel = glm::mat4(1.0);
	terrainModel = glm::translate(terrainModel, glm::vec3(8.0f, -2.0f, 40.f));
	terrainModel = glm::scale(terrainModel, glm::vec3(-5.0f, 5.0f, 5.0f));
	objShader.SetMat4("model", terrainModel);
	terrain->Draw(objShader);

	glm::mat4 terrainModel2 = glm::mat4(1.0);
	terrainModel2 = glm::translate(terrainModel2, glm::vec3(8.0f, -2.0f, 70.0f));
	terrainModel2 = glm::scale(terrainModel2, glm::vec3(5.0f));
	terrainModel2 = glm::rotate(terrainModel2, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	objShader.SetMat4("model", terrainModel2);
	terrain->Draw(objShader);

	glm::mat4 terrainModel3 = glm::mat4(1.0);
	terrainModel3 = glm::translate(terrainModel3, glm::vec3(8.0f, -2.0f, 100.f));
	terrainModel3 = glm::scale(terrainModel3, glm::vec3(-5.0f, 5.0f, 5.0f));
	//terrainModel3 = glm::rotate(terrainModel3, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	objShader.SetMat4("model", terrainModel3);
	terrain->Draw(objShader);
	

	//glm::mat4 butterflyModel = glm::mat4(1.0);
	//// Position the butterfly above tree2 which is at (0.0, -1.0, 2.5)
	//butterflyModel = glm::translate(butterflyModel, glm::vec3(0.0f, 2.0f, 2.5f));  // Same x and z as tree2, but higher y
	//// Make it visible but not too large
	//butterflyModel = glm::scale(butterflyModel, glm::vec3(0.3f));
	//// Rotate it to face the right direction
	//butterflyModel = glm::rotate(butterflyModel, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//objShader.SetMat4("model", butterflyModel);
	//models[models.size() - 1].Draw(objShader);

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
	characters.emplace_back(PiratePath, false);


	std::string GiraffePath = currentPath + "\\Models\\Giraffe\\CIL1PYJ81IH0BT4B9ME2F53L7.obj";
	//std::string GiraffeTexturePath = currentPath + "\\Models\\Giraffe\\Giraffe.jpg";
	animals.emplace_back(GiraffePath, false);

	std::string SeaLionPath = currentPath + "\\Models\\SeaLion\\10041_sealion_v1_L3.obj";
	animals.emplace_back(SeaLionPath, false);

	std::string WolfPath = currentPath + "\\Models\\Wolf\\Wolf_One_obj.obj";
	animals.emplace_back(WolfPath, false);
	std::string TreePath = currentPath + "\\Models\\Tree\\Tree2.obj";
	plants.emplace_back(TreePath, false);
	std::string GrassPath = currentPath + "\\Models\\Grass\\Grass.obj";
	plants.emplace_back(GrassPath, false);

	std::string CatPath = currentPath + "\\Models\\Cat\\Cat.obj";
	animals.emplace_back(CatPath, false);
	std::string Tree2Path = currentPath + "\\Models\\Tree2\\Tree.obj";
	plants.emplace_back(Tree2Path, false);
	std::string Tree3Path = currentPath + "\\Models\\Tree3\\tree 1.obj";
	plants.emplace_back(Tree3Path, false);

	std::string HorsePath = currentPath + "\\Models\\Horse\\horse.obj";
	animals.emplace_back(HorsePath, false);

	std::string DeerPath = currentPath + "\\Models\\Deer\\deer.obj";
	animals.emplace_back(DeerPath, false);

	std::string FencePath = currentPath + "\\Models\\Fence\\Fence.obj";
	structures.emplace_back(FencePath, false);

	std::string LampPath = currentPath + "\\Models\\Lamp\\lamp.obj";
	structures.emplace_back(LampPath, false);

	std::string ElephantPath = currentPath + "\\Models\\Elephant\\elephant.obj";
	animals.emplace_back(ElephantPath, false);

	std::string lionPath = currentPath + "\\Models\\lion\\lion.obj";
	animals.emplace_back(lionPath, false);
	std::string bearPath = currentPath + "\\Models\\bear\\bear.obj";
	animals.emplace_back(bearPath, false);

	std::string zebraPath = currentPath + "\\Models\\zebra\\zebra.obj";
	animals.emplace_back(zebraPath, false);



	std::string apePath = currentPath + "\\Models\\ape\\ape.obj";
	animals.emplace_back(apePath, false);

	std::string ape2Path = currentPath + "\\Models\\ape2\\ape2.obj";
	animals.emplace_back(ape2Path, false);

	std::string kangarooPath = currentPath + "\\Models\\kangaroo\\kangaroo.obj";
	animals.emplace_back(kangarooPath, false);

	//yes, this thing is actually a plant
	std::string hayBalePath = currentPath + "\\Models\\hayBale\\hayBale.obj";
	plants.emplace_back(hayBalePath, false);

	std::string bush2Path = currentPath + "\\Models\\bush2\\bush2.obj";
	plants.emplace_back(bush2Path, false);

	std::string bushPath = currentPath + "\\Models\\bush\\bush.obj";
	plants.emplace_back(bushPath, false);

	std::string stationPath = currentPath + "\\Models\\station\\station.obj";
	structures.emplace_back(stationPath, false);

	std::string CondorPath = currentPath + "\\Models\\Condor\\CONDOR.OBJ";
	animals.emplace_back(CondorPath, false);

	std::string WallPath = currentPath + "\\Models\\Wall\\Wall.obj";
	structures.emplace_back(WallPath, false);

	std::string crocodilePath = currentPath + "\\Models\\Crocodile\\ImageToStl.com_crocodile.obj";
	animals.emplace_back(crocodilePath, false);

	/*std::string butterflyPath = currentPath + "\\Models\\Butterfly\\ImageToStl.com_monarch butterfly.obj";
	models.emplace_back(butterflyPath, false);*/

	std::string Fence2Path = currentPath + "\\Models\\Fence2\\Fence2.obj";
	structures.emplace_back(Fence2Path, false);


	std::string butterflyPath = currentPath + "\\Models\\Butterfly\\ImageToStl.com_monarch butterfly.obj";
	animals.emplace_back(butterflyPath, false);

	std::string benchPath = currentPath + "\\Models\\bench\\ImageToStl.com_outdoor bench 01.obj";
	structures.emplace_back(benchPath, false);




	//********************************************************************************************************************
	// Load the terrain model
	std::string Terrain = currentPath + "\\Models\\Terrain\\terrainBlender.obj";
	terrain = std::make_unique<Model>(Terrain, false);

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

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		deltaTime *= 2;

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


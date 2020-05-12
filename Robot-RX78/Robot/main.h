#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
using namespace std;
//#include <cmath>

#include "vgl.h"
#include "LoadShaders.h"
#include "objloader.hpp"
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
using namespace glm;



#define PARTSNUM 6
///#define PARTSNUM 18
#define BODY 0
#define LEFTSHOUDER 1
#define ULEFTARM 2
#define DLEFTARM 3
#define LEFTHAND 4

void updateModels();

void init();
#pragma region Cubemap shader
typedef struct CubemapTexture
{
	GLenum type;
	std::string fileName;
};
typedef struct _TextureData
{
	_TextureData() : width(0), height(0), data(0) {}
	int width;
	int height;
	unsigned char* data;
} TextureData;

void initCubemapShader();
void drawCubemapShader();
TextureData Load_png(const char* path, bool mirroredY);
#pragma endregion

#pragma region windowShader
//	uniform attribute
GLint fxRadiusID;
float fxRadius = 0;
GLint colorErrorID;
float colorError = 0;
GLint zaTimeID;
float zaTime = 0.0f;

GLuint windowProgram;
GLuint window_vao, window_vbo;
GLuint			FBODataTexture;
GLuint			FBO;
GLuint			depthRBO;
static const GLfloat window_positions[] =
{
	1.0f,-1.0f,1.0f,0.0f,
	-1.0f,-1.0f,0.0f,0.0f,
	-1.0f,1.0f,0.0f,1.0f,
	1.0f,1.0f,1.0f,1.0f
};
void initWindowShader();
#pragma endregion
#pragma region uiEvents
bool ZawarudoShowing = false;
float ZawarudoShowTime = 0;
float ZawarudoTotalShowTime = 2.0f;
bool ShowLegs = false;
#pragma endregion


void ChangeSize(int w,int h);
void display();
void Keyboard(unsigned char key, int x, int y);
void Mouse(int button,int state,int x,int y);
void My_Mouse_Moving(int x, int y);

void menuEvents(int option);
void InstanceMenuEvents(int option);
void ActionMenuEvents(int option);
void BodyMovementMenuEvents(int option);
void ExtraMenuEvents(int option);

void idle(int dummy);

mat4 translate(float x,float y,float z);
mat4 scale(float x,float y,float z);
mat4 rotate(float angle,float x,float y,float z);

void Obj2Buffer();
void load2Buffer( char* obj,int);

void updateObj(float);
void resetObj();

bool isFrame;

GLuint VAO;
GLuint VBO;
GLuint uVBO;
GLuint nVBO;
GLuint mVBO;
GLuint UBO;
GLuint VBOs[PARTSNUM];
GLuint uVBOs[PARTSNUM];
GLuint nVBOs[PARTSNUM];
GLuint program;
GLuint program_zawarudo;
GLuint InstanceVBO;
GLuint InstanceVBOY;
glm::vec3 instanceOffset[100];
GLfloat instanceOffsetY[100];
int instanceAmount = 1;

int pNo;

float angles[PARTSNUM];
float positionY = -25;
float angle2 = 0.0;
float eyeAngley = 0.0;
float eyedistance = 100;
float size = 1;
GLfloat movex,movey;
GLint MatricesIdx;
GLuint ModelID;
GLuint DeformRotationID;
GLuint IsBodyID;
///	time
GLint fTime;
float fakeTime = 1.0f;
float timeSpeed = 1.0f;		//	0 ~ 1
float armRotateAngle = 0.0f;
mat4 bodyRotateMatrix;

int vertices_size[PARTSNUM];
int uvs_size[PARTSNUM];
int normals_size[PARTSNUM];
int materialCount[PARTSNUM];

std::vector<std::string> mtls[PARTSNUM];//use material
std::vector<unsigned int> faces[PARTSNUM];//face count
map<string,vec3> KDs;//mtl-name&Kd
map<string,vec3> KSs;//mtl-name&Ks

mat4 Projection ;
mat4 View;
mat4 Model;
mat4 Models[PARTSNUM];
mat4 DeformRotation;

#define DOR(angle) (angle*3.1415/180);

#define leftHand 0
#define rightHand 1
#define leftFoot 2
#define rightFoot 3
#define WALK 1
#define IDLE 0
#define PI 3.14159265
int mode;
int action = WALK;
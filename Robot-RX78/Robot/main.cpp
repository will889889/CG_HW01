#include "main.h"


vec3 camera = vec3(0,0,30);
int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitContextVersion(4,3);//�HOpenGL version4.3���������
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);//�O�_�V�U�ۮe,GLUT_FORWARD_COMPATIBLE���䴩(?
	glutInitContextProfile(GLUT_CORE_PROFILE);

	//multisample for golygons smooth
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH|GLUT_MULTISAMPLE);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("OpenGL 4.3 - Robot");

	glewExperimental = GL_TRUE; //�m��glewInit()���e
	if (glewInit()) {
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;//c error
		exit(EXIT_FAILURE);
	}

	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	///glCullFace(GL_BACK);
	///glEnable(GL_CULL_FACE);


	///	init cubemap shader
	glDepthFunc(GL_LEQUAL);
	///glEnable(GL_TEXTURE_2D);
	///glEnable(GL_BLEND);
	///glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	///	cubemap shader
	cout << "initCubemapShader\n";
	initCubemapShader();
	///

	init();
	glutDisplayFunc(display);
	glutReshapeFunc(ChangeSize);
	glutKeyboardFunc(Keyboard);
	glutPassiveMotionFunc(My_Mouse_Moving);
	glutMotionFunc(My_Mouse_Moving);

	int ActionMenu,BodyMovementMenu,ExtraMenu;

	ActionMenu = glutCreateMenu(ActionMenuEvents);//�إߥk����
	glutAddMenuEntry("ZA WARUDO", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//�P�k�����p

	BodyMovementMenu = glutCreateMenu(BodyMovementMenuEvents);//�إߥk����
	glutAddMenuEntry("Arms", 0);
	glutAddMenuEntry("Body", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//�P�k�����p

	ExtraMenu = glutCreateMenu(ExtraMenuEvents);
	glutAddMenuEntry("Toggle Legs",0);
	glutAddMenuEntry("Switch Skybox",1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//�P�k�����p


	glutCreateMenu(menuEvents);//�إߥk����
	glutAddSubMenu("Action",ActionMenu);
	glutAddSubMenu("Body Movement", BodyMovementMenu);
	glutAddSubMenu("Extra",ExtraMenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//�P�k�����p

	glutMouseFunc(Mouse);
	glutTimerFunc (33, idle, 0); 
	glutMainLoop();
	return 0;
}

float screenW = 0, screenH = 0;
void ChangeSize(int w,int h){
	if(h == 0) h = 1;
	glViewport(0,0,w,h);
	screenW = w;
	screenH = h;
	Projection = perspective(80.0f,(float)w/h,0.1f,1000.0f);

#pragma region windowShader - fbo & texture?
	glDeleteRenderbuffers(1, &depthRBO);
	glDeleteTextures(1, &FBODataTexture);
	glGenRenderbuffers(1, &depthRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, w, h);

	glGenTextures(1, &FBODataTexture);
	glBindTexture(GL_TEXTURE_2D, FBODataTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBODataTexture, 0);
#pragma endregion


}
void Mouse(int button,int state,int x,int y){
	if(button == 2) isFrame = false;
}
void My_Mouse_Moving(int x, int y) {
	x = x - screenW / 2.0f;
	y = y - screenH / 2.0f;
	float distance = sqrtf(x*x + y * y);

	if (!ZawarudoShowing)
	{
		timeSpeed = 1.0f - (distance) / 500.0f;
		if (timeSpeed < 0)
			timeSpeed = 0;
	}

	/*if (distance < 100)
		timeSpeed = 1.0f;
	else if (distance >= 300)
		timeSpeed = 0;
	else
		timeSpeed = 1.0f - (distance - 100.0f) / 200.0f;*/
	//cout << "x: " << (x) << ", y: " << y << "\n";
	//cout << "L = " << distance << "\n";
	//cout << "screenW: " << screenW << ", screenH: " << screenH << "\n";
	
}

//	update	(called per 33ms)
void idle(int dummy){
	float deltaTime = 0.033f;

	//	zawarudo	(use real deltatime)
	if (ZawarudoShowing && ZawarudoShowTime > 0)
	{
		ZawarudoShowTime -= deltaTime;

		//	0 ~ 1
		float process = 1.0f - (ZawarudoShowTime / ZawarudoTotalShowTime);

		float raduisT = DOR(process * 180.0f);
		float speedT = DOR((process * 90.0f + 90.0f));

		fxRadius = 1.0f * sinf(raduisT);
		timeSpeed = sinf(speedT);
		cout << "speedT = " << speedT << "\n";

		if (ZawarudoShowTime <= 0)
		{
			timeSpeed = 0;
			fxRadius = 0;
			ZawarudoShowing = false;
		}
	}


	deltaTime *= timeSpeed;
	fakeTime += deltaTime;
	zaTime += deltaTime;
	//	particle time elapse
	//for (int i = 0; i < 100; i++)
	//{
	//	instanceOffsetY[i] = 0;
	//	/*instanceOffsetY[i] += timeStep;
	//	if (instanceOffsetY[i] >= timeCycle)
	//		instanceOffsetY[i] -= timeCycle;*/
	//}

	//	IDK what's this
	isFrame = true;

	//	update model movements
	if(action == WALK){
		updateObj(deltaTime);
	}
	else if(action == IDLE){
		resetObj();
	}

	//	draw stuff
	glutPostRedisplay();
	
	//	keep update...
	glutTimerFunc (33, idle, 0); 
}
void resetObj(){
	//	reset Arm rotation
	//armRotateAngle = 0;

	//	reset deform rotation
	//...
}

#define DOR(angle) (angle*3.1415/180);

float armTime = 0;
float armTimeScale = 30.0f;
float bodyTime = 0;
float bodyTimeScale = 15.0f;
void updateObj(float deltaTime){
	//	Arm rotation
	armTime += deltaTime;
	armRotateAngle = (90.0f * sin(armTime * armTimeScale)) + 90.0f;	//	between 0 ~ 180

	//	Body deform matrix
	bodyTime += deltaTime;
	// float rotateAngle = 1.0f * (1.0f - abs(cos(bodyTime * bodyTimeScale)));
	// rotateAngle = rotateAngle < 0.2f ? 0.2f : rotateAngle;
	bodyRotateMatrix = rotate(0.5f, cos(bodyTime * bodyTimeScale), 0, sin(bodyTime * bodyTimeScale));
}


 GLuint M_KaID;
 GLuint M_KdID;
 GLuint M_KsID;

void init(){

	isFrame = false;
	pNo = 0;
	for(int i = 0;i<PARTSNUM;i++)//��l�ƨ��װ}�C
		angles[i] = 0.0;

	//VAO
	glGenVertexArrays(1,&VAO);
	glBindVertexArray(VAO);

	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "DSPhong_Material.vp" },//vertex shader
		{ GL_FRAGMENT_SHADER, "DSPhong_Material.fp" },//fragment shader
		{ GL_NONE, NULL }};
	program = LoadShaders(shaders);//Ū��shader

	glUseProgram(program);//uniform�ѼƼƭȫe������use shader
	
	MatricesIdx = glGetUniformBlockIndex(program,"MatVP");
	ModelID =  glGetUniformLocation(program,"Model");
	DeformRotationID = glGetUniformLocation(program, "DeformRotation");
	IsBodyID = glGetUniformLocation(program, "isBody");
    M_KaID = glGetUniformLocation(program,"Material.Ka");
	M_KdID = glGetUniformLocation(program,"Material.Kd");
	M_KsID = glGetUniformLocation(program,"Material.Ks");
	fTime = glGetUniformLocation(program, "fTime");

	DeformRotation = rotate(1, 0, 0, 1);

	//or
	M_KdID = M_KaID+1;
	M_KsID = M_KaID+2;

	Projection = glm::perspective(80.0f,4.0f/3.0f,0.1f,1000.0f);
	//glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
	
	// Camera matrix
	View       = glm::lookAt(
		glm::vec3(0,5,25) , // Camera is at (0,10,25), in World Space
		glm::vec3(0,0,0), // and looks at the origin
		glm::vec3(0,1,0)  // Head is up (set to 0,1,0 to look upside-down)
		);

	Obj2Buffer();

	//UBO
	glGenBuffers(1,&UBO);
	glBindBuffer(GL_UNIFORM_BUFFER,UBO);
	glBufferData(GL_UNIFORM_BUFFER,sizeof(mat4)*2,NULL,GL_DYNAMIC_DRAW);
	//get uniform struct size
	int UBOsize = 0;
	glGetActiveUniformBlockiv(program, MatricesIdx, GL_UNIFORM_BLOCK_DATA_SIZE, &UBOsize);  
	//bind UBO to its idx
	glBindBufferRange(GL_UNIFORM_BUFFER,0,UBO,0,UBOsize);
	glUniformBlockBinding(program, MatricesIdx,0);

	//	init instanceOffset
	int randCuts = 100;
	for (int i = 0; i < 100; i++)
	{
		glm::vec3 offset;
		offset.x = 0;
		offset.y = 0;
		offset.z = 0;
		/*offset.x = (rand() % 2) ? (rand() % randCuts) : -(rand() % randCuts);
		offset.y = (rand() % 2) ? (rand() % randCuts) : -(rand() % randCuts);
		offset.z = (rand() % 2) ? (rand() % randCuts) : -(rand() % randCuts);*/
		instanceOffset[i] = offset;
	}
	//	instance VBO
	glGenBuffers(1, &InstanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, InstanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 100, &instanceOffset[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//	init instanceOffsetY
	for (int i = 0; i < 100; i++)
	{
		instanceOffsetY[i] = 1.0f * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
	}
	//	instance VBO
	glGenBuffers(1, &InstanceVBOY);
	glBindBuffer(GL_ARRAY_BUFFER, InstanceVBOY);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 100, &instanceOffsetY[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	///	windowShader
	initWindowShader();

	glClearColor(0.0,0.0,0.0,1);//black screen
}

void display(){

	///
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	///
	static const GLfloat green[] = { 0.0f, 0.25f, 0.0f, 1.0f };
	static const GLfloat one = 1.0f;
	glClearBufferfv(GL_COLOR, 0, green);
	glClearBufferfv(GL_DEPTH, 0, &one);


	float eyey = DOR(eyeAngley);
	View = lookAt(
		vec3(eyedistance*sin(eyey), 0, eyedistance*cos(eyey)), // Camera is at (0,0,20), in World Space
		vec3(0, 0, 0), // and looks at the origin
		vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	///	cubemap
	drawCubemapShader();

	glBindVertexArray(VAO);
	glUseProgram(program);//uniform�ѼƼƭȫe������use shader
	
	updateModels();

	//update data to UBO for MVP
	glBindBuffer(GL_UNIFORM_BUFFER,UBO);
	glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(mat4),&View);
	glBufferSubData(GL_UNIFORM_BUFFER,sizeof(mat4),sizeof(mat4),&Projection);
	glBindBuffer(GL_UNIFORM_BUFFER,0);

	glUniformMatrix4fv(DeformRotationID, 1, GL_FALSE, &DeformRotation[0][0]);
	GLuint offset[3] = {0,0,0};//offset for vertices , uvs , normals
	for(int i = 0;i < PARTSNUM ;i++){
		glUniformMatrix4fv(ModelID,1,GL_FALSE,&Models[i][0][0]);
		glUniform1i(IsBodyID, i);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,				//location
							  3,				//vec3
							  GL_FLOAT,			//type
							  GL_FALSE,			//not normalized
							  0,				//strip
							  (void*)offset[0]);//buffer offset
		//(location,vec3,type,�T�w�I,�s���I�������q,buffer point)
		offset[0] +=  vertices_size[i]*sizeof(vec3);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);//location 1 :vec2 UV
		glBindBuffer(GL_ARRAY_BUFFER, uVBO);
		glVertexAttribPointer(1, 
							  2, 
							  GL_FLOAT, 
							  GL_FALSE, 
							  0,
							  (void*)offset[1]);
		//(location,vec2,type,�T�w�I,�s���I�������q,point)
		offset[1] +=  uvs_size[i]*sizeof(vec2);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);//location 2 :vec3 Normal
		glBindBuffer(GL_ARRAY_BUFFER, nVBO);
		glVertexAttribPointer(2,
							  3, 
							  GL_FLOAT, 
							  GL_FALSE, 
							  0,
							  (void*)offset[2]);
		//(location,vec3,type,�T�w�I,�s���I�������q,point)
		offset[2] +=  normals_size[i]*sizeof(vec3);

		//	4th attribute buffer : instance offset
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, InstanceVBO);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glVertexAttribDivisor(3, 1);

		//	4th attribute buffer : instance offset Y
		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, InstanceVBOY);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 100, &instanceOffsetY[0], GL_STATIC_DRAW);
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (GLvoid*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glVertexAttribDivisor(4, 1);

		////	5th attribute buffer : time
		//glEnableVertexAttribArray(4);
		//glBindBuffer(GL_ARRAY_BUFFER, InstanceVBO);
		//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glVertexAttribDivisor(3, 1);

		glUniform1f(fTime, fakeTime);

		int vertexIDoffset = 0;//glVertexID's offset 
		string mtlname;//material name
		vec3 Ks = vec3(1,1,1);//because .mtl excluding specular , so give it here.
		for(int j = 0;j <mtls[i].size() ;j++){//
			mtlname = mtls[i][j];	
			//find the material diffuse color in map:KDs by material name.
			glUniform3fv(M_KdID,1,&KDs[mtlname][0]);
			glUniform3fv(M_KsID,1,&Ks[0]);
			//          (primitive   , glVertexID base , vertex count    )
			glDrawArraysInstanced(GL_TRIANGLES, vertexIDoffset, faces[i][j + 1] * 3, instanceAmount);
			///glDrawArrays(GL_TRIANGLES, vertexIDoffset  , faces[i][j+1]*3);
			//we draw triangles by giving the glVertexID base and vertex count is face count*3
			vertexIDoffset += faces[i][j+1]*3;//glVertexID's base offset is face count*3
		}//end for loop for draw one part of the robot	
		
	}//end for loop for updating and drawing model

	///	window shader render
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, FBODataTexture);
	glBindVertexArray(window_vao);
	glUseProgram(windowProgram);
	glUniform1f(fxRadiusID, fxRadius);
	glUniform1f(colorErrorID, colorError);
	glUniform1f(zaTimeID, zaTime);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glutSwapBuffers();

	///glFlush();//�j�����W����OpenGL commands
	///glutSwapBuffers();//�մ��e�x�M��xbuffer ,����Obuffer�e���M�e�xbuffer�洫�ϧڭ̬ݨ���
}

void Obj2Buffer(){
	std::vector<vec3> Kds;
	std::vector<vec3> Kas;
	std::vector<vec3> Kss;
	std::vector<std::string> Materials;//mtl-name
	std::string texture;
	//loadMTL("Obj/Arm_L2.mtl", Kds, Kas, Kss, Materials, texture);
	loadMTL("Obj/BodyPureFace.mtl", Kds, Kas, Kss, Materials, texture);
	///loadMTL("Obj/android.mtl", Kds, Kas, Kss, Materials, texture);
	///loadMTL("Obj/gundam.mtl",Kds,Kas,Kss,Materials,texture);
	//printf("%d\n",texture);
	for(int i = 0;i<Materials.size();i++){
		string mtlname = Materials[i];
		//  name            vec3
		KDs[mtlname] = Kds[i];
	}

	load2Buffer("Obj/BodyPureFace.obj", 0);
	load2Buffer("Obj/Arm_L2.obj", 1);
	load2Buffer("Obj/Arm_R2.obj", 2);
	load2Buffer("Obj/Helmet.obj", 3);


	///load2Buffer("Obj/android.obj", 0);
	/*load2Buffer("Obj/android.obj", 1);
	load2Buffer("Obj/android.obj", 2);*/
	
	GLuint totalSize[3] = {0,0,0};
	GLuint offset[3] = {0,0,0};
	for(int i = 0;i < PARTSNUM ;i++){
		totalSize[0] += vertices_size[i]*sizeof(vec3);
		totalSize[1] += uvs_size[i] * sizeof(vec2);
		totalSize[2] += normals_size[i] * sizeof(vec3);
	}
	//generate vbo
	glGenBuffers(1,&VBO);
	glGenBuffers(1,&uVBO);
	glGenBuffers(1,&nVBO);
	//bind vbo ,�Ĥ@��bind�]�P���� create vbo 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);//VBO��target�OGL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER,totalSize[0],NULL,GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, uVBO);//VBO��target�OGL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER,totalSize[1],NULL,GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, nVBO);//VBO��target�OGL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER,totalSize[2],NULL,GL_STATIC_DRAW);
	
	
	for(int i = 0;i < PARTSNUM ;i++){
		glBindBuffer(GL_COPY_WRITE_BUFFER,VBO);
		glBindBuffer(GL_COPY_READ_BUFFER, VBOs[i]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,0,offset[0],vertices_size[i]*sizeof(vec3));
		offset[0] += vertices_size[i]*sizeof(vec3);
		glInvalidateBufferData(VBOs[i]);//free vbo
		glBindBuffer(GL_COPY_WRITE_BUFFER,0);

		glBindBuffer(GL_COPY_WRITE_BUFFER,uVBO);
		glBindBuffer(GL_COPY_READ_BUFFER, uVBOs[i]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,0,offset[1],uvs_size[i]*sizeof(vec2));
		offset[1] += uvs_size[i]*sizeof(vec2);
		glInvalidateBufferData(uVBOs[i]);//free vbo
		glBindBuffer(GL_COPY_WRITE_BUFFER,0);

		glBindBuffer(GL_COPY_WRITE_BUFFER,nVBO);
		glBindBuffer(GL_COPY_READ_BUFFER, nVBOs[i]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,0,offset[2],normals_size[i]*sizeof(vec3));
		offset[2] += normals_size[i] * sizeof(vec3);
		glInvalidateBufferData(uVBOs[i]);//free vbo
		glBindBuffer(GL_COPY_WRITE_BUFFER,0);
	}
	glBindBuffer(GL_COPY_WRITE_BUFFER,0);


}

void updateModels(){
	mat4 Rotatation[PARTSNUM];
	mat4 Translation[PARTSNUM];
	for(int i = 0 ; i < PARTSNUM;i++){
		Models[i] = mat4(1.0f);
		Rotatation[i] = mat4(1.0f);
		Translation[i] = mat4(1.0f); 
	}
	float r,pitch,yaw,roll;
	float alpha, beta ,gamma;
	DeformRotation = bodyRotateMatrix;

	//	0
	Rotatation[0] = rotate(0,1,0,0);
	Translation[0] = translate(0, positionY,0);
	Models[0] = Translation[0]*Rotatation[0]* scale(1.0f, 1.0f, 1.0f);

	//	armRotateAngle [0 ~ 180]
	//	1 - Arm_L
	Rotatation[1] = rotate(-armRotateAngle, 1, 0, 0);
	Translation[1] = translate(2, 11.0f + positionY, 0);
	mat4 dRotation = DeformRotation * translate(0, -positionY, 0);;
	{
		int level = 15;
		for (int i = 1; i < level; i++)
		{
			Translation[1] = DeformRotation * Translation[1];
			Translation[1] *= translate(0, 1.0f, 0);
		}
		// Translation[1] *= translate(0, positionY, 0);
		for (int i = 1; i < level; i++)
		{
			dRotation = DeformRotation * dRotation;
		}
	}
	Models[1] = Translation[1] * translate(0, positionY, 0) * dRotation * Rotatation[1] * scale(1.0f, 1.0f, 1.0f);

	//	2 - Arm_R
	Rotatation[2] = rotate(armRotateAngle - 180.0f, 1, 0, 0);
	Translation[2] = translate(-2, 11.0f + positionY, 0);
	dRotation = DeformRotation * translate(0, -positionY, 0);;
	{
		int level = 15;
		for (int i = 1; i < level; i++)
		{
			Translation[2] = DeformRotation * Translation[2];
			Translation[2] *= translate(0, 1.0f, 0);
		}
		// Translation[2] = Translation[2] * translate(0, 10.0f, 0);
		// Translation[2] *= translate(0, positionY, 0);
		for (int i = 1; i < level; i++)
		{
			dRotation = DeformRotation * dRotation;
		}
	}
	Models[2] = Translation[2] * translate(0, positionY, 0) * dRotation * Rotatation[2] * scale(1.0f, 1.0f, 1.0f);

	//	3 - Helmet
	Translation[3] = translate(0.2f , 27.4f + positionY, 0);
	dRotation = DeformRotation * translate(0, -positionY, 0);
	{
		int level = 17;
		for (int i = 1; i < level; i++)
		{
			Translation[3] = DeformRotation * Translation[3];
			Translation[3] *= translate(0, 1.0f, 0);
		}
		// Translation[3] *= translate(0, 1.0f, 0);
		for (int i = 1; i < level; i++)
		{
			dRotation = DeformRotation * dRotation;
		}
	}
	Models[3] = Translation[3] * translate(0, positionY, 0) * dRotation * scale(1.0f, 1.0f, 1.0f);
	return;


	//Body=======================================================
	Rotatation[0] = rotate(beta, 0, 1, 0);
	Translation[0] = translate(0, 2.9 + 0, 0);
	Models[0] = Translation[0] * Rotatation[0];
	//����=======================================================
	//���W���u
	yaw = DOR(beta);r = 3.7;
	alpha = angles[1];
	gamma = 10;
	Rotatation[1] = rotate(alpha,1,0,0)*rotate(gamma,0,0,1);//�V�e����*�V�k����
	Translation[1] = translate(3.7,1,-0.5);

	Models[1] = Models[0]*Translation[1]*Rotatation[1];
	
	//���ӻH
	Rotatation[4] = rotate(alpha,1,0,0)*rotate(gamma,0,0,1);//�V�e����*�V�k����
	Translation[4] =translate(3.7,1,-0.5);//�첾�쥪�W���u�B
	Models[4] =Models[0]*Translation[1]*Rotatation[1];
	
	//���U���u
	pitch = DOR(alpha);r = 3;
	roll = DOR(gamma);
	static int i=0;
	i+=5;
	alpha = angles[2]-20;
	//�W���u+�U���u�V�e����*�V�k����
	Rotatation[2] = rotate(alpha,1,0,0);
	//��x�b�첾�H�W���u���b�|����P��:translate(0,r*cos,r*sin)
	//��z�b�첾�H�W���u���b�|����:translate(r*sin,-rcos,0)
	Translation[2] = translate(0,-3,0);

	Models[2] = Models[1]*Translation[2]*Rotatation[2];
	

	pitch = DOR(alpha);
	//b = DOR(angles[2]);
	roll = DOR(gamma);
	//��x���׻P�U���u�ۦP
	//Rotatation[3] = Rotatation[2];
	//��x�b�첾�H�W���u���b�|����P��:translate(0,r*cos,r*sin) ,���׬��W���u+�U���u
	Translation[3] = translate(0,-4.8,0);
	Models[3] = Models[2]*Translation[3]*Rotatation[3];
	//=============================================================
}

void load2Buffer(char* obj,int i){
	std::vector<vec3> vertices;
	std::vector<vec2> uvs;
	std::vector<vec3> normals; // Won't be used at the moment.
	std::vector<unsigned int> materialIndices;

	bool res = loadOBJ(obj, vertices, uvs, normals,faces[i],mtls[i]);
	if(!res) printf("load failed\n");

	//glUseProgram(program);

	glGenBuffers(1,&VBOs[i]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
	glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(vec3),&vertices[0],GL_STATIC_DRAW);
	vertices_size[i] = vertices.size();

	//(buffer type,data�_�l��m,data size,data first ptr)
	//vertices_size[i] = glm_model->numtriangles;
	
	//printf("vertices:%d\n",vertices_size[);

	glGenBuffers(1,&uVBOs[i]);
	glBindBuffer(GL_ARRAY_BUFFER, uVBOs[i]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), &uvs[0], GL_STATIC_DRAW);
	uvs_size[i] = uvs.size();

	glGenBuffers(1,&nVBOs[i]);
	glBindBuffer(GL_ARRAY_BUFFER, nVBOs[i]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);
	normals_size[i] = normals.size();
}
mat4 translate(float x,float y,float z){
	vec4 t = vec4(x,y,z,1);//w = 1 ,�hx,y,z=0�ɤ]��translate
	vec4 c1 = vec4(1,0,0,0);
	vec4 c2 = vec4(0,1,0,0);
	vec4 c3 = vec4(0,0,1,0);
	mat4 M = mat4(c1,c2,c3,t);
	return M;
} 
mat4 scale(float x,float y,float z){
	vec4 c1 = vec4(x,0,0,0);
	vec4 c2 = vec4(0,y,0,0);
	vec4 c3 = vec4(0,0,z,0);
	vec4 c4 = vec4(0,0,0,1);
	mat4 M = mat4(c1,c2,c3,c4);
	return M;
}
mat4 rotate(float angle,float x,float y,float z){
	float r = DOR(angle);
	mat4 M = mat4(1);

	vec4 c1 = vec4(cos(r)+(1-cos(r))*x*x,(1-cos(r))*y*x+sin(r)*z,(1-cos(r))*z*x-sin(r)*y,0);
	vec4 c2 = vec4((1-cos(r))*y*x-sin(r)*z,cos(r)+(1-cos(r))*y*y,(1-cos(r))*z*y+sin(r)*x,0);
	vec4 c3 = vec4((1-cos(r))*z*x+sin(r)*y,(1-cos(r))*z*y-sin(r)*x,cos(r)+(1-cos(r))*z*z,0);
	vec4 c4 = vec4(0,0,0,1);
	M = mat4(c1,c2,c3,c4);
	return M;
}
void Keyboard(unsigned char key, int x, int y){
	switch(key){
	case '1':
		instanceAmount += 5;
		break;
	case '2':
		instanceAmount -= 5;
		if (instanceAmount < 0)
			instanceAmount = 0;
		break;
	case '3':
		bodyTimeScale += 0.1f;
		cout << "bodyTimeScale = " + std::to_string(bodyTimeScale) << "\n";

		break;
	case '4':
		bodyTimeScale -= 0.1f;

		break;
	case 'w':
		eyedistance -= 1;
		break;
	case 's':
		eyedistance += 1;
		cout << "eyedistance = " + std::to_string(eyedistance);
		break;
	case 'a':
		eyeAngley -=10;
		break;
	case 'd':
		eyeAngley +=10;
		break;
	case 'r':
		fxRadius += 0.05f;
		colorError += 0.1f;
		cout << "fxRadius = " + std::to_string(fxRadius) << "\n";
		break;
	case 't':
		fxRadius -= 0.05f;
		colorError -= 0.1f;
		cout << "fxRadius = " + std::to_string(fxRadius) << "\n";
		break;
	case 'q':
		positionY += 1.0f;
		break;
	case 'e':
		positionY -= 1.0f;
		cout << "positionY = " + std::to_string(positionY);


		break;
	}
	glutPostRedisplay();
}

void menuEvents(int option){}
void ActionMenuEvents(int option) {
	if (ZawarudoShowing == false)
	{
		ZawarudoShowTime = ZawarudoTotalShowTime;
		ZawarudoShowing = true;
	}
}
void BodyMovementMenuEvents(int option) {
	switch (option) {
	case 0:	//	arms
		armTimeScale = (armTimeScale == 0) ? 30.0f : 0.0f;
		break;
	case 1:	//	body
		bodyTimeScale = (bodyTimeScale == 0) ? 15.0f : 0.0f;
		break;
	}
}

void ExtraMenuEvents(int option){
	switch(option){
	case 0:	//	legs

		break;
	case 1:	//	skybox

		break;
	}
}


#pragma region CubemapShader
string imagePath = "./Imgs/";
GLuint cubemapProgram;
GLuint cubemapTextureID;
GLuint cubemapUm4mvLocation;
GLuint cubemapUm4pLocation;
GLuint cubemap_vao, cubemap_vbo, cubemap_ebo;
void initCubemapShader()
{
	//	create program
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "cubemap.vs.glsl" },//vertex shader
		{ GL_FRAGMENT_SHADER, "cubemap.fs.glsl" },//fragment shader
		{ GL_NONE, NULL } };
	cubemapProgram = LoadShaders(shaders);//Ū��shader

	glUseProgram(cubemapProgram);//uniform�ѼƼƭȫe������use shader

	cubemapUm4mvLocation = glGetUniformLocation(cubemapProgram, "um4mv");
	cubemapUm4pLocation = glGetUniformLocation(cubemapProgram, "um4p");

#pragma region Create box
	float vertices[] = {
		-0.5, -0.5, -0.5,
		0.5, -0.5, -0.5,
		0.5, 0.5, -0.5,
		-0.5, 0.5, -0.5,
		-0.5, -0.5, 0.5,
		0.5, -0.5, 0.5,
		0.5, 0.5, 0.5,
		-0.5, 0.5, 0.5
	};

	int indices[] = {
		// posx
		1, 5, 6,
		6, 2, 1,
		// negx
		4, 0, 3,
		3, 7, 4,
		// posy
		3, 2, 6,
		6, 7, 3,
		// neg y
		4, 5, 1,
		1, 0, 4,
		// posz
		5, 4, 7,
		7, 6, 5,
		// negz
		0, 1, 2,
		2, 3, 0
	};

	glGenVertexArrays(1, &cubemap_vao);
	glBindVertexArray(cubemap_vao);

	glGenBuffers(1, &cubemap_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, cubemap_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &cubemap_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubemap_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
#pragma endregion

#pragma region Load texture
	std::vector<CubemapTexture> textures(6);
	textures[0].type = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
	textures[0].fileName = imagePath + "posx.jpg";
	textures[1].type = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
	textures[1].fileName = imagePath + "negx.jpg";
	textures[2].type = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
	textures[2].fileName = imagePath + "negy.jpg";
	textures[3].type = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
	textures[3].fileName = imagePath + "posy.jpg";
	textures[4].type = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
	textures[4].fileName = imagePath + "posz.jpg";
	textures[5].type = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
	textures[5].fileName = imagePath + "negz.jpg";

	glGenTextures(1, &cubemapTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID);

	for (int i = 0; i < textures.size(); ++i)
	{
		TextureData texData = Load_png(textures[i].fileName.c_str(), true);
		if (texData.data != nullptr)
		{
			glTexImage2D(textures[i].type, 0, GL_RGBA, texData.width, texData.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData.data);
			delete[] texData.data;
		}
		else
		{
			printf("Load texture file error %s\n", textures[i].fileName.c_str());
		}
	}

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	///glUseProgram(0);
#pragma endregion
}

void drawCubemapShader()
{
	//	enable
	glUseProgram(cubemapProgram);
	//	SetMVMat
	glUniformMatrix4fv(cubemapUm4mvLocation, 1, GL_FALSE, &(glm::mat4(glm::mat3(View))[0][0]));
	glUniformMatrix4fv(cubemapUm4pLocation, 1, GL_FALSE, &Projection[0][0]);
	/*glUniformMatrix4fv(cubemapUm4mvLocation, 1, GL_FALSE, glm::value_ptr(View));
	glUniformMatrix4fv(cubemapUm4pLocation, 1, GL_FALSE, glm::value_ptr(Projection));*/
	//	render
	glBindVertexArray(cubemap_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindVertexArray(0);
	//	disable
	glUseProgram(0);
}
TextureData Load_png(const char* path, bool mirroredY)
{
	TextureData texture;
	int n;
	stbi_uc *data = stbi_load(path, &texture.width, &texture.height, &n, 4);
	if (data != NULL)
	{
		texture.data = new unsigned char[texture.width * texture.height * 4 * sizeof(unsigned char)];
		memcpy(texture.data, data, texture.width * texture.height * 4 * sizeof(unsigned char));
		// vertical-mirror image data
		if (mirroredY)
		{
			for (size_t i = 0; i < texture.width; i++)
			{
				for (size_t j = 0; j < texture.height / 2; j++)
				{
					for (size_t k = 0; k < 4; k++) {
						std::swap(texture.data[(j * texture.width + i) * 4 + k], texture.data[((texture.height - j - 1) * texture.width + i) * 4 + k]);
					}
				}
			}
		}
		stbi_image_free(data);
	}
	else
	{
		cout << "stbi_load = null\n";
	}
	return texture;
}

#pragma endregion
void initWindowShader()
{
	//	create program
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "gray.vs.glsl" },//vertex shader
		{ GL_FRAGMENT_SHADER, "gray.fs.glsl" },//fragment shader
		{ GL_NONE, NULL } };
	windowProgram = LoadShaders(shaders);//Ū��shader

	glUseProgram(windowProgram);//uniform�ѼƼƭȫe������use shader

	//	attribute
	fxRadiusID = glGetUniformLocation(windowProgram, "fxRadius");
	colorErrorID = glGetUniformLocation(windowProgram, "colorError");
	zaTimeID = glGetUniformLocation(windowProgram, "time");

	glGenVertexArrays(1, &window_vao);
	glBindVertexArray(window_vao);

	glGenBuffers(1, &window_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, window_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(window_positions), window_positions, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, (const GLvoid*)(sizeof(GL_FLOAT) * 2));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glGenFramebuffers(1, &FBO);
}

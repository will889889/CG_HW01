#include "main.h"

vec3 camera = vec3(0,0,30);
int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitContextVersion(4,3);//以OpenGL version4.3版本為基準
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);//是否向下相容,GLUT_FORWARD_COMPATIBLE不支援(?
	glutInitContextProfile(GLUT_CORE_PROFILE);

	//multisample for golygons smooth
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH|GLUT_MULTISAMPLE);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("OpenGL 4.3 - Robot");

	glewExperimental = GL_TRUE; //置於glewInit()之前
	if (glewInit()) {
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;//c error
		exit(EXIT_FAILURE);
	}

	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(ChangeSize);
	glutKeyboardFunc(Keyboard);
	glutPassiveMotionFunc(My_Mouse_Moving);
	glutMotionFunc(My_Mouse_Moving);

	int ActionMenu,ModeMenu,ShaderMenu, InstanceMenu;
	InstanceMenu = glutCreateMenu(InstanceMenuEvents);//建立右鍵菜單
	//加入右鍵物件
	glutAddMenuEntry("1", 0);
	glutAddMenuEntry("100", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//與右鍵關聯

	ActionMenu = glutCreateMenu(ActionMenuEvents);//建立右鍵菜單
	//加入右鍵物件
	glutAddMenuEntry("idle",0);
	glutAddMenuEntry("walk",1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//與右鍵關聯

	ModeMenu = glutCreateMenu(ModeMenuEvents);//建立右鍵菜單
	//加入右鍵物件
	glutAddMenuEntry("Line",0);
	glutAddMenuEntry("Fill",1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//與右鍵關聯


	glutCreateMenu(menuEvents);//建立右鍵菜單
	//加入右鍵物件
	glutAddSubMenu("action",ActionMenu);
	glutAddSubMenu("mode", ModeMenu);
	glutAddSubMenu("instance amount",InstanceMenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//與右鍵關聯

	glutMouseFunc(Mouse);
	glutTimerFunc (33, idle, 0); 
	glutMainLoop();
	return 0;
}
void ChangeSize(int w,int h){
	if(h == 0) h = 1;
	glViewport(0,0,w,h);
	Projection = perspective(80.0f,(float)w/h,0.1f,1000.0f);
}
void Mouse(int button,int state,int x,int y){
	if(button == 2) isFrame = false;
}
void My_Mouse_Moving(int x, int y) {
	cout << "x: " << x << ", y: " << y << "\n";
}

//	update	(called per 33ms)
void idle(int dummy){
	float deltaTime = 0.033f;
	deltaTime *= timeSpeed;
	fakeTime += deltaTime;

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
	armRotateAngle = 0;

	//	reset deform rotation
	//...
}

#define DOR(angle) (angle*3.1415/180);

float armTime = 0;
float armTimeScale = 10.0f;
float bodyTime = 0;
float bodyTimeScale = 5.0f;
void updateObj(float deltaTime){
	//	Arm rotation
	//armTime += deltaTime;
	armRotateAngle = (90.0f * sin(armTime * armTimeScale)) + 90.0f;	//	between 0 ~ 180

	//	Body deform matrix
	bodyTime += deltaTime;
	bodyRotateMatrix = rotate(1.0f, cos(bodyTime * bodyTimeScale), 0, sin(bodyTime * bodyTimeScale));
}


 GLuint M_KaID;
 GLuint M_KdID;
 GLuint M_KsID;

void init(){
	cout << "sin(1) = " << sin(1.0f) << "\n";
	cout << "sin(3.14) = " << sin(3.14f) << "\n";


	isFrame = false;
	pNo = 0;
	for(int i = 0;i<PARTSNUM;i++)//初始化角度陣列
		angles[i] = 0.0;

	//VAO
	glGenVertexArrays(1,&VAO);
	glBindVertexArray(VAO);

	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "DSPhong_Material.vp" },//vertex shader
		{ GL_FRAGMENT_SHADER, "DSPhong_Material.fp" },//fragment shader
		{ GL_NONE, NULL }};
	program = LoadShaders(shaders);//讀取shader

	glUseProgram(program);//uniform參數數值前必須先use shader
	
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

	glClearColor(0.0,0.0,0.0,1);//black screen
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(VAO);
	glUseProgram(program);//uniform參數數值前必須先use shader
	float eyey = DOR(eyeAngley);
	View       = lookAt(
		               vec3(eyedistance*sin(eyey), 0,eyedistance*cos(eyey)) , // Camera is at (0,0,20), in World Space
		               vec3(0,0,0), // and looks at the origin
		               vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
		                );
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
		//(location,vec3,type,固定點,連續點的偏移量,buffer point)
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
		//(location,vec2,type,固定點,連續點的偏移量,point)
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
		//(location,vec3,type,固定點,連續點的偏移量,point)
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
	glFlush();//強制執行上次的OpenGL commands
	glutSwapBuffers();//調換前台和後台buffer ,當後臺buffer畫完和前台buffer交換使我們看見它
}

void Obj2Buffer(){
	std::vector<vec3> Kds;
	std::vector<vec3> Kas;
	std::vector<vec3> Kss;
	std::vector<std::string> Materials;//mtl-name
	std::string texture;
	//loadMTL("Obj/Arm_L2.mtl", Kds, Kas, Kss, Materials, texture);
	loadMTL("Obj/BodyCapFace.mtl", Kds, Kas, Kss, Materials, texture);
	///loadMTL("Obj/android.mtl", Kds, Kas, Kss, Materials, texture);
	///loadMTL("Obj/gundam.mtl",Kds,Kas,Kss,Materials,texture);
	//printf("%d\n",texture);
	for(int i = 0;i<Materials.size();i++){
		string mtlname = Materials[i];
		//  name            vec3
		KDs[mtlname] = Kds[i];
	}

	load2Buffer("Obj/BodyCapFace.obj", 0);
	load2Buffer("Obj/Arm_L2.obj", 1);
	load2Buffer("Obj/Arm_R2.obj", 2);

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
	//bind vbo ,第一次bind也同等於 create vbo 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);//VBO的target是GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER,totalSize[0],NULL,GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, uVBO);//VBO的target是GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER,totalSize[1],NULL,GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, nVBO);//VBO的target是GL_ARRAY_BUFFER
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
	beta = angle;
	Rotatation[0] = rotate(0,1,0,0);
	Translation[0] = translate(0, positionY,0);
	Models[0] = Translation[0]*Rotatation[0]* scale(1.0f, 1.0f, 1.0f);

	//	armRotateAngle [0 ~ 180]
	//	1 - Arm_L
	Rotatation[1] = rotate(-armRotateAngle, 1, 0, 0);
	Translation[1] = translate(2, 1.0f + positionY, 0);
	mat4 dRotation = DeformRotation;
	{
		int level = 25;
		for (int i = 1; i < level; i++)
		{
			Translation[1] = DeformRotation * Translation[1];
			Translation[1] *= translate(0, 1.0f, 0);
		}
		Translation[1] *= translate(0, 1.0f, 0);
		for (int i = 1; i < level; i++)
		{
			dRotation = DeformRotation * dRotation;
		}
	}
	mat4 fixedOffset = translate(18, 0, 0);
	mat4 negfixedOffset = translate(-18, 0, 0);
	Models[1] = Translation[1] * negfixedOffset * dRotation * fixedOffset * Rotatation[1] * scale(1.0f, 1.0f, 1.0f);

	//	2 - Arm_R
	Rotatation[2] = rotate(armRotateAngle - 180.0f, 1, 0, 0);
	Translation[2] = translate(-2, 1.0f + positionY, 0);
	dRotation = DeformRotation;
	{
		int level = 25;
		for (int i = 1; i < level; i++)
		{
			Translation[2] = DeformRotation * Translation[2];
			Translation[2] *= translate(0, 1.0f, 0);
		}
		Translation[2] *= translate(0, 1.0f, 0);
		for (int i = 1; i < level; i++)
		{
			dRotation = DeformRotation * dRotation;
		}
	}
	Models[2] = Translation[2] * fixedOffset * dRotation * negfixedOffset * Rotatation[2] * scale(1.0f, 1.0f, 1.0f);

	return;


	//Body=======================================================
	beta = angle;
	Rotatation[0] = rotate(beta, 0, 1, 0);
	Translation[0] = translate(0, 2.9 + 0, 0);
	Models[0] = Translation[0] * Rotatation[0];
	//左手=======================================================
	//左上手臂
	yaw = DOR(beta);r = 3.7;
	alpha = angles[1];
	gamma = 10;
	Rotatation[1] = rotate(alpha,1,0,0)*rotate(gamma,0,0,1);//向前旋轉*向右旋轉
	Translation[1] = translate(3.7,1,-0.5);

	Models[1] = Models[0]*Translation[1]*Rotatation[1];
	
	//左肩膀
	Rotatation[4] = rotate(alpha,1,0,0)*rotate(gamma,0,0,1);//向前旋轉*向右旋轉
	Translation[4] =translate(3.7,1,-0.5);//位移到左上手臂處
	Models[4] =Models[0]*Translation[1]*Rotatation[1];
	
	//左下手臂
	pitch = DOR(alpha);r = 3;
	roll = DOR(gamma);
	static int i=0;
	i+=5;
	alpha = angles[2]-20;
	//上手臂+下手臂向前旋轉*向右旋轉
	Rotatation[2] = rotate(alpha,1,0,0);
	//延x軸位移以上手臂為半徑的圓周長:translate(0,r*cos,r*sin)
	//延z軸位移以上手臂為半徑角度:translate(r*sin,-rcos,0)
	Translation[2] = translate(0,-3,0);

	Models[2] = Models[1]*Translation[2]*Rotatation[2];
	

	pitch = DOR(alpha);
	//b = DOR(angles[2]);
	roll = DOR(gamma);
	//手掌角度與下手臂相同
	//Rotatation[3] = Rotatation[2];
	//延x軸位移以上手臂為半徑的圓周長:translate(0,r*cos,r*sin) ,角度為上手臂+下手臂
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

	//(buffer type,data起始位置,data size,data first ptr)
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
	vec4 t = vec4(x,y,z,1);//w = 1 ,則x,y,z=0時也能translate
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
		armRotateAngle += 10.0f;
		cout << "armRotateAngle = " + std::to_string(armRotateAngle);
		break;
	case 't':
		armRotateAngle -= 10.0f;
		cout << "armRotateAngle = " + std::to_string(armRotateAngle);
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
void InstanceMenuEvents(int option) {
	switch (option) {
	case 0:
		instanceAmount = 1;
		break;
	case 1:
		instanceAmount = 100;
		break;
	}
}
void ActionMenuEvents(int option){
	switch(option){
	case 0:
		action = IDLE;
		break;
	case 1:
		action = WALK;
		break;
	}
}
void ModeMenuEvents(int option){
	switch(option){
	case 0:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 1:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	}
}
void ShaderMenuEvents(int option){
	pNo = option;
}

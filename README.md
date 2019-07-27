# ComputerGraphicsWeek9

## 9주차. Shading

### 1.주어진 아이언맨 모델(obj 파일)을 읽고 shding 을 올바르게 출력하시오.(50점)

|||
|:------------------------|:--------|
|Obj 파일 안의 vertex를 이용해 face normal 구하기 |+20|
|Face normal을 이용해서 vertex normal 구하기 |+20 |
|쿼터니언을 이용해서 모델을 회전 시키기기|+10 |


### 2.아이언맨과 자신이 맘에 드는 .obj 파일을 각각 다른 material 을 이용해서 한 윈도우에 모두 출력하시오.(50점)

|||
|:------------------------|:--------|
|아이언맨을 자신이 원하는 material 로 설정 |+20|
|자신이 맘에 드는 obj 파일을 다른 material로 설정 |+20 |
|위의 모델들을 한 윈도우에 모두 출력|+10 |

Hint. http://www.it.hiof.no/~borres/j3d/explain/light/p-materials.html

## 주요 소스 코드
### 1. face normal과 vertex normal 구하기
~~~
// 1. calculate face normals

		// prepare for a temporary memory
		vector<vec3> face_normals; //face를 담을 vector를 만든다.
		face_normals.resize(ix_stack_.size());
		// use cross product to calculate face normals
		for (int i = 0; i < ix_stack_.size(); i++) 
		{
			const unsigned int v0 = ix_stack_[i][0];
			const unsigned int v1 = ix_stack_[i][1];
			const unsigned int v2 = ix_stack_[i][2];

			const vec3 l0 = pos_stack_[v1] - pos_stack_[v0];
			const vec3 l1 = pos_stack_[v2] - pos_stack_[v1];

			const vec3 normal = glm::cross(l0, l1);

			face_normals[i] = normal;
		}

		// 2. find vertex normals

		// prepare for memory to store normal vectors
		nor_stack_.clear();
		nor_stack_.resize(pos_stack_.size());

		// make normal vectors zeros
		for (int i = 0; i < nor_stack_.size(); i++) {
			nor_stack_[i] = vec3(0.0f, 0.0f, 0.0f);
		}

		// accumulate face normals
		for (int i = 0; i < ix_stack_.size(); i++) {
			glm::vec3 &n0 = nor_stack_[ix_stack_[i][0]];
			glm::vec3 &n1 = nor_stack_[ix_stack_[i][1]];
			glm::vec3 &n2 = nor_stack_[ix_stack_[i][2]];

			n0 += face_normals[i];
			n1 += face_normals[i];
			n2 += face_normals[i];
		}

		// normalize vertex normals for smooth average face normals
		for (int i = 0; i < nor_stack_.size(); i++) {
			nor_stack_[i] = glm::normalize(nor_stack_[i]);

		}
// update object
		glm::quat rot = glm::angleAxis(3.14f / 4.0f * 0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 m_rot = glm::toMat4(rot);

		//lightPos = m_rot * lightPos;
		std::vector<glm::vec3>& vertices1 = obj_reader1.pos_stack_;
		std::vector<glm::vec3>& normals1 = obj_reader1.nor_stack_;

glBindBuffer(GL_ARRAY_BUFFER, vbo1[0]); //위치
		glBufferData(GL_ARRAY_BUFFER, obj_reader1.pos_stack_.size() * sizeof(float) * 3,
		   &obj_reader1.pos_stack_[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo1[1]); //색
		glBufferData(GL_ARRAY_BUFFER, obj_reader1.nor_stack_.size() * sizeof(float) * 3,
		   &obj_reader1.nor_stack_[0], GL_STATIC_DRAW);
       
~~~

### 결과 화면
![image](https://user-images.githubusercontent.com/22046757/61995570-37a33700-b0c5-11e9-9674-06193c372bf5.png)
 

## 주요 소스 코드
~~~
	OBJReader obj_reader1;
	OBJReader obj_reader2;
	obj_reader1.readObj("IronMan_Full_smooth.obj");
	obj_reader2.readObj("earth.obj");

	// copy geometry to buffer object
	GLuint vbo1[3];
	glGenBuffers(3, vbo1);

	GLuint vbo2[3];
	glGenBuffers(3, vbo2);

	glBindBuffer(GL_ARRAY_BUFFER, vbo1[0]); //위치
	glBufferData(GL_ARRAY_BUFFER, obj_reader1.pos_stack_.size() * sizeof(float) * 3,
		&obj_reader1.pos_stack_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo1[1]); //색
	glBufferData(GL_ARRAY_BUFFER, obj_reader1.nor_stack_.size() * sizeof(float) * 3,
		&obj_reader1.nor_stack_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo1[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj_reader1.ix_stack_.size() * sizeof(unsigned int) * 3,
		&obj_reader1.ix_stack_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]); //위치
	glBufferData(GL_ARRAY_BUFFER, obj_reader2.pos_stack_.size() * sizeof(float) * 3,
		&obj_reader2.pos_stack_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]); //색
	glBufferData(GL_ARRAY_BUFFER, obj_reader2.nor_stack_.size() * sizeof(float) * 3,
		&obj_reader2.nor_stack_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo2[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj_reader2.ix_stack_.size() * sizeof(unsigned int) * 3,
		&obj_reader2.ix_stack_[0], GL_STATIC_DRAW);

	// depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0); //GL_LIGHT0 ~ GL_LIGHT7
	//glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_COLOR_MATERIAL);

	GLfloat ambientLight[] = { 0.1, 0.1, 0.1, 1.0f }; //물체가 가지고 있는 빛의 색깔
	GLfloat lightColor[] = { 100.0f, 100.0f, 100.0f, 1.0f }; //빛이 스스로 가지고 있는 색깔(굉장히 밝은 색으로 지정했음)
	GLfloat lightPos[] = { 0.0, 2.0, 0.0, 1}; //빛의 위치 지정

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight); //GPU에 값을 넣어준다.
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor); //빛의 색깔 넣어준다
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);
	glLightfv(GL_LIGHT0, GL_POSITION, &lightPos[0]); //light 포지션을 넣어준다.

while (!glfwWindowShouldClose(window))
	{
		// update object
		glm::quat rot = glm::angleAxis(3.14f / 4.0f * 0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 m_rot = glm::toMat4(rot);

		//lightPos = m_rot * lightPos;
		std::vector<glm::vec3>& vertices1 = obj_reader1.pos_stack_;
		std::vector<glm::vec3>& normals1 = obj_reader1.nor_stack_;

		std::vector<glm::vec3>& vertices2 = obj_reader2.pos_stack_;
		std::vector<glm::vec3>& normals2 = obj_reader2.nor_stack_;

		for (int i = 0; i < vertices1.size(); i++) {
		   vertices1[i] = m_rot * glm::vec4(vertices1[i], 1);
		   normals1[i] = m_rot * glm::vec4(normals1[i], 0); //벡터이므로 0으로
		}

		for (int i = 0; i < vertices2.size(); i++) {
			vertices2[i] = m_rot * glm::vec4(vertices2[i], 1);
			normals2[i] = m_rot * glm::vec4(normals2[i], 0); //벡터이므로 0으로
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo1[0]); //위치
		glBufferData(GL_ARRAY_BUFFER, obj_reader1.pos_stack_.size() * sizeof(float) * 3,
		   &obj_reader1.pos_stack_[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo1[1]); //색
		glBufferData(GL_ARRAY_BUFFER, obj_reader1.nor_stack_.size() * sizeof(float) * 3,
		   &obj_reader1.nor_stack_[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]); //위치
		glBufferData(GL_ARRAY_BUFFER, obj_reader2.pos_stack_.size() * sizeof(float) * 3,
			&obj_reader2.pos_stack_[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]); //색
		glBufferData(GL_ARRAY_BUFFER, obj_reader2.nor_stack_.size() * sizeof(float) * 3,
			&obj_reader2.nor_stack_[0], GL_STATIC_DRAW);

		glLightfv(GL_LIGHT0, GL_POSITION, &lightPos[0]);

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // <- depth test

	   // non-shader version
		glPushMatrix();
		glTranslatef(-1, 0, 0);
		GLfloat mat_ambient[] = { 0.24725f, 0.1995f, 0.0745f, 1.0f };
		GLfloat mat_diffuse[] = { 0.75164f, 0.60648f, 0.22648f, 1.0f };
		GLfloat mat_specular[] = { 0.628281f, 0.555802f, 0.366065f, 1.0f };
		GLfloat shininess = 51.2f;

		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse); //노멀이 나가는 부분 FRONT 들어가는 부분 BACK 뒤에것도 같이 계산하고자 하면 GL_FRONT_AND_BACK을 사용 뒤에것만 사용한다면 GL_BACK
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);

		glBindBuffer(GL_ARRAY_BUFFER, vbo1[0]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo1[1]);
		glEnableClientState(GL_NORMAL_ARRAY);
		//glColorPointer(3, GL_FLOAT, 0, 0);
		glNormalPointer(GL_FLOAT, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo1[2]);
		glPolygonMode(GL_FRONT, GL_FILL);
		glDrawElements(GL_TRIANGLES, obj_reader1.ix_stack_.size() * 3, GL_UNSIGNED_INT, 0);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(1, 0, 0);
		GLfloat mat_ambient2[] = { 0.0215f, 0.1745f, 0.0215f, 0.55f };
		GLfloat mat_diffuse2[] = { 0.07568f, 0.61424f, 0.07568f, 0.55f };
		GLfloat mat_specular2[] = { 0.633f, 0.727811f, 0.633f, 0.55f };
		GLfloat shininess2 = 76.8f;

		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse2); //노멀이 나가는 부분 FRONT 들어가는 부분 BACK 뒤에것도 같이 계산하고자 하면 GL_FRONT_AND_BACK을 사용 뒤에것만 사용한다면 GL_BACK
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular2);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient2);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess2);
		glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
		glEnableClientState(GL_NORMAL_ARRAY);
		//glColorPointer(3, GL_FLOAT, 0, 0);
		glNormalPointer(GL_FLOAT, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo2[2]);
		glPolygonMode(GL_FRONT, GL_FILL);
		glDrawElements(GL_TRIANGLES, obj_reader2.ix_stack_.size() * 3, GL_UNSIGNED_INT, 0);
		glPopMatrix();
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		// xyz axes
		//glDisable(GL_LIGHTING);
		glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(1.0, 0.0, 0.0);
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 1.0, 0.0);
		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 1.0);
		glEnd();
~~~

### 결과화면
![image](https://user-images.githubusercontent.com/22046757/61995573-3a9e2780-b0c5-11e9-947a-96ed8813d28c.png) 

/*******************************************************************************
Assignment 2
Filename:   as2_MOK.cpp
Author:     Geoffrey Mok (100515125)
Date:       Nov 15, 2013
Purpose:    
  AS2
			Updated assignment 1 with additional models, textures, fullscreen viewport
			toggle & sphere/cube mapping capabilities
			*Reflection maps created in maya
			*Removed assignment1 specific objloader (not usable with standard OBJs; i.e maya)

  AS1
			Mimics 3ds Max's viewports and mouse navigations
		    GDW OBJLoader, standard obj: (vertices, textures, normals, face indices)

			*Vec3 is a tutorial provided file
			*Model.h/Model.cpp is the OBJloader I created for GDW
Controls: 
  Keyboard
   w,a,s,d  navigate view
   -,+      zoom in/out
   esc      exit program
   m        toggle shading
   r        toggle ortho rotation
   f		toggle fullscreen viewport
   l		toggle lights
   k		toggle reflection mapping - none, sphere, cube
 Mouse
  Left      rotate view
  Middle    navigate view
  right     zoom in/out
  wheel     zoom in/out
*******************************************************************************/
#include "Engine.h"

Model* reflector;
std::vector<Model*> world;

VIEW top;
VIEW front;
VIEW side;
VIEW persp;

GLboolean model = TRUE;
bool orthoRot = true;
bool fullScreen = false;
bool lighting = true;
int reflectionMapping = 2;

GLint windowWidth = WINDOW_WIDTH;
GLint windowHeight = WINDOW_HEIGHT;

int activeView = DEFAULT_VIEW;

bool leftClickFlag = false;
bool middleClickFlag = false;
bool rightClickFlag = false;

sf::Vector2i posOriginLMB;
sf::Vector2i posOriginMMB;
sf::Vector2i posOriginRMB;

//Cube map textures
char* filenames []= {TEX_CUBE_MAP_RIGHT, TEX_CUBE_MAP_LEFT,
					 TEX_CUBE_MAP_TOP, TEX_CUBE_MAP_BOTTEM,
					 TEX_CUBE_MAP_FRONT, TEX_CUBE_MAP_BACK};

int main()
{
	// Create the main window
	sf::Window App(sf::VideoMode(windowWidth, windowHeight, 32), "AS2_Mok");//, sf::Style::Fullscreen, 2); // 2 levels of antialiasing

	init(); //Init OpenGL states 

	sf::Event Event;

	///// MAIN LOOP /////
	while (App.isOpen())
	{
		// Clear color and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			updateWorldPos(0.f, SPEED_WASD_NAV);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			updateWorldPos(0.f, -SPEED_WASD_NAV);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			updateWorldPos(SPEED_WASD_NAV, 0.f);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			updateWorldPos(-SPEED_WASD_NAV, 0.f);

		//// TESTING
		//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
		//	activeView = TOP;
		//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
		//	activeView = FRONT;
		//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
		//	activeView = SIDE;
		//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
		//	activeView = PERSP;

		//if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal))
			updateZoom(-SPEED_KEYBOARD_ZOOM);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Dash))
			updateZoom(SPEED_KEYBOARD_ZOOM);

		// hold left mouse to rotate view
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			if (!leftClickFlag)
			{
				leftClickFlag = true;
				posOriginLMB = sf::Mouse::getPosition(App);

				if (!fullScreen && !middleClickFlag && !rightClickFlag)
					setActiveView(sf::Mouse::getPosition(App));
			}
			else
			{
				sf::Vector2i newPos = sf::Mouse::getPosition(App);

				newPos -= posOriginLMB;

				updateOrientation(newPos.y, newPos.x, 0);
			}
		}
		else
		{
			if (leftClickFlag)
			{
				leftClickFlag = false;

				applyOffsets(OFFSET_ROT);
			}
		}
		// hold middle mouse to move around view
		if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
		{
			if (!middleClickFlag)
			{
				middleClickFlag = true;
				posOriginMMB = sf::Mouse::getPosition(App);

				if (!fullScreen && !leftClickFlag && !rightClickFlag)
					setActiveView(sf::Mouse::getPosition(App));
			}
			else
			{
				sf::Vector2i newPos = sf::Mouse::getPosition(App);

				newPos -= posOriginMMB;

				//updateWorldPos(newPos.x*.00002f, newPos.y*.00002f);
				updateWorldOffsetPos(newPos.x*SPEED_MOUSE_NAV, newPos.y*SPEED_MOUSE_NAV);
			}
		}
		else
		{
			if (middleClickFlag)
			{
				middleClickFlag = false;

				applyOffsets(OFFSET_WORLD);
			}
		}
		//hold right mouse to zoom
		if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
		{
			if (!rightClickFlag)
			{
				rightClickFlag = true;
				posOriginRMB = sf::Mouse::getPosition(App);
				App.setMouseCursorVisible(false);

				if (!fullScreen && !leftClickFlag && !middleClickFlag)
					setActiveView(sf::Mouse::getPosition(App));
			}
			else
			{
				sf::Vector2i newPos = sf::Mouse::getPosition(App);

				newPos -= posOriginRMB;

				updateOffsetZoom(((newPos.x + newPos.y)/ 2)*SPEED_MOUSE_ZOOM );
			}
		}
		else
		{
			if (rightClickFlag)
			{
				rightClickFlag = false;
				App.setMouseCursorVisible(true);

				applyOffsets(OFFSET_ZOOM);
			}
		}


		while (App.pollEvent(Event))
		{
			// Close window : exit
			if (Event.type == sf::Event::Closed)
				App.close();

			// mousewheel to zoom
			if (Event.type == sf::Event::MouseWheelMoved)
			{
				updateZoom(-Event.mouseWheel.delta*SPEED_MOUSE_WHEEL_ZOOM);
			}

			// Escape key : exit
			if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::Escape))
				App.close();

			// Resize event : adjust viewport
			if (Event.type == sf::Event::Resized) {
				windowWidth = Event.size.width;
				windowHeight = Event.size.height;
			}

			// M key: switch shading model (smooth | flat)
			if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::M)) 
				if (model) {
					glShadeModel(GL_FLAT);
					model = 0;
				}
				else {
					glShadeModel(GL_SMOOTH);
					model = 1;
				}

			// R key: toggle ortho rotation
			if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::R)) 
				orthoRot = !orthoRot;

			// F key: toggle fullscreen
			if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::F)) 
				fullScreen = !fullScreen;

			// L key: toggle lighting
			if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::L)) 
				if (lighting) {
					glDisable(GL_LIGHTING);
					lighting = false;
				}
				else {
					glEnable(GL_LIGHTING);
					lighting = true;
				}

			// K key: toggle none, sphere, cube mapping
			if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::K)) 
			{
				reflectionMapping = (reflectionMapping+1) % 3;

				switch(reflectionMapping)
				{
				case(0):
					//reflector->texType = TEX_TYPE_REG;
					//reflector->objTex = texNone;
					reflector->loadTexture(TEX_CHECKERED);
					break;
				case(1):
					//reflector->texType = TEX_TYPE_SPHERE;
					//reflector->objTex = texSphere;
					reflector->loadTexture(TEX_SPHERE_MAP, TEX_TYPE_SPHERE);
					break;
				case(2):
					//reflector->texType = TEX_TYPE_CUBE;
					//reflector->objTex = texCube;
					reflector->loadCubeMap(filenames);
					break;
				}
			}
		}

		App.setActive();

		if (!fullScreen)
			setViewports();
		else
			setFullScreenViewport();

		// Finally, display rendered frame on screen
		App.display();
	}

	return EXIT_SUCCESS;
}

void init()
{
	///// INIT OpenGL /////
	// Set color and depth clear value
	glClearDepth(1.f);
	glClearColor(CLEAR_COLOR);

	// Enable Z-buffer read and write (for hidden surface removal)
	glEnable(GL_DEPTH_TEST);

	// Setup a perspective projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.f, 1.f, FOV_NEAR, FOV_FAR); // (fov, aspect, near z, far z)

	// Load OBJs
	Model* model;
	model = new Model(OBJ_WORLD_BALL);
	world.push_back(model);

	model = new Model(OBJ_WORLD_CONE);
	world.push_back(model);

	model = new Model(OBJ_WORLD_CUBE);

	world.push_back(model);

	model = new Model(OBJ_WORLD_CYLINDER);
	world.push_back(model);

	model = new Model(OBJ_WORLD_MONKEY);
	model->loadTexture(TEX_MONKEY);
	world.push_back(model);

	model = new Model(OBJ_WORLD_PIPE);
	model->loadTexture(TEX_UV_CHECKER);
	world.push_back(model);

	model = new Model(OBJ_WORLD_PYRAMID);
	model->loadTexture(TEX_UV_CHECKER);
	world.push_back(model);

	model = new Model(OBJ_WORLD_FLOOR);
	model->loadTexture(TEX_CHECKERED);
	world.push_back(model);

	model = new Model(OBJ_WORLD_SKYDOME);
	model->loadTexture(TEX_SKYDOME);
	world.push_back(model);

	model = new Model(OBJ_WORLD_SOCCER_BALL);
	world.push_back(model);

	model = new Model(OBJ_WORLD_SPIRAL);
	world.push_back(model);

	model = new Model(OBJ_WORLD_TORUS);
	model->loadTexture(TEX_UV_CHECKER);
	world.push_back(model);

	model = new Model(OBJ_WORLD_TRI_P);
	world.push_back(model);

	reflector = new Model(OBJ_WORLD_REFLECT_CUBE);
	reflector->loadCubeMap(filenames);
	world.push_back(reflector);

	// Prepare viewports
	top.camX = 0.f;
	top.camY = VIEW_CAM_DISTANCE;
	top.camZ = 0.0001f;
	top.zoomScale = 15.f;

	front.camX = 0.f;
	front.camY = 0.f;
	front.camZ = VIEW_CAM_DISTANCE;
	front.zoomScale = 15.f;

	side.camX = VIEW_CAM_DISTANCE;
	side.camY = 0.f;
	side.camZ = 0.f;
	side.zoomScale = 15.f;

	persp.camX = 1.f;
	persp.camY = 1.f;
	persp.camZ = VIEW_CAM_DISTANCE/2;
	persp.zoomScale = 1.f;

	GLfloat ambientColor[] = LIGHT_AMBIENT_COL;
	GLfloat diffuseColor[] = LIGHT_DEFUSE_COL;
	GLfloat specularColor[] = LIGHT_SPECULAR_COL;
	//GLfloat lightColor[] = {1.f, 1.f, 0.f, 1.f};
	GLfloat lightPos1[] = {0, 0, 0, 1};
	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos1);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);

	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	printf("[WASD] + [Mouse] Navigation\n");
	printf("[+/-] Zoom in/out\n");
	printf("[R] Orthographic View rotation ON/OFF\n");
	printf("[F] Fullscreen ON/OFF\n");
	printf("[L] lights ON/OFF\n");
	printf("[M] Shading SMOOTH/FLAT\n");
	printf("[K] Reflection Map NONE/SPHERE/CUBE\n");
	printf("*Keyboard controls only work when Main window is active\n");
}

void updateWorldPos(float x, float y)
{
	switch(activeView)
	{
	case TOP:
		top.worldX += x;
		top.worldY += 0;
		top.worldZ += y;
		break;
	case FRONT:
		front.worldX += x;
		front.worldY += -y;
		front.worldZ += 0;
		break;
	case SIDE:
		side.worldX += 0;
		side.worldY += -y;
		side.worldZ += -x;
		break;
	case PERSP:
		persp.worldX += x;
		persp.worldY += 0;
		persp.worldZ += y;
		break;
	}
}
void updateWorldOffsetPos(float x, float y)
{
	switch(activeView)
	{
	case TOP:
		top.worldOffsetX = x;
		top.worldOffsetY = 0;
		top.worldOffsetZ = y;
		break;
	case FRONT:
		front.worldOffsetX = x;
		front.worldOffsetY = -y;
		front.worldOffsetZ = 0;
		break;
	case SIDE:
		side.worldOffsetX = 0;
		side.worldOffsetY = -y;
		side.worldOffsetZ = -x;
		break;
	case PERSP:
		persp.worldOffsetX = x;
		persp.worldOffsetY = -y;
		persp.worldOffsetZ = 0;
		break;
	}
}

void updateZoom(float zoom)
{
	switch(activeView)
	{
	case TOP:
		top.zoomScale += zoom;
		if (top.zoomScale < 0)
			top.zoomScale = VIEW_MAX_ZOOM;
		break;
	case FRONT:
		front.zoomScale += zoom;
		if (front.zoomScale < 0)
			front.zoomScale = VIEW_MAX_ZOOM;
		break;
	case SIDE:
		side.zoomScale += zoom;
		if (side.zoomScale < 0)
			side.zoomScale = VIEW_MAX_ZOOM;
		break;
	case PERSP:
		persp.worldZ += -zoom*5;
		break;
	}
}
void updateOffsetZoom(float zoom)
{
	switch(activeView)
	{
	case TOP:
		top.zoomOffsetScale = zoom;
		if (top.zoomScale + top.zoomOffsetScale < 0)
			top.zoomOffsetScale = -top.zoomScale + VIEW_MAX_ZOOM;
		break;
	case FRONT:
		front.zoomOffsetScale = zoom;
		if (front.zoomScale + front.zoomOffsetScale < 0)
			front.zoomOffsetScale = -front.zoomScale + VIEW_MAX_ZOOM;
		break;
	case SIDE:
		side.zoomOffsetScale = zoom;
		if (side.zoomScale + side.zoomOffsetScale < 0)
			side.zoomOffsetScale = -side.zoomScale + VIEW_MAX_ZOOM;
		break;
	case PERSP:
		persp.worldOffsetZ = -zoom*5;
		break;
	}
}

void updateOrientation(float x, float y, float z)
{
	if (orthoRot || activeView == PERSP)
	{

		switch(activeView)
		{

		case TOP:
			top.rotOffsetY = (x+y)/2;
			break;
		case FRONT:
			front.rotOffsetZ = (x+y)/2;
			break;
		case SIDE:
			side.rotOffsetX = (x+y)/2;
			break;
		case PERSP:
			persp.rotOffsetX = x;
			persp.rotOffsetY = y;
			persp.rotOffsetZ = z;
			break;
		}
	}
}

void setActiveView(sf::Vector2i pos)
{
	if (leftClickFlag || middleClickFlag || rightClickFlag)
	{
		if (pos.x <= windowWidth/2)
		{
			if(pos.y <= windowHeight/2)
				activeView = TOP;
			else if(pos.y > windowHeight/2)
				activeView = SIDE;
		}
		else if(pos.x > windowWidth/2)
		{
			if(pos.y <= windowHeight/2)
				activeView = FRONT;
			else if(pos.y > windowHeight/2)
				activeView = PERSP;
		}
	}
}

void applyOffsets(int offsetType)
{
	switch(activeView)
	{
	case TOP:
		switch (offsetType)
		{
		case OFFSET_ROT:
			top.rotX += top.rotOffsetX;
			top.rotY += top.rotOffsetY;
			top.rotZ += top.rotOffsetZ;
			top.rotOffsetX = top.rotOffsetY = top.rotOffsetZ = 0;
			break;
		case OFFSET_WORLD:
			top.worldX += top.worldOffsetX;
			top.worldY += top.worldOffsetY;
			top.worldZ += top.worldOffsetZ;
			top.worldOffsetX = top.worldOffsetY = top.worldOffsetZ = 0;
			break;
		case OFFSET_ZOOM:
			top.zoomScale += top.zoomOffsetScale;
			top.zoomOffsetScale = 0;
			break;
		}
		break;
	case FRONT:
		switch (offsetType)
		{
		case OFFSET_ROT:
			front.rotX += front.rotOffsetX;
			front.rotY += front.rotOffsetY;
			front.rotZ += front.rotOffsetZ;
			front.rotOffsetX = front.rotOffsetY = front.rotOffsetZ = 0;
			break;
		case OFFSET_WORLD:
			front.worldX += front.worldOffsetX;
			front.worldY += front.worldOffsetY;
			front.worldZ += front.worldOffsetZ;
			front.worldOffsetX = front.worldOffsetY = front.worldOffsetZ = 0;
			break;
		case OFFSET_ZOOM:
			front.zoomScale += front.zoomOffsetScale;
			front.zoomOffsetScale = 0;
			break;
		}
		break;
	case SIDE:
		switch (offsetType)
		{
		case OFFSET_ROT:
			side.rotX += side.rotOffsetX;
			side.rotY += side.rotOffsetY;
			side.rotZ += side.rotOffsetZ;
			side.rotOffsetX = side.rotOffsetY = side.rotOffsetZ = 0;
			break;
		case OFFSET_WORLD:
			side.worldX += side.worldOffsetX;
			side.worldY += side.worldOffsetY;
			side.worldZ += side.worldOffsetZ;
			side.worldOffsetX = side.worldOffsetY = side.worldOffsetZ = 0;
			break;
		case OFFSET_ZOOM:
			side.zoomScale += side.zoomOffsetScale;
			side.zoomOffsetScale = 0;
			break;
		}
		break;
	case PERSP:
		switch (offsetType)
		{
		case OFFSET_ROT:
			persp.rotX += persp.rotOffsetX;
			persp.rotY += persp.rotOffsetY;
			persp.rotZ += persp.rotOffsetZ;
			persp.rotOffsetX = persp.rotOffsetY = persp.rotOffsetZ = 0;
			break;
		case OFFSET_WORLD:
		case OFFSET_ZOOM:
			persp.worldX += persp.worldOffsetX;
			persp.worldY += persp.worldOffsetY;
			persp.worldZ += persp.worldOffsetZ;
			persp.worldOffsetX = persp.worldOffsetY = persp.worldOffsetZ = 0;
			break;
		}
		break;
	}
}

void setViewports() 
{
	float aspect = windowWidth/windowHeight;

	//// Viewport 1 TL - TOP
	// Setup a orthographic projection
	glViewport(0, windowHeight/2, windowWidth/2, windowHeight/2);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(VIEW_ORTHO_SIZE*-aspect*(top.zoomScale+top.zoomOffsetScale),
			VIEW_ORTHO_SIZE*aspect*(top.zoomScale+top.zoomOffsetScale),
			-VIEW_ORTHO_SIZE*(top.zoomScale+top.zoomOffsetScale), 
			VIEW_ORTHO_SIZE*(top.zoomScale+top.zoomOffsetScale),
			FOV_NEAR, FOV_FAR);
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	drawWorld(top);

	///// Viewport 2 TR - FRONT
	// Setup a orthographic projection
	glViewport(windowWidth/2, windowHeight/2, windowWidth/2, windowHeight/2);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-VIEW_ORTHO_SIZE*aspect*(front.zoomScale+front.zoomOffsetScale),
			VIEW_ORTHO_SIZE*aspect*(front.zoomScale+front.zoomOffsetScale),
			-VIEW_ORTHO_SIZE*(front.zoomScale+front.zoomOffsetScale),
			VIEW_ORTHO_SIZE*(front.zoomScale+front.zoomOffsetScale),
			FOV_NEAR, FOV_FAR);
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	drawWorld(front);

	///// Viewport 3 BL - SIDE
	// Setup a orthographic projection 
	glViewport(0, 0, windowWidth/2, windowHeight/2);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-VIEW_ORTHO_SIZE*aspect*(side.zoomScale+side.zoomOffsetScale),
			VIEW_ORTHO_SIZE*aspect*(side.zoomScale+side.zoomOffsetScale),
			-VIEW_ORTHO_SIZE*(side.zoomScale+side.zoomOffsetScale),
			VIEW_ORTHO_SIZE*(side.zoomScale+side.zoomOffsetScale),
			FOV_NEAR, FOV_FAR);
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	drawWorld(side);

	///// Viewport 4 BR - PERSPECTIVE
	// Setup a perspective projection
	glViewport(windowWidth/2, 0, windowWidth/2, windowHeight/2);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.f*persp.zoomScale, aspect, FOV_NEAR, FOV_FAR);
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	drawWorld(persp);

	/////// Viewport Borders
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	glViewport(0, 0, windowWidth, windowHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, windowWidth, 0, windowHeight, -1, 1);

	glPushMatrix();

	//offset for border thickness
	int width = windowWidth-VIEW_BORDER_SIZE;
	int height = windowHeight-VIEW_BORDER_SIZE;

	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);

	glEnable( GL_LINE_SMOOTH );
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);

	glLineWidth(VIEW_LINE_WIDTH);
	glBegin(GL_LINE_LOOP);
		glVertex2d(VIEW_BORDER_SIZE, VIEW_BORDER_SIZE);
		glVertex2d(width, VIEW_BORDER_SIZE);
		glVertex2d(width, height);
		glVertex2d(VIEW_BORDER_SIZE, height);
	glEnd();
	glBegin(GL_LINES);
		glVertex2d(VIEW_BORDER_SIZE, height/2);
		glVertex2d(width, height/2);
	glEnd();
	glBegin(GL_LINES);
		glVertex2d(width/2, VIEW_BORDER_SIZE);
		glVertex2d(width/2, height);
	glEnd();

	//Draw border around active viewport
	//glLineWidth(5);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	switch(activeView)
	{
	case TOP:
		glBegin(GL_LINE_LOOP);
		glVertex2d(VIEW_BORDER_SIZE, height/2);
		glVertex2d(width/2, height/2);
		glVertex2d(width/2, height);
		glVertex2d(VIEW_BORDER_SIZE, height);
		glEnd();
		break;
	case FRONT:
		glBegin(GL_LINE_LOOP);
		glVertex2d(width/2, height/2);
		glVertex2d(width, height/2);
		glVertex2d(width, height);
		glVertex2d(width/2, height);
		glEnd();
		break;
	case SIDE:
		glBegin(GL_LINE_LOOP);
		glVertex2d(VIEW_BORDER_SIZE, VIEW_BORDER_SIZE);
		glVertex2d(width/2, VIEW_BORDER_SIZE);
		glVertex2d(width/2, height/2);
		glVertex2d(VIEW_BORDER_SIZE, height/2);

		glEnd();
		break;
	case PERSP:
		glBegin(GL_LINE_LOOP);
		glVertex2d(width/2, VIEW_BORDER_SIZE);
		glVertex2d(width, VIEW_BORDER_SIZE);
		glVertex2d(width, height/2);
		glVertex2d(width/2, height/2);
		glEnd();
		break;
	}

	glPopMatrix();
	glDisable( GL_LINE_SMOOTH );
	glLineWidth(VIEW_EDGE_WIDTH);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	if (lighting) glEnable(GL_LIGHTING);

}

void drawWorld(VIEW view) {
	 glColor4f(1.f, 1.f, 1.f, 1.f);
	 glMatrixMode(GL_MODELVIEW);

	 glPushMatrix();

	 gluLookAt(view.camX, view.camY, view.camZ,
		       0,0,0,
		       0,1,0);

	 glTranslatef(view.worldX+view.worldOffsetX, view.worldY+view.worldOffsetY, view.worldZ+view.worldOffsetZ);
	 glRotatef(view.rotX + view.rotOffsetX, 1.f, 0.f, 0.f);
	 glRotatef(view.rotY + view.rotOffsetY, 0.f, 1.f, 0.f);
	 glRotatef(view.rotZ + view.rotOffsetZ, 0.f, 0.f, 1.f);

	 GLfloat lightPos1[] = {view.worldX+view.worldOffsetX-500.f,
		 view.worldY+view.worldOffsetY+1000.f,
		 view.worldZ+view.worldOffsetZ+1000.f,
		 1};

	 glLightfv(GL_LIGHT0, GL_POSITION, lightPos1);

	 for (int i = 0; i < world.size(); i++)
	 {
		 world[i]->draw();
	 }

	 glPopMatrix();
 }

void setFullScreenViewport()
{
	float aspect = windowWidth/windowHeight;

	glViewport(0, 0, windowWidth, windowHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	switch(activeView)
	{
	case TOP:
		glOrtho(VIEW_ORTHO_SIZE*-aspect*(top.zoomScale+top.zoomOffsetScale),
			VIEW_ORTHO_SIZE*aspect*(top.zoomScale+top.zoomOffsetScale),
			-VIEW_ORTHO_SIZE*(top.zoomScale+top.zoomOffsetScale), 
			VIEW_ORTHO_SIZE*(top.zoomScale+top.zoomOffsetScale),
			FOV_NEAR, FOV_FAR);
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		drawWorld(top);
		break;
	case FRONT:
		glOrtho(-VIEW_ORTHO_SIZE*aspect*(front.zoomScale+front.zoomOffsetScale),
			VIEW_ORTHO_SIZE*aspect*(front.zoomScale+front.zoomOffsetScale),
			-VIEW_ORTHO_SIZE*(front.zoomScale+front.zoomOffsetScale),
			VIEW_ORTHO_SIZE*(front.zoomScale+front.zoomOffsetScale),
			FOV_NEAR, FOV_FAR);
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		drawWorld(front);
		break;
	case SIDE:
		glOrtho(-VIEW_ORTHO_SIZE*aspect*(side.zoomScale+side.zoomOffsetScale),
			VIEW_ORTHO_SIZE*aspect*(side.zoomScale+side.zoomOffsetScale),
			-VIEW_ORTHO_SIZE*(side.zoomScale+side.zoomOffsetScale),
			VIEW_ORTHO_SIZE*(side.zoomScale+side.zoomOffsetScale),
			FOV_NEAR, FOV_FAR);
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		drawWorld(side);
		break;
	case PERSP:
		gluPerspective(90.f*persp.zoomScale, aspect, FOV_NEAR, FOV_FAR);
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		drawWorld(persp);
		break;
	}
}
#ifndef _AS2_MOK_H
#define _AS2_MOK_H

//VIEW_SCALE is in accordance to the size of loaded obj, controls navigation speeds and camera distance
#define VIEW_SCALE 10.f

// Window
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 600

// Camera
#define CLEAR_COLOR 0.5f, 0.5f, 0.5f, 1.f
#define FOV_NEAR 2.f
#define FOV_FAR 2000.f

// Viewport
#define VIEW_CAM_DISTANCE 10.f*VIEW_SCALE
#define VIEW_MAX_ZOOM .01f
#define VIEW_ORTHO_SIZE 4
#define VIEW_BORDER_SIZE 2
#define VIEW_LINE_WIDTH 6
#define VIEW_EDGE_WIDTH 1

// Navigation
#define SPEED_WASD_NAV .1f*VIEW_SCALE
#define SPEED_KEYBOARD_ZOOM .01f*VIEW_SCALE
#define SPEED_MOUSE_NAV .05f*VIEW_SCALE
#define SPEED_MOUSE_ZOOM .01f*VIEW_SCALE
#define SPEED_MOUSE_WHEEL_ZOOM .1f*VIEW_SCALE

// Lighting
#define LIGHT_AMBIENT_COL {.2f, .2f, .2f, 1.f}
//#define LIGHT_DEFUSE_COL {0.992f, 0.722f, 0.075f, 1.f}
#define LIGHT_DEFUSE_COL {1.f, 1.f, 1.f, 1.f}
#define LIGHT_SPECULAR_COL {0.992f, 0.722f, 0.075f, 1.f};

// 
#define TOP		0
#define FRONT	1
#define SIDE	2
#define PERSP	3
#define DEFAULT_VIEW PERSP

#define OFFSET_ROT   0
#define OFFSET_WORLD 1
#define OFFSET_ZOOM  2

#define TEX_MONKEY				"resources/monkey.jpg"
#define TEX_UV_CHECKER			"resources/uvchecker.jpg"
#define TEX_CHECKERED			"resources/CbxKs.jpg"
#define TEX_SKYDOME				"resources/sky.jpg"
#define TEX_SPHERE_MAP			"resources/sphere_map.png"
#define TEX_CUBE_MAP_LEFT		"resources/left.png"
#define TEX_CUBE_MAP_RIGHT		"resources/right.png"
#define TEX_CUBE_MAP_TOP		"resources/top.png"
#define TEX_CUBE_MAP_BOTTEM 	"resources/bottem.png"
#define TEX_CUBE_MAP_BACK		"resources/back.png"
#define TEX_CUBE_MAP_FRONT		"resources/front.png"
			
#define	OBJ_WORLD_BALL			"resources/ball.obj"
#define	OBJ_WORLD_CONE			"resources/cone.obj"
#define	OBJ_WORLD_CUBE			"resources/cube.obj"
#define	OBJ_WORLD_CYLINDER		"resources/cylinder.obj"
#define	OBJ_WORLD_FLOOR			"resources/floor.obj"
#define	OBJ_WORLD_MONKEY		"resources/monkey.obj"
#define	OBJ_WORLD_PIPE			"resources/pipe.obj"
#define	OBJ_WORLD_PYRAMID		"resources/pyramid.obj"
#define	OBJ_WORLD_REFLECT_BALL	"resources/reflectiveBall.obj"
#define	OBJ_WORLD_REFLECT_CUBE	"resources/reflectiveCube.obj"
#define	OBJ_WORLD_SKYDOME		"resources/skyDome.obj"
#define	OBJ_WORLD_SOCCER_BALL	"resources/soccerball.obj"
#define	OBJ_WORLD_SPIRAL		"resources/spiral.obj"
#define	OBJ_WORLD_TORUS			"resources/torus.obj"
#define	OBJ_WORLD_TRI_P			"resources/triangularPyr.obj"

struct VIEW {
	float camX, camY, camZ;
	float worldX, worldY, worldZ;
	float rotX, rotY, rotZ;
	float zoomScale;

	//temporary offset/delta values; used with mouse manipulation
	float rotOffsetX, rotOffsetY, rotOffsetZ;
	float worldOffsetX, worldOffsetY, worldOffsetZ;
	float zoomOffsetScale;
};

void init();
void setViewports();
void setFullScreenViewport();

void updateWorldPos(float x, float y);
void updateWorldOffsetPos(float x, float y);
void updateZoom(float zoom);
void updateOffsetZoom(float zoom);
void updateOrientation(float x, float y, float z);
void applyOffsets(int offsetType);
void setActiveView(sf::Vector2i);

void drawWorld(VIEW view);

#endif
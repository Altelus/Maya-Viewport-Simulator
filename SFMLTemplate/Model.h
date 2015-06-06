#ifndef _MODEL_H
#define _MODEL_H

#if OBJ_MODE == GDW_OBJ

#include "Vec3.h"

#define MODEL_TYPE_V 0
#define MODEL_TYPE_VT 1
#define MODEL_TYPE_VTN 2

#define TEX_TYPE_REG 0
#define TEX_TYPE_SPHERE 1
#define TEX_TYPE_CUBE 2

typedef struct tagVERTEX {
	float x, y, z;
} VERTEX;

typedef struct tagUV {
	float u, v;
} UV;

typedef struct tagNormal {
	float x, y, z;
} NORMAL;

typedef struct tagTRIANGLE {
	VERTEX vertex[3];
	UV uv[3];
	NORMAL normal[3];
} TRIANGLE;

class Model
{
public:
	Model();
	Model(const int);
	Model(const char*, int);
	Model(const char*);

	int GetModelType() const { return modelType; }
	void SetModelType(int);

	bool loadObj(const char*);
	bool loadTexture(const char*, const int = TEX_TYPE_REG);
	bool loadCubeMap(char**);

	void draw();

	Vec3 scale;
	Vec3 pos;
	Vec3 rot;

	GLuint objTex;
	int texType;

private:
	void readLine(FILE *, char *);

	std::vector <VERTEX> vertices;
	std::vector <UV> uvs;
	std::vector <NORMAL> normals;
	std::vector <TRIANGLE> triangles;

	int modelType;
	bool isLoaded;
};

#endif

#endif

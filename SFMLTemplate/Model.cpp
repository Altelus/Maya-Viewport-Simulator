#include "Engine.h"
#include <exception>

#if OBJ_MODE == GDW_OBJ
Model::Model()
{
	isLoaded = false;
	modelType = MODEL_TYPE_VTN;

	scale.x = 1;
	scale.y = 1;
	scale.z = 1;
}

Model::Model(const int type)
{
	isLoaded = false;
	SetModelType(type);

	scale.x = 1;
	scale.y = 1;
	scale.z = 1;
}

Model::Model(const char* filename)
{
	isLoaded = false;
	modelType = MODEL_TYPE_VTN;

	scale.x = 1;
	scale.y = 1;
	scale.z = 1;

	loadObj(filename);
}

Model::Model(const char* filename, const int type)
{
	isLoaded = false;

	scale.x = 1;
	scale.y = 1;
	scale.z = 1;

	SetModelType(type);
	loadObj(filename);
}

void Model::SetModelType(int type)
{
	if (0 <= type && type <= MODEL_TYPE_VTN)
		modelType = type;
}

bool Model::loadObj(const char* filename)
{
	bool result = false;

	std::vector <float> triVertA, triVertB, triVertC;
	std::vector <float> triUVA, triUVB, triUVC;
	std::vector <float> triNormA, triNormB, triNormC;

	float vertA, uvA, normA, vertB, uvB, normB, vertC, uvC, normC;

	char line[255];
	char header[3];

	FILE* file = fopen(filename, "r");
	if (file == nullptr)
		return false;
	try
	{
		while (!feof(file))
		{
			VERTEX tempVert;
			UV tempUV;
			NORMAL tempNorm;

			readLine(file, line);

			sscanf (line, "%s", &header);

			if (strcmp(header, "v") == 0)
			{
				sscanf(line, "%*s %f %f %f", &tempVert.x, &tempVert.y, &tempVert.z);
				vertices.push_back(tempVert);
			}
			else if (strcmp(header, "vt") == 0)
			{
				sscanf(line, "%*s %f %f", &tempUV.u, &tempUV.v);
				uvs.push_back(tempUV);
			}
			else if (strcmp(header, "vn") == 0)
			{
				sscanf(line, "%*s %f %f %f", &tempNorm.x, &tempNorm.y, &tempNorm.z);
				normals.push_back(tempNorm);
			}
			else if(strcmp(header, "f") == 0)
			{
				if (modelType == MODEL_TYPE_V)
				{
					sscanf(line, "%*s %f %f %f", &vertA, &vertB, &vertC);
				}
				else if (modelType == MODEL_TYPE_VT)
				{
					sscanf(line, "%*s %f/%f %f/%f %f/%f", &vertA, &uvA, &vertB, &uvB, &vertC, &uvC);
			
					triUVA.push_back(uvA);
					triUVB.push_back(uvB);
					triUVC.push_back(uvC);
				}
				else if (modelType == MODEL_TYPE_VTN)
				{
					sscanf(line, "%*s %f/%f/%f %f/%f/%f %f/%f/%f", &vertA, &uvA, &normA, &vertB, &uvB, &normB, &vertC, &uvC, &normC);

					triUVA.push_back(uvA);
					triUVB.push_back(uvB);
					triUVC.push_back(uvC);

					triNormA.push_back(normA);
					triNormB.push_back(normB);
					triNormC.push_back(normC);
				}

				triVertA.push_back(vertA);
				triVertB.push_back(vertB);
				triVertC.push_back(vertC);
			}
		}
		fclose(file);

		//assign vertices to faces(triangle)
		for (int i = 0; i < triVertA.size(); i++)
		{
			TRIANGLE tempTri;

			// Vertices
			tempTri.vertex[0].x = vertices.at(triVertA[i] - 1).x;
			tempTri.vertex[0].y = vertices.at(triVertA[i] - 1).y;
			tempTri.vertex[0].z = vertices.at(triVertA[i] - 1).z;

			tempTri.vertex[1].x = vertices.at(triVertB[i] - 1).x;
			tempTri.vertex[1].y = vertices.at(triVertB[i] - 1).y;
			tempTri.vertex[1].z = vertices.at(triVertB[i] - 1).z;

			tempTri.vertex[2].x = vertices.at(triVertC[i] - 1).x;
			tempTri.vertex[2].y = vertices.at(triVertC[i] - 1).y;
			tempTri.vertex[2].z = vertices.at(triVertC[i] - 1).z;

			if (modelType == MODEL_TYPE_VT || modelType == MODEL_TYPE_VTN)
			{
				// Textures
				tempTri.uv[0].u = uvs.at(triUVA[i] - 1).u;
				tempTri.uv[0].v = uvs.at(triUVA[i] - 1).v;

				tempTri.uv[1].u = uvs.at(triUVB[i] - 1).u;
				tempTri.uv[1].v = uvs.at(triUVB[i] - 1).v;

				tempTri.uv[2].u = uvs.at(triUVC[i] - 1).u;
				tempTri.uv[2].v = uvs.at(triUVC[i] - 1).v;

				// Normals
				if (modelType == MODEL_TYPE_VTN)
				{
					tempTri.normal[0].x = normals.at(triNormA[i] - 1).x;
					tempTri.normal[0].y = normals.at(triNormA[i] - 1).y;
					tempTri.normal[0].z = normals.at(triNormA[i] - 1).z;

					tempTri.normal[1].x = normals.at(triNormB[i] - 1).x;
					tempTri.normal[1].y = normals.at(triNormB[i] - 1).y;
					tempTri.normal[1].z = normals.at(triNormB[i] - 1).z;

					tempTri.normal[2].x = normals.at(triNormC[i] - 1).x;
					tempTri.normal[2].y = normals.at(triNormC[i] - 1).y;
					tempTri.normal[2].z = normals.at(triNormC[i] - 1).z;
				}
			}

			triangles.push_back(tempTri);
		}

		isLoaded = result = true;
	}
	catch(...)
	{
		vertices.clear();
		uvs.clear();
		normals.clear();
	}

	return result;
}

void Model::draw()
{
	if (isLoaded)
	{
		glPushMatrix();

		glTranslatef(pos.x, pos.y, pos.z);
		glScalef(scale.x, scale.y, scale.z);

		glRotatef(rot.x, 1, 0, 0);
		glRotatef(rot.y, 0, 1, 0);
		glRotatef(rot.z, 0, 0, 1);

		if (texType == TEX_TYPE_REG)
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, objTex);
		}
		else if (texType == TEX_TYPE_SPHERE)
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, objTex);
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
		}
		else if (texType == TEX_TYPE_CUBE)
		{
			glEnable(GL_TEXTURE_CUBE_MAP);
			glBindTexture(GL_TEXTURE_CUBE_MAP, objTex);
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glEnable(GL_TEXTURE_GEN_R);
		}

		glBegin(GL_TRIANGLES);
		for (int i = 0; i < triangles.size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (texType == TEX_TYPE_REG)
					glTexCoord2f(triangles[i].uv[j].u, -triangles[i].uv[j].v);

				glNormal3f(triangles[i].normal[j].x,
						   triangles[i].normal[j].y,
						   triangles[i].normal[j].z);

				glVertex3f(triangles[i].vertex[j].x,
					triangles[i].vertex[j].y,
					triangles[i].vertex[j].z);
			}
		}
		glEnd();

		if (texType == TEX_TYPE_REG)
		{
			glDisable(GL_TEXTURE_2D);
		}
		else if (texType == TEX_TYPE_SPHERE)
		{

			glDisable(GL_TEXTURE_2D);
			glDisable(GL_TEXTURE_GEN_T);
			glDisable(GL_TEXTURE_GEN_S);
		}
		else if (texType == TEX_TYPE_CUBE)
		{
			glDisable(GL_TEXTURE_CUBE_MAP);
			glDisable(GL_TEXTURE_GEN_R);
			glDisable(GL_TEXTURE_GEN_T);
			glDisable(GL_TEXTURE_GEN_S);
		}

		glPopMatrix();
	}
}

void Model::readLine(FILE * fp, char * string)
{
	do
	{
		fgets(string, 255, fp);

	} while (string[0] == '\n');
	return;
}

bool Model::loadTexture(const char* filename, const int type)
{
	bool result = false;
	texType = type;

	sf::Image texImg;
	if(texImg.loadFromFile(filename))
	{
		glGenTextures(1, &objTex);
		glBindTexture(GL_TEXTURE_2D, objTex);
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, texImg.getSize().x, texImg.getSize().y, GL_RGBA, GL_UNSIGNED_BYTE, texImg.getPixelsPtr());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		if (texType == TEX_TYPE_SPHERE)
		{
			glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGenf(GL_R, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		}
		result = true;
	}

	return true;
}

bool Model::loadCubeMap( char** filenames )
{
	bool result = false;
	texType = TEX_TYPE_CUBE;

	glEnable(GL_TEXTURE_CUBE_MAP);

	glGenTextures(1, &objTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, objTex);

	sf::Image texImg;
	for (int i = 0; i < 6; i++, filenames++)
	{
		
		if(texImg.loadFromFile(*filenames))
		{
			gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, GL_RGBA, texImg.getSize().x, texImg.getSize().y, GL_RGBA, GL_UNSIGNED_BYTE, texImg.getPixelsPtr());
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
			glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
			glTexGenf(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}
		else
		{
			glDisable(GL_TEXTURE_CUBE_MAP);
			return false;
		}
	}
	glDisable(GL_TEXTURE_CUBE_MAP);
	return true;
}

#endif

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "CustomOperators.h"

using namespace std;
using namespace glm;

class Wall
{
public:
    vector<vec3> vertices;
    vector<vec3> normals;
    vector<vec2> uvs;
    vector<uint> indices;

    float numWindows;

    Wall(vec3 corner1, vec3 corner3, float wallThickness);
};

void createHPlane(vector<vec3> &vertices, vector<vec3> &normals, vector<uint> &indices,
  vector<vec2> &uvs, vec3 corner1, vec3 corner3)
{
  vertices.clear();
  normals.clear();
  indices.clear();
  uvs.clear();

  vec3 hProj = corner1-corner3;
  hProj.z = 0;
  vec3 corner2 = corner1-hProj;
  vec3 corner4 = corner3+hProj;

  vertices.push_back(corner1);
  vertices.push_back(corner2);
  vertices.push_back(corner3);
  vertices.push_back(corner4);

  uvs.push_back(vec2(1,0));
  uvs.push_back(vec2(0,0));
  uvs.push_back(vec2(0,1));
  uvs.push_back(vec2(1,1));

  for(uint i=0; i<3; i++)
    indices.push_back(i);

  for(uint i=0; i<3; i++)
    indices.push_back((i+2)%4);

  vec3 normal = vec3(0,1,0);

  for(uint i=0; i<4; i++)
    normals.push_back(normal);
}

void createVPlane(vector<vec3> &vertices, vector<vec3> &normals, vector<uint> &indices,
  vector<vec2> &uvs, vec3 corner1, vec3 corner3)
{
  vertices.clear();
  normals.clear();
  indices.clear();
  uvs.clear();

  vec3 hProj = corner1-corner3;
  hProj.y = 0;
  vec3 corner2 = corner1-hProj;
  vec3 corner4 = corner3+hProj;

  vertices.push_back(corner1);
  vertices.push_back(corner2);
  vertices.push_back(corner3);
  vertices.push_back(corner4);

  uvs.push_back(vec2(1,0));
  uvs.push_back(vec2(0,0));
  uvs.push_back(vec2(0,1));
  uvs.push_back(vec2(1,1));

  for(uint i=0; i<3; i++)
    indices.push_back(i);

  for(uint i=0; i<3; i++)
    indices.push_back((i+2)%4);

  vec3 normal = normalize(
    cross(vertices[1]-vertices[0],
          vertices[2]-vertices[1]));

  for(uint i=0; i<4; i++)
    normals.push_back(normal);
}

void createPrism(vector<vec3> &vertices, vector<vec3> &normals, vector<uint> &indices,
  vector<vec2> &uvs, vec3 corner1, vec3 corner3, float height)
{
  vertices.clear();
  normals.clear();
  indices.clear();

  vector<vec3> verts, norms;
  vector<vec2> textCoords;
  vector<uint> indexes;
  createHPlane(verts, norms, indexes, textCoords, corner1, corner3);
  vertices = verts;
  normals = norms;
  indices = indexes;
  uvs = textCoords;

  for(uint i=0; i<verts.size(); i++)
  {
    verts[i].y += height;
    vertices.push_back(verts[i]);
  }

  for(vec3 normal:norms)
    normals.push_back(normal);

  uint offset = verts.size();
  for(uint i: indexes)
  {
    indices.push_back(i+offset);
  }

  uvs.insert(uvs.end(), uvs.begin(), uvs.end());

  vec3 diagonal = corner1-corner3;
  diagonal.z = 0;
  vector<vec3> corners = {corner1, corner1-diagonal, corner3, corner3+diagonal};

  for(uint i=0; i<corners.size(); i++)
  {
    createVPlane(verts, norms, indexes, textCoords, corners[i], corners[(i+1)%corners.size()]+vec3(0,height,0));

    for(uint i=0; i<verts.size(); i++)
    {
      vertices.push_back(verts[i]);
    }

    for(vec3 normal:norms)
      normals.push_back(normal);

    offset+=verts.size();
    for(uint i: indexes)
    {
      indices.push_back(i+offset);
    }

    for(vec2 v : textCoords)
      uvs.push_back(v);
  }
}

Wall::Wall(vec3 corner1, vec3 corner3, float wallThickness)
{
		//Number of windows
		float numWindows = 2.f;
		float numSegments = 2.f*numWindows + 1.f;

		//We are rendering windows
		bool window = true;

		vec3 front = vec3(corner1.x - corner3.x, 0.f, 0.f);
		vec3 side = vec3(0.f, 0.f, corner1.z - corner3.z);
		vec3 pos;

		if(length(front) > length(side))
			pos = front/(numSegments);
		else
			pos = side/(numSegments);

		//Init values
		bool first = true;
		vector<vec3> v;
		vector<vec3> n;
    vector<vec2> tCoords;
		vector<uint> in;
		uint addV;

		vec3 startpos = corner1 - (pos*(numSegments-1));
		vec3 dpos = vec3(0.f);

		//Iterate over windows and wall segments
		for(int i = 0; i < numSegments ; i++)
		{
			if(first)
			{
				createPrism(vertices, normals, indices,  uvs, startpos + dpos, corner3 + dpos, wallThickness);

				v = vertices;
				n = normals;
				in = indices;
        tCoords = uvs;

				addV = vertices.size();
				first = false;
			}

			else
			{
				dpos += pos;
				if(((i+1) % 2) == 0)
					continue;

				createPrism(vertices, normals, indices, uvs, startpos + dpos, corner3 + dpos, wallThickness);

				for(uint i = 0; i < vertices.size(); i++)
					v.push_back(vertices[i]);
				for(uint i = 0; i < normals.size(); i++)
					n.push_back(normals[i]);
				for(uint i = 0; i < indices.size(); i++)
					in.push_back(addV + indices[i]);
        for(uint i = 0; i < uvs.size(); i++)
          tCoords.push_back(uvs[i]);



				addV += vertices.size();
			}
		}

		//If the opening is a window...
		//Change the boolean to alternate between windows and doors
		//Draw the bottom component of the wall
		if(window)
		{
			createPrism(vertices, normals, indices, uvs, corner1, corner3, wallThickness/3.f);

			for(uint i = 0; i < vertices.size(); i++)
				v.push_back(vertices[i]);
			for(uint i = 0; i < normals.size(); i++)
				n.push_back(normals[i]);
			for(uint i = 0; i < indices.size(); i++)
				in.push_back(addV + indices[i]);
      for(uint i = 0; i < uvs.size(); i++)
        tCoords.push_back(uvs[i]);


			addV += vertices.size();
		}

		//Top component does not depend on door or window position
		//Top component of the wall
		createPrism(vertices, normals, indices, uvs, corner1, corner3, wallThickness/3.f);

		for(uint i = 0; i < vertices.size(); i++)
		{
			if(wallThickness == -1)
				vertices[i].y -= 2.f/3.f;
			v.push_back(vertices[i]);
		}
		for(uint i = 0; i < normals.size(); i++)
			n.push_back(normals[i]);
		for(uint i = 0; i < indices.size(); i++)
			in.push_back(addV + indices[i]);
    for(uint i = 0; i < uvs.size(); i++)
      tCoords.push_back(uvs[i]);

		//Save these components
		vertices = v;
		normals = n;
		indices = in;
    uvs = tCoords;
}
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
  createPrism(vertices, normals, indices, uvs, corner1, corner3, wallThickness);
}
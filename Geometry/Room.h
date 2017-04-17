#include <vector>
#include <glm/glm.hpp>
#include "CustomOperators.h"

using namespace std;
using namespace glm;

class Room;

void createHPlane(vector<vec3> &vertices, vector<vec3> &normals, vector<uint> &indices, vec3 corner1, vec3 corner3)
{
  vertices.clear();
  normals.clear();
  indices.clear();

  vec3 hProj = corner1-corner3;
  hProj.z = 0;
  vec3 corner2 = corner1-hProj;
  vec3 corner4 = corner3+hProj;

  vertices.push_back(corner1);
  vertices.push_back(corner2);
  vertices.push_back(corner3);
  vertices.push_back(corner4);

  for(uint i=0; i<3; i++)
    indices.push_back(i);

  for(uint i=0; i<3; i++)
    indices.push_back((i+2)%4);

  vec3 normal = vec3(0,1,0);

  for(uint i=0; i<4; i++)
    normals.push_back(normal);
}

void createVPlane(vector<vec3> &vertices, vector<vec3> &normals, vector<uint> &indices, vec3 corner1, vec3 corner3)
{
  vertices.clear();
  normals.clear();
  indices.clear();

  vec3 hProj = corner1-corner3;
  hProj.y = 0;
  vec3 corner2 = corner1-hProj;
  vec3 corner4 = corner3+hProj;

  vertices.push_back(corner1);
  vertices.push_back(corner2);
  vertices.push_back(corner3);
  vertices.push_back(corner4);

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

void createPrism(vector<vec3> &vertices, vector<vec3> &normals, vector<uint> &indices, vec3 corner1, vec3 corner3, float height)
{
  vertices.clear();
  normals.clear();
  indices.clear();

  vector<vec3> verts, norms;
  vector<uint> indexes;
  createHPlane(verts, norms, indexes, corner1, corner3);
  vertices = verts;
  normals = norms;
  indices = indexes;

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

  vec3 diagonal = corner1-corner3;
  diagonal.z = 0;
  vector<vec3> corners = {corner1, corner1-diagonal, corner3, corner3+diagonal};

  for(uint i=0; i<corners.size(); i++)
  {
    createVPlane(verts, norms, indexes, corners[i], corners[(i+1)%corners.size()]+vec3(0,height,0));

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
  }
}

vec3 intersectingPoint(vec3 vec1Pos, vec3 vec1Dir, vec3 vec2Pos, vec3 vec2Dir) 
{
  /*
  vec1Pos + t*vec1Dir = vec2Pos + s*vec2Dir
  t*vec1Dir - s*vec2Dir = vec2Pos - vec1Pos

  t*(vec1Dir.x) - s*(vec2Dir.x) = vec2Pos.x - vec1Pos.x
  t*(vec1Dir.y) - s*(vec2Dir.y) = vec2Pos.y - vec1Pos.y

  For vec2Dir, either x or y is 0

  So, either
      t*(vec1Dir.x) = vec2Pos.x - vec1Pos.x
      t*(vec1Dir.y) - s*(vec2Dir.y) = vec2Pos.y - vec1Pos.y
  Or
      t*(vec1Dir.x) - s*(vec2Dir.x) = vec2Pos.x - vec1Pos.x
      t*(vec1Dir.y) = vec2Pos.y - vec1Pos.y
  */
  float t = 0.f;
  if (vec2Dir.x == 0) {
    t = (vec2Pos.x - vec1Pos.x) / vec1Dir.x;
  } else if (vec2Dir.z == 0) {
    t = (vec2Pos.z - vec1Pos.z) / vec1Dir.z;
  }

  return (vec1Pos + t*vec1Dir);
}

class Wall
{
public:
    vector<vec3> vertices;
    vector<vec3> normals;
    vector<uint> indices;

    Wall(vec3 corner1, vec3 corner3, float wallThickness);
};

Wall::Wall(vec3 corner1, vec3 corner3, float wallThickness)
{
  createPrism(vertices, normals, indices, corner1, corner3, wallThickness);
}

class Room
{
public:
    int type;   //public = 0, private = 1, extra = 2
    float size;
    float wallThickness = 0.05;
    int index;
    vector<Room*> neighbours;
    Room* parent;

    vector<Wall*> walls; 
    vector<vec3> doors;

    vec3 basePos;
    vec3 upRightPos;
    vec3 downLeftPos;

    float upExpand = 0.005f;
    float rightExpand = 0.005f;
    float downExpand = 0.005f;
    float leftExpand = 0.005f;

    Room(int _type, float _size, int _index):type(_type), size(_size), index(_index){}

    vector<Room*> createRooms(int type, float size, int baseIndex, int maxNumRooms);
    float area();

    void expand();
    void getGeometry(vector<vec3> &verts, vector<vec3> &norms, vector<uint> &indexes);
    void setRoomGeometry();
    void setDoors();

    vec3 getDoorPos();
};

void getWallByQuadrant(Room* room, vec3& wallVector, vec3& wallStartPos, vec3& edgeVector) 
{
    vec3 displacement = wallStartPos - room->basePos;
    if (abs(normalize(displacement).z) > abs(normalize(edgeVector).z)) {
      wallVector = vec3(0,0, displacement.z);
    } else {
      wallVector = vec3(displacement.x, 0,0);
    }
}

vec3 Room::getDoorPos() {
  // door is placed at the intersection between the edge of the graph and the wall of this room
  if (index == 0) {
    return vec3(0.f);
  } 

  else 
  {
    vec3 edgeVector = parent->basePos - basePos;
    vec3 edgeStartPos = basePos;
    vec3 wallVector;
    vec3 wallStartPos;

    // first quadrant, top right 
    if (edgeVector.x >= 0.f && edgeVector.z >= 0.f) {
      wallStartPos = upRightPos;
      getWallByQuadrant(this, wallVector, wallStartPos, edgeVector);
      if (wallVector.z == 0.f) 
          return vec3(0.f);
    }

    // second quadrant, top left 
    else if (edgeVector.x < 0.f && edgeVector.z >= 0.f) {
      wallStartPos = vec3(downLeftPos.x, basePos.y, upRightPos.z);
      getWallByQuadrant(this, wallVector, wallStartPos, edgeVector);
      if (wallVector.x == 0.f) 
          return vec3(0.f);
    }

    // third quadrant, bottom left 
    else if (edgeVector.x <= 0.f && edgeVector.z < 0.f) {
      wallStartPos = downLeftPos;
      getWallByQuadrant(this, wallVector, wallStartPos, edgeVector);
      if (wallVector.z == 0.f) 
          return vec3(0.f);
    }

    // fourth quadrant, bottom right 
    else if (edgeVector.x > 0.f && edgeVector.z < 0.f) {
      wallStartPos = vec3(upRightPos.x, basePos.y, downLeftPos.z);
      getWallByQuadrant(this, wallVector, wallStartPos, edgeVector);
      if (wallVector.x == 0.f) 
          return vec3(0.f);
    }

    return (intersectingPoint(edgeStartPos, edgeVector, wallStartPos, wallVector));
  }
}


void Room::expand()
{
  upRightPos.x  += rightExpand;
  upRightPos.z  += upExpand;
  downLeftPos.x -= leftExpand;
  downLeftPos.z -= downExpand;
}

void Room::getGeometry(vector<vec3> &verts, vector<vec3> &norms, vector<uint> &indexes)
{
  verts.clear();
  norms.clear();
  indexes.clear();

  for(Wall *w: walls)
  {
    for(uint i=0; i < w->indices.size(); i++)
    {
      indexes.push_back(w->indices[i]+verts.size());
    }
    
    verts.insert( verts.end(), w->vertices.begin(), w->vertices.end() );
    norms.insert( norms.end(), w->normals.begin(), w->normals.end() );
  }
}

void Room::setRoomGeometry()
{
  walls.clear();
  vec3 corner1 = (upRightPos);
  vec3 corner3 = (downLeftPos);

  vec3 hProj = corner1-corner3;
  hProj.z = 0;
  vec3 corner2 = corner1-hProj;
  vec3 corner4 = corner3+hProj;

  vector<vec3> corners;

  corners.push_back(corner1);
  corners.push_back(corner2);
  corners.push_back(corner3);
  corners.push_back(corner4);

  walls.push_back(new Wall(corner1, corner3, wallThickness));

  for(uint i=0; i<4; i++)
  {
    vec3 offset =  corners[(i+2)%corners.size()] - corners[(i+1)%corners.size()];
    vec3 wallCorner = (corners[(i+1)%corners.size()]+normalize(offset)*wallThickness);
    walls.push_back(new Wall(corners[i], wallCorner+normalize(offset)*wallThickness, -1));
  }
}

vector<Room*> Room::createRooms(int type, float size, int baseIndex, int maxNumRooms)
{
    int numNewRooms = rand() % maxNumRooms + 1;

    vector<Room*> addedRooms;

    for(int i = 0; i < numNewRooms; i++)
    {
        Room *newRoom = new Room(type, size, baseIndex + i);
        newRoom->neighbours.push_back(this);
        newRoom->parent = this;
        neighbours.push_back(newRoom);
        addedRooms.push_back(newRoom);
    }
    return addedRooms;
}

float Room::area() {
    float xLength = upRightPos.x - downLeftPos.x;
    float zLength = upRightPos.z - downLeftPos.z;
    return (xLength * zLength);
}

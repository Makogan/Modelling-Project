#include <vector>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class Room
{
public:
    int type;   //public = 0, private = 1, extra = 2
    float size;
    float wallThickness = 0.05;
    int index;
    vector<Room*> neighbours;
    Room* parent;

    vector<vec3> vertices;
    vector<vec3> normals;
    vector<uint> indices;

    vec2 basePos;
    vec2 upRightPos;
    vec2 downLeftPos;

    float upExpand = 0.005f;
    float leftExpand = 0.005f;
    float downExpand = 0.005f;
    float rightExpand = 0.005f;

    bool renderWall[4] = {true, true, true, true}; //up, left, down, right (in CCW order)

    Room(int _type, float _size, int _index):type(_type), size(_size), index(_index){}

    vector<Room*> createRooms(int type, float size, int baseIndex, int maxNumRooms);
    float area();
    void setRoomGeometry(bool is3D, vector<vec3> &vertices, vector<vec3> &normals, vector<uint> &indices);
    vec2 getDoorPos();
};

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
    float yLength = upRightPos.y - downLeftPos.y;
    return (xLength * yLength);
}

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

void Room::setRoomGeometry(bool is3D, vector<vec3> &vertices, vector<vec3> &normals, vector<uint> &indices)
{ 
  vec3 corner1 = vec3(upRightPos.x, -10.f, upRightPos.y);
  vec3 corner3 = vec3(downLeftPos.x, -10.f, downLeftPos.y);

  vec3 hProj = corner1-corner3;
  hProj.z = 0.f;
  vec3 corner2 = corner1-hProj;
  vec3 corner4 = corner3+hProj;

  vector<vec3> corners;

  corners.push_back(corner1);
  corners.push_back(corner2);
  corners.push_back(corner3);
  corners.push_back(corner4);

  if (is3D)
    createPrism(vertices, normals, indices, corner1, corner3, wallThickness);
  else
    createPrism(vertices, normals, indices, corner1, corner3, 0.f);

  for(uint i=0; i<4; i++) {
    if (renderWall[i]) {
      vector<vec3> verts, norms;
      vector<uint> indexes;

      vec3 offset =  corners[(i+2)%corners.size()] - corners[(i+1)%corners.size()];
      vec3 wallCorner = (corners[(i+1)%corners.size()] + normalize(offset) * 2.f * wallThickness);

      if (is3D)
        createPrism(verts, norms, indexes, corners[i], wallCorner, -1);
      else
        createPrism(verts, norms, indexes, corners[i], wallCorner, 0);


      uint iOffset = vertices.size();
      for(uint i: indexes)
        indices.push_back(i+iOffset);

      vertices.insert( vertices.end(), verts.begin(), verts.end());
      normals.insert(normals.end(), norms.begin(), norms.end());
    }
  }
}

void getWallByQuadrant(Room* room, vec2& wallVector, vec2& wallStartPos, vec2& edgeVector) {
    vec2 displacement = wallStartPos - room->basePos;
    if (abs(normalize(displacement).y) > abs(normalize(edgeVector).y)) {
      wallVector = vec2(0.f, displacement.y);
    } else {
      wallVector = vec2(displacement.x, 0.f);
    }
}

vec2 intersectingPoint(vec2 vec1Pos, vec2 vec1Dir, vec2 vec2Pos, vec2 vec2Dir) {
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
  } else if (vec2Dir.y == 0) {
    t = (vec2Pos.y - vec1Pos.y) / vec1Dir.y;
  }

  return (vec1Pos + t*vec1Dir);
}

vec2 Room::getDoorPos() {
  // door is placed at the intersection between the edge of the graph and the wall of this room
  if (index == 0) {
    return vec2(0.f, 0.f);
  } else {
    vec2 edgeVector = parent->basePos - basePos;
    vec2 edgeStartPos = basePos;
    vec2 wallVector;
    vec2 wallStartPos;

    /* first quadrant, top right */
    if (edgeVector.x >= 0.f && edgeVector.y >= 0.f) {
      wallStartPos = upRightPos;
      getWallByQuadrant(this, wallVector, wallStartPos, edgeVector);
      if (wallVector.y == 0.f && !renderWall[0]) {
          return vec2(0.f, 0.f);
      } else if (wallVector.y != 0.f && !renderWall[3]) {
          return vec2(0.f, 0.f);
      }
    }

    /* second quadrant, top left */
    else if (edgeVector.x < 0.f && edgeVector.y >= 0.f) {
      wallStartPos = vec2(downLeftPos.x, upRightPos.y);
      getWallByQuadrant(this, wallVector, wallStartPos, edgeVector);
      if (wallVector.x == 0.f && !renderWall[1]) {
          return vec2(0.f, 0.f);
      } else if (wallVector.x != 0.f && !renderWall[0]) {
          return vec2(0.f, 0.f);
      }
    }

    /* third quadrant, bottom left */
    else if (edgeVector.x <= 0.f && edgeVector.y < 0.f) {
      wallStartPos = downLeftPos;
      getWallByQuadrant(this, wallVector, wallStartPos, edgeVector);
      if (wallVector.y == 0.f && !renderWall[2]) {
          return vec2(0.f, 0.f);
      } else if (wallVector.y != 0.f && !renderWall[1]) {
          return vec2(0.f, 0.f);
      }
    }

    /* fourth quadrant, bottom right */
    else if (edgeVector.x > 0.f && edgeVector.y < 0.f) {
      wallStartPos = vec2(upRightPos.x, downLeftPos.y);
      getWallByQuadrant(this, wallVector, wallStartPos, edgeVector);
      if (wallVector.x == 0.f && !renderWall[3]) {
          return vec2(0.f, 0.f);
      } else if (wallVector.x != 0.f && !renderWall[2]) {
          return vec2(0.f, 0.f);
      }
    }

    return (intersectingPoint(edgeStartPos, edgeVector, wallStartPos, wallVector));
  }
}
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
    float rightExpand = 0.005f;
    float downExpand = 0.005f;
    float leftExpand = 0.005f;

    Room(int _type, float _size, int _index):type(_type), size(_size), index(_index){}

    vector<Room*> createRooms(int type, float size, int baseIndex, int maxNumRooms);
    float area();

    void getGeometry(vector<vec3> &verts, vector<vec3> &norms, vector<uint> &indexes);
    void setRoomGeometry();
};

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

  vec3 normal = -normalize(
    cross(vertices[1]-vertices[0],
          vertices[2]-vertices[1]));

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
    normals.push_back(-normal);

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

void Room::getGeometry(vector<vec3> &verts, vector<vec3> &norms, vector<uint> &indexes)
{
  verts = vertices;
  norms = normals;
  indexes=indices;
}

void Room::setRoomGeometry()
{
  vec3 corner1 = vec3(upRightPos.x, -10, upRightPos.y);
  vec3 corner3 = vec3(downLeftPos.x, -10, downLeftPos.y);

  vec3 hProj = corner1-corner3;
  hProj.y = 0;
  vec3 corner2 = corner1-hProj;
  vec3 corner4 = corner3+hProj;

  vector<vec3> corners;

  corners.push_back(corner1);
  corners.push_back(corner2);
  corners.push_back(corner3);
  corners.push_back(corner4);

  createPrism(vertices, normals, indices, corner1, corner3, wallThickness);

  for(uint i=0; i<4; i++)
  {
    vector<vec3> prismBase;
    prismBase.push_back(corners[i]);
    prismBase.push_back(corners[(i+1)%corners.size()]);

    vec3 offset =  corners[(i+2)%corners.size()] - corners[(i+1)%corners.size()];
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
    float yLength = upRightPos.y - downLeftPos.y;
    return (xLength * yLength);
}

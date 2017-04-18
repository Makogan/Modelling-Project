#include <vector>
#include <glm/glm.hpp>
#include "Wall.h"

using namespace std;
using namespace glm;

class Room
{
public:
    int type;   //public = 0, private = 1, extra = 2
    float size;
    float wallThickness = 0.05f;
    int index;
    vector<Room*> neighbours;
    vector<vec3> doors;
    Room* parent;

    vec3 basePos;
    vec3 upRightPos;
    vec3 downLeftPos;

    bool renderWall[4] = {true, true, true, true};
    vector<Wall*> walls;

    float upExpand = 0.005f;
    float leftExpand = 0.005f;
    float downExpand = 0.005f;
    float rightExpand = 0.005f;

    Room(int _type, float _size, int _index):type(_type), size(_size), index(_index){}

    vector<Room*> createRooms(int type, float size, int baseIndex, int maxNumRooms);
    float area();

    void setRoomGeometry(bool is3D);
    void getRoomGeometry(vector<vec3> &verts, vector<vec3> &norms, vector<uint> &indexes);

    void increaseArea ();
    void initializeExpansionRates();

    void setDoorPos();
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
    float zLength = upRightPos.z - downLeftPos.z;
    return (xLength * zLength);
}

void Room::setRoomGeometry(bool is3D)
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

  if (is3D)
    walls.push_back(new Wall(corner1, corner3, wallThickness));
  else
    walls.push_back(new Wall(corner1, corner3, 0.f));

  for(uint i=0; i<4; i++)
  {
    if (renderWall[i]) {
      vec3 offset =  corners[(i+2)%corners.size()] - corners[(i+1)%corners.size()];
      vec3 wallCorner = (corners[(i+1)%corners.size()]+normalize(offset)*wallThickness);

      if (is3D)
        walls.push_back(new Wall(corners[i], wallCorner+normalize(offset)*wallThickness, -1));
    }
  }
}

void Room::getRoomGeometry(vector<vec3> &verts, vector<vec3> &norms, vector<uint> &indexes)
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

void getWallByQuadrant(Room* room, vec3& wallVector, vec3& wallStartPos, vec3& edgeVector)
{
    vec3 displacement = wallStartPos - room->basePos;
    if (abs(normalize(displacement).z) > abs(normalize(edgeVector).z)) {
      wallVector = vec3(0.f, 0.f, displacement.z);
    } else {
      wallVector = vec3(displacement.x, 0.f, 0.f);
    }
}

vec3 intersectingPoint(vec3 vec1Pos, vec3 vec1Dir, vec3 vec2Pos, vec3 vec2Dir) {
  /*
  vec1Pos + t*vec1Dir = vec2Pos + s*vec2Dir
  t*vec1Dir - s*vec2Dir = vec2Pos - vec1Pos

  t*(vec1Dir.x) - s*(vec2Dir.x) = vec2Pos.x - vec1Pos.x
  t*(vec1Dir.y) - s*(vec2Dir.y) = vec2Pos.y - vec1Pos.y

  if vec1Dir.x or vec1Dir.y = 0 {
  
  vec1Pos.x - vec2Pos.x = s * vec2Dir.x
  vec1Pos.y - vec2Pos.y = s * vec2Dir.y

  }

  t = (vec2Pos.x - vec1Pos.x + s*(vec2Dir.x)) / vec1Dir.x
  t = ((vec2Pos.x - vec1Pos.x) / vec1Dir.x) + (s * vec2Dir.x / vec1Dir.x)

  s = (vec2Pos.y - vec1Pos.y + t*(vec1Dir.y)) / vec2Dir.y
  s = ((vec2Pos.y - vec1Pos.y) / vec2Dir.y) + (t * vec1Dir.y / vec2Dir.y)

  s = term2 + (term1 + (s * vec2Dir.x / vec1Dir.x) * vec1Dir.y / vec2Dir.y)
  s = term2 + (term1 / vec2Dir.y) + (s * vec2Dir.x / vec1Dir.x * vec1Dir.y / vec2Dir.y)

  s = term2 + (term1 / vec2Dir.y) + (s * term3)
  s - (s * term3) = term2 + (term1 / vec2Dir.y)
  s (1 - term3) = term2 + (term1 / vec2Dir.y)
  s = (term2 + (term1 / vec2Dir.y)) / (1 - term3)
  */
  
  float t = 0.f;
  float s = 0.f;
  bool useT;

  if (vec1Dir.x == 0 && vec2Dir.x != 0) {
    useT = false;
    s = (vec1Pos.x - vec2Pos.x) / vec2Dir.x;
  }
  else if (vec1Dir.z == 0 && vec2Dir.y != 0) {
    s = (vec1Pos.z - vec2Pos.z) / vec2Dir.z;
  }
  else if (vec2Dir.x == 0 && vec1Dir.x != 0) {
    useT = true;
    t = (vec2Pos.x - vec1Pos.x) / vec1Dir.x;
  }
  else if (vec2Dir.z == 0 && vec1Dir.z != 0) {
    useT = true;
    t = (vec2Pos.z - vec1Pos.z) / vec1Dir.z;
  }
  else if (vec1Dir.x != 0 && vec2Dir.z != 0 && vec2Dir.z != 0) {
    useT = false;
    float term1 = (vec2Pos.x - vec1Pos.x) / vec1Dir.x;
    float term2 = (vec2Pos.z - vec1Pos.z) / vec2Dir.z;
    float term3 = (vec2Dir.x / vec1Dir.x) * (vec1Dir.z / vec2Dir.z);
    s = (term2 + (term1 / vec2Dir.z)) / (1.f - term3);
  }

  if (useT)
    return (vec1Pos + t*vec1Dir);
  else
    return (vec2Pos + s*vec2Dir);
}

void Room::setDoorPos() {
  // door is placed at the intersection between the edge of the graph and the wall of this room
  if (index == 0) {
    return;
  } else {
    vec3 edgeVector = parent->basePos - basePos;
    vec3 edgeStartPos = basePos;
    vec3 wallVector;
    vec3 wallStartPos;

    /* first quadrant, top right */
    if (edgeVector.x >= 0.f && edgeVector.z >= 0.f) {
      wallStartPos = upRightPos;
      getWallByQuadrant(this, wallVector, wallStartPos, edgeVector);
      if (wallVector.z == 0.f && !renderWall[0] && !parent->renderWall[2])
          return;
       else if (wallVector.z != 0.f && !renderWall[3] && !parent->renderWall[1])
          return;
    }

    /* second quadrant, top left */
    else if (edgeVector.x < 0.f && edgeVector.z >= 0.f) {
      wallStartPos = vec3(downLeftPos.x, basePos.y, upRightPos.z);
      getWallByQuadrant(this, wallVector, wallStartPos, edgeVector);
      if (wallVector.x == 0.f && !renderWall[1] && !parent->renderWall[3])
          return;
       else if (wallVector.x != 0.f && !renderWall[0] && !parent->renderWall[2])
          return;
    }

    /* third quadrant, bottom left */
    else if (edgeVector.x <= 0.f && edgeVector.z < 0.f) {
      wallStartPos = downLeftPos;
      getWallByQuadrant(this, wallVector, wallStartPos, edgeVector);
      if (wallVector.z == 0.f && !renderWall[2] && !parent->renderWall[0])
          return;
       else if (wallVector.z != 0.f && !renderWall[1] && !parent->renderWall[3])
          return;
    }

    /* fourth quadrant, bottom right */
    else if (edgeVector.x > 0.f && edgeVector.z < 0.f) {
      wallStartPos = vec3(upRightPos.x, basePos.y, downLeftPos.z);
      getWallByQuadrant(this, wallVector, wallStartPos, edgeVector);
      if (wallVector.x == 0.f && !renderWall[3] && !parent->renderWall[1])
          return;
       else if (wallVector.x != 0.f && !renderWall[2] && !parent->renderWall[0])
          return;
    }

    if (normalize(abs(edgeVector)) != normalize(abs(wallVector))) {
      vec3 doorPos = (intersectingPoint(edgeStartPos, edgeVector, wallStartPos, wallVector));
      doors.push_back(doorPos);
      parent->doors.push_back(doorPos);
    }
  }
}

void Room::increaseArea () {
  if (area() < size) {
    if (rightExpand > 0.f)
      upRightPos.x += rightExpand;
    else
      rightExpand = 0.f;

    if (upExpand > 0.f)
      upRightPos.z += upExpand;
    else
      upExpand = 0.f;

    if (leftExpand > 0.f)
      downLeftPos.x -= leftExpand;
    else
      leftExpand = 0.f;

    if (downExpand > 0.f)
      downLeftPos.z -= downExpand;
    else
      downExpand = 0.f;

    upExpand += 0.0001f;
    rightExpand += 0.0001f;
    downExpand += 0.0001f;
    leftExpand += 0.0001f;
  }
}

void Room::initializeExpansionRates() {
  upRightPos = basePos;
  downLeftPos = basePos;

  vec3 dirVector;
  if (index == 0)
    dirVector = basePos;
  else
    dirVector = (basePos - parent->basePos);

  if (-dirVector.x < 0.f) {
   leftExpand += 0.001f;
   rightExpand -= 0.001f;
  } else if (-dirVector.x > 0.f) {
   rightExpand += 0.001f;
   leftExpand -= 0.001f;
  }

  if (-dirVector.z < 0.f) {
   downExpand += 0.001f;
   upExpand -= 0.001f;
  } else if (-dirVector.z > 0.f) {
   upExpand += 0.001f;
   downExpand -= 0.001f;
  }
}
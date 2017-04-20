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
    vector<vec3> roomWindows;
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

    void setDoorPos(Room* otherRoom);
    void findWindowsInRoom(vector<vec3>& windows, vec3& frontDoorRoom, vec3& frontDoorPerim);
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
      vec3 wallCorner = (corners[(i+1)%corners.size()] + normalize(offset)*wallThickness);

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

void intersectingPoint(vec3 line1Pos, vec3 line1Dir, vec3 line2Pos, vec3 line2Dir, vec3 &outputVec) {
  float t = 0.f;
  bool useT = false;

  if (line2Dir.x == 0) {
    useT = true;
    t = (line2Pos.x - line1Pos.x) / line1Dir.x;
  }
  else if (line2Dir.z == 0) {
    useT = true;
    t = (line2Pos.z - line1Pos.z) / line1Dir.z;
  }

  if (useT) {
    outputVec = (line1Pos + t*line1Dir);
    return;
  }

  if(line1Dir.x == 0.f || line2Dir.x == 0.f)
    return;

  float line1Slope = line1Dir.z / line1Dir.x;
  float line2Slope = line2Dir.z / line2Dir.x;

  if (abs(line1Slope - line2Slope) < 0.01f)
    return;

  float line1Intercept = line1Pos.z - (line1Slope * line1Pos.x);
  float line2Intercept = line2Pos.z - (line2Slope * line2Pos.x);

  float intsectX = (line2Intercept - line1Intercept) / (line1Slope - line2Slope);
  float intsectZ = ((line1Slope * line2Intercept) - (line2Slope * line1Intercept)) / (line1Slope - line2Slope);

  outputVec = vec3(intsectX, line1Pos.y, intsectZ);
}

void Room::setDoorPos(Room* otherRoom) {
  // door is placed at the intersection between the edge of the graph and the wall of this room
  vec3 edgeVector = otherRoom->basePos - basePos;
  vec3 edgeStartPos = basePos;
  vec3 wallVector;
  vec3 wallStartPos;

  /* first quadrant, top right */
  if (edgeVector.x >= 0.f && edgeVector.z >= 0.f) {
    wallStartPos = upRightPos;
    getWallByQuadrant(this, wallVector, wallStartPos, edgeVector);
    if (wallVector.z == 0.f && !renderWall[0])
        return;
     else if (wallVector.z != 0.f && !renderWall[3])
        return;
  }

  /* second quadrant, top left */
  else if (edgeVector.x < 0.f && edgeVector.z >= 0.f) {
    wallStartPos = vec3(downLeftPos.x, basePos.y, upRightPos.z);
    getWallByQuadrant(this, wallVector, wallStartPos, edgeVector);
    if (wallVector.x == 0.f && !renderWall[1])
        return;
     else if (wallVector.x != 0.f && !renderWall[0])
        return;
  }

  /* third quadrant, bottom left */
  else if (edgeVector.x <= 0.f && edgeVector.z < 0.f) {
    wallStartPos = downLeftPos;
    getWallByQuadrant(this, wallVector, wallStartPos, edgeVector);
    if (wallVector.z == 0.f && !renderWall[2])
        return;
     else if (wallVector.z != 0.f && !renderWall[1])
        return;
  }

  /* fourth quadrant, bottom right */
  else if (edgeVector.x > 0.f && edgeVector.z < 0.f) {
    wallStartPos = vec3(upRightPos.x, basePos.y, downLeftPos.z);
    getWallByQuadrant(this, wallVector, wallStartPos, edgeVector);
    if (wallVector.x == 0.f && !renderWall[3])
        return;
     else if (wallVector.x != 0.f && !renderWall[2])
        return;
  }

  vec3 doorPos = vec3(0.f);
  (intersectingPoint(edgeStartPos, edgeVector, wallStartPos, wallVector, doorPos));
  if (doorPos != vec3(0.f)) {
    doors.push_back(doorPos);
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

float pointToLineDist(vec3 point, vec3 linePos, vec3 lineDir) {
  float a = lineDir.x;
  float b = lineDir.z;
  float c = -(a * linePos.x) - (b * linePos.z);
  float distance = abs((a * point.x) + (b * point.z) + c) / sqrt((a*a) + (b*b));
  return distance;
}

void Room::findWindowsInRoom(vector<vec3>& windows, vec3& frontDoorRoom, vec3& frontDoorPerim) {
  roomWindows.clear();

  vec3 corner1 = upRightPos;
  vec3 corner2 = vec3(downLeftPos.x, basePos.y, upRightPos.z);
  vec3 corner3 = downLeftPos;
  vec3 corner4 = vec3(upRightPos.x, basePos.y, downLeftPos.z);

  vec3 wall1 = corner2 - corner1;
  vec3 wall2 = corner3 - corner2;
  vec3 wall3 = corner4 - corner3;
  vec3 wall4 = corner1 - corner4;

  vec3 window;
  for (uint i = 0; i < windows.size(); i++) {
    window = windows[i];
    if (pointToLineDist(window, corner1, wall1) <= 0.021f) {
      roomWindows.push_back(window + vec3(-0.02f, 0.f, 0.f));
      if ((type == 0) && (frontDoorRoom == vec3(0.f))){
        frontDoorPerim = roomWindows.back();
        frontDoorRoom = window;
        roomWindows.pop_back();
        windows.erase(windows.begin() + i);
      }
    }
    else if (pointToLineDist(window, corner2, wall2) <= 0.021f) {
      roomWindows.push_back(window + vec3(0.f, 0.f, -0.02f));
      if ((type == 0) && (frontDoorRoom == vec3(0.f))){
        frontDoorPerim = roomWindows.back();
        frontDoorRoom = window;
        roomWindows.pop_back();
        windows.erase(windows.begin() + i);
      }
    }
    else if (pointToLineDist(window, corner3, wall3) <= 0.021f) {
      roomWindows.push_back(window + vec3(0.02f, 0.f, 0.f));
      if ((type == 0) && (frontDoorRoom == vec3(0.f))){
        frontDoorPerim = roomWindows.back();
        frontDoorRoom = window;
        roomWindows.pop_back();
        windows.erase(windows.begin() + i);
      }
    }
    else if (pointToLineDist(window, corner4, wall4) <= 0.021f) {
      roomWindows.push_back(window + vec3(0.f, 0.f, 0.02f));
      if ((type == 0) && (frontDoorRoom == vec3(0.f))){
        frontDoorPerim = roomWindows.back();
        frontDoorRoom = window;
        roomWindows.pop_back();
        windows.erase(windows.begin() + i);
      }
    }
  }
}
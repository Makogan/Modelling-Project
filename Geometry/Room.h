#include <vector>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class Room
{
public:
    int type;   //public = 0, private = 1, extra = 2
    float size;
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
};

void Room::getGeometry(vector<vec3> &verts, vector<vec3> &norms, vector<uint> &indexes)
{
  verts = vertices;
  norms = normals;
  indexes=indices;
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

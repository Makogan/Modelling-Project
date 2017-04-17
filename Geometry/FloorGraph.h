#include <vector>
#include <queue>
#include <glm/glm.hpp>
#include "Room.h"

using namespace std;

class FloorGraph
{
public:
  vector<Room*> graph;
  vector<vec3> doors;

  FloorGraph();

  void expandRooms();
  void setRoomsPos();
  void getRoomsPos(vector<vec3> &input);
  void addPublicRooms();
  void addOtherRooms( int probability, int randomness,
                      int type, float size, int numRooms,
                      vector<Room*> extantRooms);
  void concatenateRooms(vector <Room*> newRooms);
  void setRoomsFloors();
  void getRoomsOutlines(vector<vec3> &vertices, vector<uint> &indices);
  void getEdges(vector<vec3> &vertices);
  void setDoors();
  void getDoors(vector<vec3> &vertices);
};

FloorGraph::FloorGraph()
{
	addPublicRooms();
	addOtherRooms(15, 60, 1, 5.5f, 2, graph);	//add Private Rooms
	addOtherRooms(5, 35, 2, 2.5f, 1, graph);//add Extra Rooms

	queue<int> queue;

	Room* room = graph[0];
	room->basePos = vec3(0);
	room->upRightPos = room->basePos;
	room->downLeftPos = room->basePos;
	queue.push(room->index);

	while (queue.size() > 0) {
		room = graph[queue.front()];
		queue.pop();

		cout << "Room " << room->index <<
				" of type " << room->type <<
				" and size " << room->size <<
				" has neighbours: ";

	for (Room* neib : room->neighbours)
    {
			cout << neib->index << ", ";
			if (room->index < neib->index) {
				neib->basePos = vec3(float(rand() % 11 - 5),0, float(rand() % 11 - 5));
				queue.push(neib->index);
			}
		}

		if (room->index == 0) {
			cout << "and that's it." << endl;
		} else {
			cout << "and its parent is Room " << room->parent->index << "." << endl;
		}
	}
}

void FloorGraph::getEdges(vector<vec3> &vertices)
{
  for (Room* room : graph) {
    for (Room* neib : room->neighbours) {
      if (room->index < neib->index) {
        vertices.push_back(room->basePos);
        vertices.push_back(neib->basePos);
      }
    }
  }
}

void FloorGraph::getRoomsOutlines(vector<vec3> &vertices, vector<uint> &indices)
{
  vertices.clear();
  indices.clear();
  uint count =0;
  for(Room* room : graph)
  {
    vertices.push_back(vec3(room->upRightPos.x, room->basePos.y, room->upRightPos.z));
    vertices.push_back(vec3(room->downLeftPos.x, room->basePos.y, room->upRightPos.z));
    vertices.push_back(vec3(room->downLeftPos.x, room->basePos.y, room->downLeftPos.z));
    vertices.push_back(vec3(room->upRightPos.x, room->basePos.y, room->downLeftPos.z));

    for(uint i=0; i<5; i++)
    {
      indices.push_back(count+(i)%4);
      indices.push_back(count+(i+1)%4);
    }

    count+=4;
  }
}

bool verticalOverlap(Room *r1, Room *r2)
{
	if(r1->upRightPos.z <= r2->upRightPos.z && r1->upRightPos.z >= r2->downLeftPos.z)
		return true;

	if(r1->downLeftPos.z <= r2->upRightPos.z && r1->upRightPos.z >= r2->downLeftPos.z)
		return true;

	return false;
}

bool horizontalOverlap(Room *r1, Room *r2)
{
	if(r1->upRightPos.x <= r2->upRightPos.x && r1->upRightPos.x >= r2->downLeftPos.x)
		return true;

	if(r1->downLeftPos.x <= r2->upRightPos.x && r1->upRightPos.x >= r2->downLeftPos.x)
		return true;

	return false;
}

/*float signedMin(float x, float y)
{
	if(abs(x)>abs(y))
		return y;
	else
		return x;
}

vec3 solveCollision(vec3 point, float maxX, float minX, float maxZ, float minZ)
{
	if(point.x <= maxX && point.x>=minX && point.z>=minZ && point.z<=maxZ)
	{
		float vOffset;
		if(abs(point.z-maxZ)<abs(point.z-minZ))
			vOffset = point.z-maxZ;
		else
			vOffset = point.z-minZ;

		float hOffset;
		if(abs(point.x-maxX)<abs(point.x-minX))
			hOffset = point.x-maxX;
		else
			hOffset = point.x-minX;

		if(abs(vOffset)>abs(hOffset))
			return vec3(hOffset,0,0);
		else
			return vec3(0,0,vOffset);
	}

	return vec3(0);
}

void fixOverlap(Room *r1, Room *r2)
{
	vec3 corner1 = r1->upRightPos;
	vec3 corner3 = r1->downLeftPos;
	vec3 hProj = corner1-corner3;
	hProj.z = 0;
	vec3 corner2 = corner1-hProj;
	vec3 corner4 = corner3+hProj;

	vector<vec3> corners;
	vector<vec3> corrections;
	corners.push_back(corner1);
	corners.push_back(corner2);
	corners.push_back(corner3);
	corners.push_back(corner4);

	for(uint i=0; i<corners.size(); i++)
	{
		corrections.push_back(
			solveCollision(corners[i], r2->upRightPos.x, r2->downLeftPos.x, 
				r2->upRightPos.z, r2->downLeftPos.z));
	}

	float hCorrection = corrections[0].x;
	float vCorrection = corrections[0].z;
	for(uint i=0; i<corners.size()-1; i++)
	{
		hCorrection = signedMin(corrections[i].x, corrections[i+1].x);
		vCorrection = signedMin(corrections[i].z, corrections[i+1].z);
	}	

	if(hCorrection < 0)
		r1->upRightPos.x+=hCorrection;
	else
		r1->downLeftPos.x+=hCorrection;

	if(vCorrection < 0)
		r1->upRightPos.z+=vCorrection;
	else
		r1->downLeftPos.z+=vCorrection;
}*/

void updateExpansion(Room *r1, Room *r2)
{
	if(r1 == r2)
		return;

	if(r1->upRightPos.z <= r2->upRightPos.z && r1->upRightPos.z >= r2->downLeftPos.z)
	{
		r1->upExpand = 0;
	}

	if(r1->downLeftPos.z <= r2->upRightPos.z && r1->downLeftPos.z >= r2->downLeftPos.z)
	{
		r1->downExpand = 0;
	}

	if(r1->upRightPos.x <= r2->upRightPos.x && r1->upRightPos.x >= r2->downLeftPos.x)
	{
		r1->rightExpand = 0;
	}

	if(r1->downLeftPos.x <= r2->upRightPos.x && r1->downLeftPos.x >= r2->downLeftPos.x)
	{
		r1->leftExpand =0;
	}
}

void FloorGraph::expandRooms()
{
	for (Room* room1 : graph) 
	{
		room1->expand();
	}

	for (Room* room1 : graph) 
	{
		if (room1->area() >= room1->size)
		{
			room1->upExpand=0;
			room1->downExpand=0;
			room1->leftExpand=0;
			room1->rightExpand=0;
		}

		for(Room* room2: graph)
		{
			if (room1 == room2) continue;

			if(verticalOverlap(room1,room2) && horizontalOverlap(room1,room2))
			{
				updateExpansion(room1, room2);
				//fixOverlap(room1, room2);
		    }
		}
	}

}

void FloorGraph::setDoors() 
{
	doors.clear();
	queue<int> queue;

	Room* room = graph[0];
	for (Room* neib : room->neighbours) {
		queue.push(neib->index);
	}

	while (queue.size() > 0) {
		room = graph[queue.front()];
		queue.pop();

		vec3 roomPos = room->getDoorPos();

		if (roomPos.x != 0.f || roomPos.z != 0.f)
			doors.push_back(roomPos);

		for (Room* neib : room->neighbours) {
			if (room->index < neib->index)
				queue.push(neib->index);
		}
	}
}

void FloorGraph::getDoors(vector<vec3> &vertices) {
	vertices.clear();
	for (vec3 pos : doors) {
		vertices.push_back(pos);
	}
}

void FloorGraph::setRoomsPos()
{
	queue<int> queue;

	Room* room = graph[0];
	for (Room* neib : room->neighbours)
  	{
		queue.push(neib->index);
	}

	vector<Room*> siblings;
	Room* papa = new Room(-1, 0, -1);

	while (queue.size() > 0)
	{
		room = graph[queue.front()];
		queue.pop();

	if (papa->index != room->parent->index)
    {
			siblings.clear();
			papa = room->parent;

			for (Room* sibling : papa->neighbours) {
				if (papa->index < sibling->index) {
					siblings.push_back(sibling);
				}
			}

			if (papa->index != 0) {
				vec3 papaDirectionVector = papa->parent->basePos - papa->basePos;
				vec3 thisDirectionVector = siblings[0]->basePos - papa->basePos;

				float numer = dot(papaDirectionVector, thisDirectionVector);
				float denom = length(papaDirectionVector) * length(thisDirectionVector);
				float angle = acos(numer / denom);
				if (angle < (M_PI/2.f)) {
					float phi = (3.f * M_PI / 2.f);
					float oldX = papaDirectionVector.x;
					float oldZ = papaDirectionVector.z;
					vec3 rotatedVec = vec3((oldX * cos(phi)) - (oldZ * sin(phi)), papaDirectionVector.y, 
						(oldZ * cos(phi)) + (oldX * sin(phi)));
					vec3 temp = siblings[0]->basePos;
					temp.y=0;
					siblings[0]->basePos = length(temp) * glm::normalize(rotatedVec);
				}
			}

			float theta = -(M_PI) / siblings.size();
			if (papa->index == 0) {
				theta = -(3.f * M_PI / 2.f) / siblings.size();
			}
			for (int i = 1; i < int(siblings.size()); i++) {
				float oldX = siblings[i - 1]->basePos.x - papa->basePos.x;
				float oldZ = siblings[i - 1]->basePos.z - papa->basePos.z;
				vec3 rotatedVec = glm::normalize(vec3((oldX * cos(theta)) - (oldZ * sin(theta)), papa->basePos.y, 
					(oldZ * cos(theta)) + (oldX * sin(theta))));
				vec3 temp = siblings[i]->basePos-papa->basePos;
				temp.y=0;
				siblings[i]->basePos = (length(temp) * rotatedVec) + papa->basePos;
			}
		}

		if (papa->index != 0) {
			vec3 papaDirectionVector = papa->parent->basePos - papa->basePos;
			vec3 thisDirectionVector = room->basePos - papa->basePos;
			float numer = dot(papaDirectionVector, thisDirectionVector);
			float denom = length(papaDirectionVector) * length(thisDirectionVector);
			float angle = acos(numer / denom) * (180.f / M_PI);
			if (angle < 90.f) {
				room->basePos = 2.2f * glm::normalize(-1.f *  thisDirectionVector) + papa->basePos;
			} else {
				room->basePos = 2.2f * glm::normalize(thisDirectionVector) + papa->basePos;
			}
		} else {
			room->basePos = 2.2f * glm::normalize(room->basePos);
		}

		// if (room->type == 2) {
		// 	room->basePos = ((float(room->size) / float(papa->size)) * (room->basePos - papa->basePos)) + papa->basePos;
		// }

		for (Room* neib : room->neighbours)
		{
			if (room->index < neib->index) {
				queue.push(neib->index);
			}
		}
	}

	for (Room* room : graph)
	{
		vec3 dirVector;
		if (room->index == 0) {
			dirVector = room->basePos;
		} else {
			dirVector = (room->basePos - room->parent->basePos);
		}

		room->upRightPos = room->basePos;
		room->downLeftPos = room->basePos;

		if (-dirVector.x < 0.f) {
			room->leftExpand += 0.001f;
			room->rightExpand -= 0.001f;
		} else if (-dirVector.x > 0.f) {
			room->rightExpand += 0.001f;
			room->leftExpand -= 0.001f;
		}

		if (-dirVector.z < 0.f) {
			room->downExpand += 0.001f;
			room->upExpand -= 0.001f;
		} else if (-dirVector.z > 0.f) {
			room->upExpand += 0.001f;
			room->downExpand -= 0.001f;
		}
	}
}

void FloorGraph::setRoomsFloors()
{
  for(Room* room : graph)
  {
    room->setRoomGeometry();
  }
}

void FloorGraph::getRoomsPos(vector<vec3> &input)
{
  for (Room* room : graph) {
    input.push_back(room->basePos);
  }
}

void FloorGraph::addPublicRooms()
{
  graph.push_back(new Room(0, 8.f, 0));

  uint count = 0;
  Room *currentRoom = graph[count];
  int probability = 10;

  int numNewRooms = 5;

  while(count <= graph.size() - 1)
  {
    if (graph.size() > 5)
      break;  //prevents a house having a million rooms

    int random = rand() % 25;
    if(random <= probability || count < 1)
      concatenateRooms(currentRoom->createRooms(0, 8.5f, graph.size(), numNewRooms));

    numNewRooms--;

    count++;
    currentRoom = graph[count];
  }
  cout << endl;
}

void FloorGraph::addOtherRooms( int probability, int randomness,
                                  int type, float size, int numRooms,
                                  vector<Room*> extantRooms)
{
  for(Room* room : extantRooms) {
    int random = rand() % randomness;
    if(random <= probability)
      concatenateRooms(room->createRooms(type, size, graph.size(), numRooms));
  }
}

void FloorGraph::concatenateRooms(vector<Room*> newRooms)
{
  for(Room* room : newRooms) {
    graph.push_back(room);
  }
}

#include <vector>
#include <queue>
#include <glm/glm.hpp>
#include "Room.h"

using namespace std;

class FloorGraph
{
public:
  vector<Room*> graph;

  FloorGraph();

  void expandRooms();
  void setRoomsPos();
  void getRoomsPos(vector<vec3> &input);
  void addPublicRooms();
  void addOtherRooms( int probability, int randomness,
                      int type, float size, int numRooms,
                      vector<Room*> extantRooms);
  void concatenateRooms(vector <Room*> newRooms);
  void setRoomsFloors(bool is3D);
  void getRoomsOutlines(vector<vec3> &vertices, vector<uint> &indices);
  void getEdges(vector<vec3> &vertices);
};

FloorGraph::FloorGraph()
{
	addPublicRooms();
	addOtherRooms(15, 60, 1, 5.5f, 2, graph);	//add Private Rooms
	addOtherRooms(5, 35, 2, 2.5f, 1, graph);//add Extra Rooms

	queue<int> queue;

	Room* room = graph[0];
	room->basePos = vec2(0.f, 0.f);
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
				neib->basePos = vec2(float(rand() % 11 - 5), float(rand() % 11 - 5));
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

void FloorGraph::getEdges(vector<vec3> &vertices)
{
  for (Room* room : graph) {
    for (Room* neib : room->neighbours) {
      if (room->index < neib->index) {
        vertices.push_back((vec3(room->basePos.x, -10.f, room->basePos.y)));
        vertices.push_back((vec3(neib->basePos.x, -10.f, neib->basePos.y)));
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
    vertices.push_back(vec3(room->upRightPos.x, -10.f, room->upRightPos.y));
    vertices.push_back(vec3(room->downLeftPos.x, -10.f, room->upRightPos.y));
    vertices.push_back(vec3(room->downLeftPos.x, -10.f, room->downLeftPos.y));
    vertices.push_back(vec3(room->upRightPos.x, -10.f, room->downLeftPos.y));

    for(uint i=0; i<5; i++)
    {
      indices.push_back(count+(i)%4);
      indices.push_back(count+(i+1)%4);
    }

    count+=4;
  }
}

void FloorGraph::expandRooms()
{
	for (Room* room1 : graph) {
		if (room1->area() < room1->size) {
			if (room1->rightExpand > 0.f) {
				room1->upRightPos.x += room1->rightExpand;
			} else {
				room1->rightExpand = 0.f;
			}
			if (room1->upExpand > 0.f) {
				room1->upRightPos.y += room1->upExpand;
			} else {
				room1->upExpand = 0.f;
			}
			if (room1->leftExpand > 0.f) {
				room1->downLeftPos.x -= room1->leftExpand;
			} else {
				room1->leftExpand = 0.f;
			}
			if (room1->downExpand > 0.f) {
				room1->downLeftPos.y -= room1->downExpand;
			} else {
				room1->downExpand = 0.f;
			}

			room1->upExpand += 0.0001f;
			room1->rightExpand += 0.0001f;
			room1->downExpand += 0.0001f;
			room1->leftExpand += 0.0001f;
		}

		for (Room* room2 : graph) {
			if (room1 == room2) continue;

			if ((room1->type == 2) && (room2->type == 0)) {
				continue;
			} else if ((room2->type == 2) && (room1->type == 0)) {
				continue;
			}

			if ((room1->basePos.x <= room2->basePos.x) && (room1->basePos.y <= room2->basePos.y)) {
				if ((room1->upRightPos.x >= room2->downLeftPos.x) && (room1->upRightPos.y >= room2->downLeftPos.y)) {
					if (room1->upRightPos.x >= room2->downLeftPos.x + 0.05f) {
						room1->upExpand -= 0.01f;
						room2->downExpand -= 0.01f;
					}
					else if (room1->upRightPos.y >= room2->downLeftPos.y + 0.05f) {
						room1->rightExpand -= 0.01f;
						room2->leftExpand -= 0.01f;
					}
				}
			}
			else if ((room1->basePos.x <= room2->basePos.x) && (room1->basePos.y >= room2->basePos.y)) {
				if ((room1->upRightPos.x >= room2->downLeftPos.x) && (room1->downLeftPos.y <= room2->upRightPos.y)) {
					if (room1->upRightPos.x >= room2->downLeftPos.x + 0.05f) {
						room1->downExpand -= 0.01f;
						room2->upExpand -= 0.01f;
					}
					else if (room1->downLeftPos.y <= room2->upRightPos.y - 0.05f) {
						room1->rightExpand -= 0.01f;
						room2->leftExpand -= 0.01f;
					}
				}
			}
		}
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
	Room* papa = new Room(-1, -1.f, -1);

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
				vec2 papaDirectionVector = papa->parent->basePos - papa->basePos;
				vec2 thisDirectionVector = siblings[0]->basePos - papa->basePos;
				float numer = dot(papaDirectionVector, thisDirectionVector);
				float denom = length(papaDirectionVector) * length(thisDirectionVector);
				float angle = acos(numer / denom);
				if (angle < (M_PI/2.f)) {
					float phi = (3.f * M_PI / 2.f);
					float oldX = papaDirectionVector.x;
					float oldY = papaDirectionVector.y;
					vec2 rotatedVec = vec2((oldX * cos(phi)) - (oldY * sin(phi)), (oldY * cos(phi)) + (oldX * sin(phi)));
					siblings[0]->basePos = length(siblings[0]->basePos) * glm::normalize(rotatedVec);
				}
			}

			float theta = -(M_PI) / siblings.size();
			if (papa->index == 0) {
				theta = -(3.f * M_PI / 2.f) / siblings.size();
			}
			for (int i = 1; i < int(siblings.size()); i++) {
				float oldX = siblings[i - 1]->basePos.x - papa->basePos.x;
				float oldY = siblings[i - 1]->basePos.y - papa->basePos.y;
				vec2 rotatedVec = glm::normalize(vec2((oldX * cos(theta)) - (oldY * sin(theta)), (oldY * cos(theta)) + (oldX * sin(theta))));
				siblings[i]->basePos = (length(siblings[i]->basePos - papa->basePos) * rotatedVec) + papa->basePos;
			}
		}

		if (papa->index != 0) {
			vec2 papaDirectionVector = papa->parent->basePos - papa->basePos;
			vec2 thisDirectionVector = room->basePos - papa->basePos;
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
		vec2 dirVector;
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

		if (-dirVector.y < 0.f) {
			room->downExpand += 0.001f;
			room->upExpand -= 0.001f;
		} else if (-dirVector.y > 0.f) {
			room->upExpand += 0.001f;
			room->downExpand -= 0.001f;
		}
	}
}

void FloorGraph::setRoomsFloors(bool is3D)
{
  for(Room* room : graph)
  {

    room->setRoomGeometry(is3D);
  /*  vec2 diagonal = room->downLeftPos - room->upRightPos;
    vertices.push_back(vec3(room->upRightPos+vec2(diagonal.x,0),-10.f));
    vertices.push_back(vec3(room->downLeftPos-vec2(diagonal.y,0), -10.f));*/
    /*room->vertices.clear();
    room->normals.clear();
    room->indices.clear();

    room->vertices.push_back(vec3(room->upRightPos.x, -10.f, room->upRight
    Pos.y));
    room->vertices.push_back(vec3(room->downLeftPos.x, -10.f, room->upRightPos.y));
    room->vertices.push_back(vec3(room->downLeftPos.x, -10.f, room->downLeftPos.y));
    room->vertices.push_back(vec3(room->upRightPos.x, -10.f, room->downLeftPos.y));

    for(uint i=0; i<3; i++)
      room->indices.push_back(i);

    for(uint i=0; i<3; i++)
      room->indices.push_back((i+2)%4);

    vec3 normal = -normalize(
      cross(room->vertices[1]-room->vertices[0],
            room->vertices[2]-room->vertices[1]));

    for(uint i=0; i<4; i++)
      room->normals.push_back(normal);*/
  }
}

void FloorGraph::getRoomsPos(vector<vec3> &input)
{
  for (Room* room : graph) {
    input.push_back(vec3(room->basePos.x, -10.f, room->basePos.y));
  }
}
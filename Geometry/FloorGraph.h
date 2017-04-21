#include <vector>
#include <queue>
#include <glm/glm.hpp>
#include "Room.h"

using namespace std;

class FloorGraph
{
public:
  vector<Room*> graph;
  vector<vec3> housePerimeter;
  vector<vec3> roofPoints;
  vector<vec3> windows;
  vec3 frontDoorRoom = vec3(0.f);
  vec3 frontDoorPerim = vec3(0.f);

  FloorGraph();
  void printGraphData();

  void addPublicRooms();
  void addOtherRooms( int probability, int randomness,
                      int type, float size, int numRooms,
                      vector<Room*> extantRooms);
  void concatenateRooms(vector <Room*> newRooms);

  void expandRooms();
  void setRoomsPos();
  void getRoomsPos(vector<vec3> &input);
  void getRoomsOutlines(vector<vec3> &vertices, vector<uint> &indices);
  void getEdges(vector<vec3> &vertices);
  void setDoors();
  void getDoors(vector<vec3> &vertices);
  void setPerimeter(vector<vec3> &perimeter,float offset);
  void getHousePerimeter(bool is3D, vector<vec3> &vertices, vector<vec3> &normals, vector<vec2> &uvs, vector<uint> &indices);
  void getGround(bool is3D, vector<vec3> &vertices, vector<vec3> &normals, vector<vec2> &uvs, vector<uint> &indices);
  void getCeiling(vector<vec3> &vertices);
  void setRoof(vector<vec3> &vertices, vector<vec3> &normals);
  void getRoofTop(vector<vec3> &vertices);
  void setWindows();
  void getWindows(vector<vec3> &vertices);
  void getFrontDoor(vector<vec3> &vertices);

  float findLowestPos();
  float findHighestPos();
  float findLeftmostPos(float height);
  float findRightmostPos(float height);
};

FloorGraph::FloorGraph()
{
	addPublicRooms();
	addOtherRooms(15, 60, 1, 5.5f, 2, graph);	//add Private Rooms
	addOtherRooms(5, 35, 2, 2.5f, 1, graph);	//add Extra Rooms

	Room* room = graph[0];

	room->basePos = vec3(0.f);
	room->upRightPos = room->basePos;
	room->downLeftPos = room->basePos;

	queue<int> queue;
	queue.push(room->index);

	while (queue.size() > 0) {
		room = graph[queue.front()];
		queue.pop();

		for (Room* neib : room->neighbours) {
			if (room->index < neib->index) {
				neib->basePos = vec3(float(rand() % 11 - 5), 0.f, float(rand() % 11 - 5));
				queue.push(neib->index);
			}
		}
	}
}

void FloorGraph::printGraphData()
{
	for (Room* room : graph) {
		cout << "Room " << room->index <<
				" of type " << room->type <<
				" and size " << room->size <<
				" has neighbours: ";

		for (Room* neib : room->neighbours) {
			cout << neib->index << ", ";
		}

		if (room->index == 0)
			cout << "and that's it." << endl;
		else
			cout << "and its parent is Room " << room->parent->index << "." << endl;
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
    if (graph.size() > 1)
      break;  //prevents a house having a million rooms

    int random = rand() % 25;
    if(random <= probability || count < 1)
      concatenateRooms(currentRoom->createRooms(0, 8.5f, graph.size(), numNewRooms));

    numNewRooms--;

    count++;
    currentRoom = graph[count];
  }
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
	Room* room = graph[0];

	queue<int> queue;
	for (Room* neib : room->neighbours) {
		queue.push(neib->index);
	}

	while (queue.size() > 0) {
		room = graph[queue.front()];
		queue.pop();

		vertices.push_back(room->parent->basePos + vec3(0.f, -0.01f, 0.f));
		vertices.push_back(room->basePos + vec3(0.f, -0.01f, -0.f));

		for (Room* neib : room->neighbours) {
			if (room->index < neib->index)
				queue.push(neib->index);
		}
	}
}

void FloorGraph::getRoomsOutlines(vector<vec3> &vertices, vector<uint> &indices)
{
  vertices.clear();
  indices.clear();
  uint count = 0;
  for (Room* room : graph)
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

void FloorGraph::getRoomsPos(vector<vec3> &vertices)
{
	vertices.clear();
	for (Room* room : graph) {
		vertices.push_back(room->basePos + vec3(0.f, -0.01f, 0.f));
	}
}

void FloorGraph::setDoors()
{
	Room* room;
	queue<int> queue;
	queue.push(0);
	while (!queue.empty()) {
		room = graph[queue.front()];
		queue.pop();

		room->doors.clear();

		for (Room* neib : room->neighbours) {
			room->setDoorPos(neib);
			if (room->index < neib->index)
				queue.push(neib->index);
		}
	}
}

void FloorGraph::getDoors(vector<vec3> &vertices)
{
	vertices.clear();

	Room* room;
	queue<int> queue;
	queue.push(0);

	while (!queue.empty()) {
		room = graph[queue.front()];
		queue.pop();

		for (vec3 doorPos : room->doors) {
			vertices.push_back(doorPos);
		}

		for (Room* neib : room->neighbours) {
			if (room->index < neib->index)
				queue.push(neib->index);
		}
	}
}

float FloorGraph::findLowestPos() {
	float retVal = 10.f;
	for (Room* room : graph) {
		if (room->downLeftPos.z < retVal)
			retVal = room->downLeftPos.z;
	}
	return retVal;
}

float FloorGraph::findHighestPos() {
	float retVal = -10.f;
	for (Room* room : graph) {
		if (room->upRightPos.z > retVal)
			retVal = room->upRightPos.z;
	}
	return retVal;
}

float FloorGraph::findLeftmostPos(float height) {
	float retVal = 10.f;
	for (Room* room : graph) {
		if (room->downLeftPos.z <= height && room->upRightPos.z >= height) {
			if (room->downLeftPos.x < retVal)
				retVal = room->downLeftPos.x;
		} else continue;
	}
	return retVal;
}

float FloorGraph::findRightmostPos(float height) {
	float retVal = -10.f;
	for (Room* room: graph) {
		if (room->downLeftPos.z <= height && room->upRightPos.z >= height) {
			if (room->upRightPos.x > retVal)
				retVal = room->upRightPos.x;
		} else continue;
	}
	return retVal;
}

void FloorGraph::setPerimeter(vector<vec3> &perimeter, float offset){
	perimeter.clear();

	float start = findLowestPos();
	float end = findHighestPos();
	float leftmost = findLeftmostPos(start);
	float rightmost = findRightmostPos(end);

	float newLeftmost = leftmost;
	float newRightmost = rightmost;
	float iOffset = -offset;

	for (float i = start; i <= end; i += 0.001f) {
		perimeter.push_back(vec3(leftmost - offset, -0.001f, i + iOffset));

		while (leftmost == newLeftmost) {
			i += 0.001f;
			newLeftmost = findLeftmostPos(i);
		}

		if (leftmost < newLeftmost) {
			iOffset = offset;
		} else {
			iOffset = -offset;
		}

		perimeter.push_back(vec3(leftmost - offset, -0.001f, i + iOffset));
		leftmost = newLeftmost;
	}

	for (float i = end; i >= start; i -= 0.001f) {
		perimeter.push_back(vec3(rightmost + offset, -0.001f, i + iOffset));

		while (rightmost == newRightmost) {
			i -= 0.001f;
			newRightmost = findRightmostPos(i);
		}

		if (rightmost < newRightmost) {
			iOffset = offset;
		} else {
			iOffset = -offset;
		}

		perimeter.push_back(vec3(rightmost + offset, -0.001f, i + iOffset));
		rightmost = newRightmost;
	}

	perimeter.push_back(perimeter[0]);
	perimeter.push_back(perimeter[1]);

}

void FloorGraph::getHousePerimeter(bool is3D, vector<vec3> &vertices, vector<vec3> &normals, vector<vec2> &uvs, vector<uint> &indices) {
	for (uint i = 0; i < housePerimeter.size(); i++) {
		if (is3D) {
			vector<vec3> verts, norms;
			vector<vec2> textCoords;
			vector<uint> indexes;

			vec3 offsetEnd = housePerimeter[(i+2)%housePerimeter.size()];
			vec3 offsetStart = housePerimeter[(i+1)%housePerimeter.size()];
			vec3 current = housePerimeter[(i)%housePerimeter.size()];

			vec3 offset =  offsetEnd - offsetStart;
			vec3 wallCorner = (offsetStart + normalize(offset) * 0.1f);

      Wall* w = new Wall(current, wallCorner, -1);
      for(uint j=0; j < w->indices.size(); j++)
      {
        indices.push_back(w->indices[j]+vertices.size());
      }

      vertices.insert( vertices.end(), w->vertices.begin(), w->vertices.end() );
      normals.insert( normals.end(), w->normals.begin(), w->normals.end() );
      uvs.insert(uvs.end(), w->uvs.begin(), w->uvs.end());

    } else {
      vertices.push_back(housePerimeter[i]);
    }
			/*createPrism(verts, norms, indexes, textCoords, current, wallCorner, -1);

			uint iOffset = vertices.size();
			for(uint i: indexes)
			  indices.push_back(i+iOffset);

			vertices.insert(vertices.end(), verts.begin(), verts.end());
			normals.insert(normals.end(), norms.begin(), norms.end());
			uvs.insert(uvs.end(), textCoords.begin(), textCoords.end());
		} else {
			vertices.push_back(housePerimeter[i]);
		}*/
	}
}

void FloorGraph::getCeiling(vector<vec3> &vertices) {
	vector<vec3> ceilingPerimeter;
	setPerimeter(ceilingPerimeter, 0.2f);

	uint count = 0;
	int index = -1;

	for (Room* room : graph) {
		if (count < room->neighbours.size()) {
			count = room->neighbours.size();
			index = room->index;
		}
	}

	vertices.push_back(graph[index]->basePos + vec3(0.f, -1.01f, 0.f));
	for (vec3 outlinePos : ceilingPerimeter) {
		vertices.push_back(outlinePos + vec3(0.f, -1.01f, 0.f));
	}
	vertices.push_back(ceilingPerimeter[0] + vec3(0.f, -1.01f, 0.f));
}

void FloorGraph::getGround(bool is3D, vector<vec3> &vertices, vector<vec3> &normals, vector<vec2> &uvs, vector<uint> &indices) {
	vertices.clear();
	normals.clear();
	indices.clear();
	uvs.clear();

	vec3 groundCorner1 = vec3(15.f, 0.1f, 15.f);
	vec3 groundCorner2 = vec3(-15.f, 0.1f, -15.f);
	createPrism(vertices, normals, indices, uvs, groundCorner1, groundCorner2, -0.09f);

	for(uint i=0; i<uvs.size(); i++)
		uvs[i] *=10;
}

void changeExpansion(Room* room1, Room* room2) {
	/* room1 is to the bottom left, room2 is to the top right */
	if ((room1->basePos.x <= room2->basePos.x) && (room1->basePos.z <= room2->basePos.z)) {
		if ((room1->upRightPos.x >= room2->downLeftPos.x) && (room1->upRightPos.z >= room2->downLeftPos.z)) {
			if (room1->upRightPos.x >= room2->downLeftPos.x + 0.05f) {
				room1->upExpand = 0.f;
				room2->downExpand = 0.f;
			}
			else if (room1->upRightPos.z >= room2->downLeftPos.z + 0.05f) {
				room1->rightExpand = 0.f;
				room2->leftExpand = 0.f;
			}
		}
	}

	/* room1 is to the top left, room2 is to the bottom right */
	else if ((room1->basePos.x <= room2->basePos.x) && (room1->basePos.z >= room2->basePos.z)) {
		if ((room1->upRightPos.x >= room2->downLeftPos.x) && (room1->downLeftPos.z <= room2->upRightPos.z)) {
			if (room1->upRightPos.x >= room2->downLeftPos.x + 0.05f) {
				room1->downExpand = 0.f;
				room2->upExpand = 0.f;
			}
			else if (room1->downLeftPos.z <= room2->upRightPos.z - 0.05f) {
				room1->rightExpand = 0.f;
				room2->leftExpand = 0.f;
			}
		}
	}
}

void FloorGraph::expandRooms()
{
	for (Room* room1 : graph) {
		room1->increaseArea();

		for (Room* room2 : graph) {
			if (room1 == room2) continue;

			if ((room1->type == 2) && (room2->type == 0)) {
				continue;
			} else if ((room2->type == 2) && (room1->type == 0)) {
				continue;
			}

			changeExpansion(room1, room2);
		}
	}
}

void spreadRooms(vector<Room*> &siblings, Room* room, Room* papa) {
	/* find all siblings to current room */
	for (Room* sibling : papa->neighbours) {
		if (papa->index < sibling->index) {
			siblings.push_back(sibling);
		}
	}

	/* make sure the first sibling is past 90 degrees from the incoming branch */
	if (papa->index != 0) {
		vec2 papaDirectionVector = vec2(papa->parent->basePos.x - papa->basePos.x ,
										papa->parent->basePos.z - papa->basePos.z );
		vec2 thisDirectionVector = vec2(siblings[0]->basePos.x - papa->basePos.x ,
										siblings[0]->basePos.z - papa->basePos.z );
		float numer = dot(papaDirectionVector, thisDirectionVector);
		float denom = length(papaDirectionVector) * length(thisDirectionVector);
		float angle = acos(numer / denom);

		if (angle < (M_PI/2.f)) {
			float phi = (3.f * M_PI / 2.f);
			float oldX = papaDirectionVector.x;
			float oldY = papaDirectionVector.y;
			vec3 rotatedVec = vec3((oldX * cos(phi)) - (oldY * sin(phi)), 0.f, (oldY * cos(phi)) + (oldX * sin(phi)));
			siblings[0]->basePos = length(siblings[0]->basePos) * glm::normalize(rotatedVec);
		}
	}

	/* fan the remaining siblings by equal degrees */
	float theta = -(M_PI) / siblings.size();
	if (papa->index == 0)
		theta = -(3.f * M_PI / 2.f) / siblings.size();

	for (int i = 1; i < int(siblings.size()); i++) {
		float oldX = siblings[i - 1]->basePos.x - papa->basePos.x;
		float oldZ = siblings[i - 1]->basePos.z - papa->basePos.z;
		vec3 rotatedVec = glm::normalize(vec3((oldX * cos(theta)) - (oldZ * sin(theta)), 0.f, (oldZ * cos(theta)) + (oldX * sin(theta))));
		siblings[i]->basePos = (length(siblings[i]->basePos - papa->basePos) * rotatedVec) + papa->basePos;
	}
}

void setRoomBasePos(Room* room, Room* papa) {
	// if room is not a child of papa:
	if (papa->index != 0) {
		vec2 papaDirectionVector = vec2(papa->parent->basePos.x - papa->basePos.x ,
										papa->parent->basePos.z - papa->basePos.z );
		vec2 thisDirectionVector = vec2(room->basePos.x - papa->basePos.x ,
										room->basePos.z - papa->basePos.z );
		float numer = dot(papaDirectionVector, thisDirectionVector);
		float denom = length(papaDirectionVector) * length(thisDirectionVector);
		float angle = acos(numer / denom) * (180.f / M_PI);
		if (angle < 90.f) {
			room->basePos = 2.2f * glm::normalize(-1.f * vec3(thisDirectionVector.x, 0.f, thisDirectionVector.y)) + papa->basePos;
		} else {
			room->basePos = 2.2f * glm::normalize(vec3(thisDirectionVector.x, 0.f, thisDirectionVector.y)) + papa->basePos;
		}
	} else {
		room->basePos = 2.2f * glm::normalize(room->basePos);
	}
}

void FloorGraph::setRoomsPos()
{
	Room* room = graph[0];

	queue<int> queue;
	for (Room* neib : room->neighbours)
		queue.push(neib->index);

	// initialize the parent of the first node as a blank room (type -1, size -1, index -1).
	Room* papa = new Room(-1, -1.f, -1);

	vector<Room*> siblings;

	while (queue.size() > 0) {
		/* queue operations */
		room = graph[queue.front()];
		queue.pop();
		for (Room* neib : room->neighbours) {
			if (room->index < neib->index)
				queue.push(neib->index);
		}

		// if there's a new papa:
		if (papa->index != room->parent->index) {
			siblings.clear();
			papa = room->parent;
			spreadRooms(siblings, room, papa);
		}

		/* find an appropriate basePos for room */
		setRoomBasePos(room, papa);

		/* changes the distance of a room to its parent based on its size */
		// if (room->type == 2)
		// 	room->basePos = ((float(room->size) / float(papa->size)) * (room->basePos - papa->basePos)) + papa->basePos;
	}
}

void findSkeletonEdges(vector<vec3> walls, vector<vec3> &skeletonEdges) {
	for (uint i = 0; i < walls.size(); i++) {
		vec3 dirA = normalize(walls[(i + 1) % walls.size()]);
		vec3 dirB = normalize(-walls[i]);
		vec3 dirEdge = normalize(walls[(i + 1) % walls.size()] - walls[i]);

		float dotProd = dot(dirA, dirEdge);
		float det = dirA.x * dirEdge.z - dirA.z * dirEdge.x;
		float angle1 = atan2(det, dotProd) * (180.f / M_PI);

		dotProd = dot(dirEdge, dirB);
		det = dirB.z * dirEdge.x - dirB.x * dirEdge.z;
		float angle2 = atan2(det, dotProd) * (180.f / M_PI);

		if (angle1 > 90.f || angle2 > 90.f) {
			dirEdge = normalize(-1.f * dirEdge);
		}
		skeletonEdges.push_back(normalize(dirEdge));
	}
}

void findIntersections(vector<vec3>& intersections, vector<vec3>& roofPerimeter, vector<vec3>& skeletonEdges)
{
	vec3 intsectPoint = vec3(0.f);

	intersectingPoint(	roofPerimeter[0], skeletonEdges[0],
						roofPerimeter[roofPerimeter.size() - 3], skeletonEdges.back(), intsectPoint);

	if (intsectPoint != vec3(0.f) && length(intsectPoint - roofPerimeter[0]) > 1.f)
		intersections.push_back(intsectPoint);

	for (uint i = 1; i < skeletonEdges.size(); i++) {
		intsectPoint = vec3(0.f);
		intersectingPoint(	roofPerimeter[i-1], skeletonEdges[i-1],
							roofPerimeter[i], skeletonEdges[i], intsectPoint);

		if (intsectPoint != vec3(0.f) && length(intsectPoint - roofPerimeter[i]) > 1.f)
			intersections.push_back(intsectPoint);
	}

	for (uint i = 0; i < intersections.size() - 1; i++) {
		if (length(intersections[i + 1] - intersections[i]) < 2.5f)
			intersections.erase(intersections.begin() + i);
	}
}

void FloorGraph::setRoof(vector<vec3> &vertices, vector<vec3> &normals) {
	vector<vec3> roofPerimeter;
	setPerimeter(roofPerimeter, 0.2f);

	vector<vec3> walls;
	walls.push_back(normalize(roofPerimeter[0] - roofPerimeter[roofPerimeter.size() - 3]));
	for (uint i = 1; i < roofPerimeter.size() - 2; i++) {
		walls.push_back(normalize(roofPerimeter[i] - roofPerimeter[i-1]));
	}

	vector<vec3> skeletonEdges;
	findSkeletonEdges(walls, skeletonEdges);

	vector<vec3> intersections;
	findIntersections(intersections, roofPerimeter, skeletonEdges);

	/* here onwards sets the roofPoints and the vertices to return */
	roofPoints.clear();
	roofPoints.push_back(vec3(0.f, -2.f, 0.f));

	float distance;
	vec3 wallPos, p1, p2, p3, normal;
	uint save = 0;
	for (uint i = 0; i < roofPerimeter.size() - 2; i++) {
		wallPos = (roofPerimeter[i+1] + roofPerimeter[i]) / 2.f;
		distance = length(wallPos - intersections[save]);
		for (uint j = 0; j < intersections.size(); j++) {
			if (length(wallPos - intersections[j]) < distance) {
				if (save != j) {
					roofPoints.push_back(intersections[save] + vec3(0, -2.f, 0));
					roofPoints.push_back(roofPerimeter[i] + vec3(0, -1.01f, 0));
					save = j;
				}
				distance = length(wallPos - intersections[j]);
			}
		}

		p1 = intersections[save] + vec3(0, -2, 0);
		p2 = roofPerimeter[i] + vec3(0, -1.01f, 0);
		p3 = roofPerimeter[i + 1] + vec3(0, -1.01f, 0);

		normal = normalize(cross(p3 - p1, p2 - p1));

		//if (normal.y > 0.f) {
			normals.push_back(normal);
			vertices.push_back(p1);
			vertices.push_back(p2);
			vertices.push_back(p3);
		//} else roofPoints.push_back(p2);
	}

	roofPoints.push_back(intersections[0] + vec3(0, -2.f, 0));
	roofPoints.push_back(roofPoints[1]);
}

void FloorGraph::getRoofTop(vector<vec3> &vertices) {
	for (vec3 roofPoint : roofPoints) {
		vertices.push_back(roofPoint);
	}
}

void FloorGraph::setWindows() {
	windows.clear();
	frontDoorRoom = vec3(0.f);
	frontDoorPerim = vec3(0.f);

	vec3 wall;
	for (uint i = 0; i < housePerimeter.size() - 2; i++) {
		wall = housePerimeter[i + 1] - housePerimeter[i];
		int maxWindows = 0;

		if (length(wall) < 0.5f)
			continue;
		else if (length(wall) < 1.2f)
			maxWindows = 1;
		else if (length(wall) < 2.f)
			maxWindows = 2;
		else maxWindows = 3;

		int numWindows = rand() % maxWindows + 1;

		vec3 windowDisp;
		vec3 returnVec;
		for (int j = 0; j < numWindows; j++) {
			if (wall.x == 0) {
				windowDisp = vec3(0.f, 0.f, ((wall.z / float(numWindows)) * (j + 0.5f)));
				returnVec = housePerimeter[i] + windowDisp;
				windows.push_back(returnVec);
			}
			else if (wall.z < 0.01) {
				windowDisp = vec3(((wall.x / float(numWindows)) * (j + 0.5f)), 0.f, 0.f);
				returnVec = housePerimeter[i] + windowDisp;
				windows.push_back(returnVec);
			}
		}
	}

	for (Room* room : graph) {
		room->findWindowsInRoom(windows, frontDoorRoom, frontDoorPerim);
	}
}

void FloorGraph::getWindows(vector<vec3> &vertices) {
	for (vec3 window : windows) {
		vertices.push_back(window);
	}

	for (Room* room : graph) {
		for (vec3 roomWindow : room->roomWindows) {
			vertices.push_back(roomWindow);
		}
	}
}

void FloorGraph::getFrontDoor(vector<vec3> &vertices) {
	vertices.push_back(frontDoorRoom);
	vertices.push_back(frontDoorPerim);
}

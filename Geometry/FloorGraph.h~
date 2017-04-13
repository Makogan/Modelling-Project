#include <vector>
#include <glm/glm.hpp>
#include "Room.h"

using namespace std;

class FloorGraph
{
public:
  vector<Room*> graph;

  FloorGraph(){}

  void addPublicRooms();
  void addOtherRooms( int probability, int randomness,
                      int type, float size, int numRooms,
                      vector<Room*> extantRooms);
  void concatenateRooms(vector <Room*> newRooms);
};

void FloorGraph::addPublicRooms() {
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
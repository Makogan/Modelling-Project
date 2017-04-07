#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

class Node
{
public:
  vec3 position;
  float size;
  uint index;
  vector<uint> neighbours;

  Node();
  Node(uint i);
  Node(uint i, vec3 v);
  Node(uint i, vec3 v, float s);

  vector<vec3> getNodeCircle();
};

Node::Node():Node(0){}

Node::Node(uint i):Node(i,vec3(0)){}

Node::Node(uint i, vec3 v):Node(i,v,3){}

Node::Node(uint i, vec3 v, float s)
{
  position = v;
  index = i;
  size = s;
}

#define SUBDIVISIONS 500
vector<vec3> Node::getNodeCircle()
{
  float r = size;
  vector<vec3> circle;
  for(uint u=0; u<500; u++)
  {
    circle.push_back(position +
      vec3(r*cos(2*M_PI*u/(SUBDIVISIONS-1)),0,r*sin(2*M_PI*u/(SUBDIVISIONS-1))));
  }

  return circle;
}

class Graph
{
public:
  vector<Node*> nodes;

  Graph();
  Graph(Node *node);
  Graph(Graph *oGraph);

  void connect(uint i, uint j);
  void addNode(Node *node);
  void balanceNodes();
  vector<uint> getEdges();
};

Graph::Graph(){}

Graph::Graph(Node *node)
{
  node->index = 0;
  nodes.push_back(node);
}

void Graph::connect(uint i, uint j)
{
  if(i>nodes.size()||j>nodes.size())
    cerr << "Attempt to connect unexistent node.\n"
      << "values where: (" << i <<", " << j << ")"
      << "Current node number: " << nodes.size() << endl;
  else
  {
    nodes[i]->neighbours.push_back(j);
    nodes[j]->neighbours.push_back(i);
  }
}

void Graph::addNode(Node *node)
{
  node->index = nodes.size();
  nodes.push_back(node);
}

void Graph::balanceNodes()
{
  vector<Node*> overlapingNodes;
  for(Node *n1: nodes)
  {
    vec3 nextPos = vec3(0);
    for(Node *n2: nodes)
    {
      vec3 p1 = n1->position;
      vec3 p2 = n2->position;
      float l = length(p1-p2);
      if(l < n1->size+n2->size && n1!=n2)
      {
        if(l>0)
          nextPos += normalize(p1-p2)*(n1->size+n2->size-l);
        else
          nextPos += vec3(0,0,1)*(n1->size+n2->size);
      }
      n1->position+=nextPos;
    }
  }
}

vector<uint> Graph::getEdges()
{
  vector<uint> edges;
  vector<uint> nodesToVisit;
  vector<bool> visitedFlag(nodes.size());
  nodesToVisit.push_back(0);
  visitedFlag[0]=true;
  while(nodesToVisit.size()>0)
  {
    Node *currentNode = nodes[nodesToVisit[nodesToVisit.size()-1]];
    nodesToVisit.pop_back();

    for(uint i: currentNode->neighbours)
    {
      if((visitedFlag[i]!=true))
      {
        edges.push_back(i);
        edges.push_back(currentNode->index);

        nodesToVisit.push_back(i);
      }
    }
  }
  return edges;
}

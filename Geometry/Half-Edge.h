/*
  Still in testing, multiple known edge cases that break things!!!!
*/
class Vertex;
class HalfEdge;
class Face;
class HE_Object;

class Vertex
{
public:
  HalfEdge *halfEdge = NULL;
  vec3 position;
  vec3 normal;

  HE_Object *myObject;

  Vertex(vec3 p, HE_Object *obj);
  Vertex(HalfEdge *h, vec3 p, HE_Object *obj);

  static void connect(Vertex *v1, Vertex *v2);
};

class Face
{
public:
  HalfEdge *halfEdge = NULL;
  vec3 normal;

  HE_Object *myObject;

  Face(HalfEdge *h, vec3 n, HE_Object *he);
};

class HalfEdge
{
public:
  Face *face = NULL;
  Vertex *source = NULL;
  HalfEdge *next = NULL;
  HalfEdge *pair = NULL;

  HE_Object *myObject;

  HalfEdge(Vertex *s, HE_Object *obj);
  HalfEdge(Face *f, Vertex *s, HalfEdge *n, HalfEdge *p, HE_Object *obj);
};

class HE_Object
{
public:
  vector<Vertex*> vertices;
  vector<Face*> faces;
  vector<HalfEdge*> edges;
  vector<uint> indices;

  HE_Object(vector<vec3> vertices, vector<uint> indices);
  HE_Object(vector<vec3> vertices);
  HE_Object();

  HalfEdge* findEdge(Vertex *v1, Vertex *v2);

  vector<Vertex*> findVertexNeighbours(Vertex* vert);
  vector<HalfEdge*> findCyle(HalfEdge *e);

  void getGeometry(vector<vec3> &verts, vector<uint> &indices);
  void merge(HalfEdge *e1, HalfEdge *e2);
  void connect(Vertex *v1, Vertex *v2);
  void createTriFrame(Vertex *v1, Vertex *v2, Vertex *v3);
};

void HE_Object::createTriFrame(Vertex *v1, Vertex *v2, Vertex *v3)
{
  /*HalfEdge *edge1 = findEdge(v1, v2);
  HalfEdge *edge2 = findEdge(v2, v3);
  HalfEdge *edge3 = findEdge(v3, v1);*/


 /* vector<HalfEdge*> faceEdges;

  faceEdges.push_back(new HalfEdge(v1, this));
  faceEdges.push_back(new HalfEdge(v2, this));
  faceEdges.push_back(new HalfEdge(v3, this));

  vector<HalfEdge*> offFaceEdges;
  offFaceEdges.push_back(new HalfEdge(v3, this));
  offFaceEdges.push_back(new HalfEdge(v2, this));
  offFaceEdges.push_back(new HalfEdge(v1, this));

  for(uint i=0; i<3; i++)
    faceEdges[i]->next = faceEdges[(i+1)%3];

  for(uint i=0; i<3; i++)
    offFaceEdges[i]->next = offFaceEdges[(i+1)%3];

  for(uint i=0; i<3; i++)
    faceEdges[i]->pair = offFaceEdges[(i+1)%3];

  for(uint i=0; i<3; i++)
  {
    this->edges.push_back(faceEdges[i]);
    this->edges.push_back(offFaceEdges[i]);
  }

  vec3 normal = normalize(cross(faceEdges[0]->source->position - faceEdges[1]->source->position, 
    faceEdges[1]->source->position - faceEdges[2]->source->position));
  Face *face = new Face(faceEdges[0], normal, this);

  faces.push_back(face);*/
}

void HE_Object::connect(Vertex *v1, Vertex *v2)
{
  HalfEdge* edge = findEdge(v1, v2);

  if(edge == NULL)
  {
    HalfEdge *halfE1 = new HalfEdge(v1, v1->myObject);
    HalfEdge *halfE2 = new HalfEdge(v2, v2->myObject);
    halfE1->pair=halfE2;
    halfE2->pair=halfE1;

    halfE1->myObject = v1->myObject;
    halfE2->myObject =v2->myObject;

    v1->halfEdge = halfE1;

    v2->halfEdge = halfE2;
  }
}

void HE_Object::merge(HalfEdge *e1, HalfEdge *e2)
{
  e2->pair->source->halfEdge = e1;
  e2->pair = e1;
  e2->source = e1->pair->source;

  e1->pair->source->halfEdge = e2;
  e1->pair = e2;
}

void HE_Object::getGeometry(vector<vec3> &verts, vector<uint> &indices)
{
  indices = this->indices;
  verts.clear();
  for(Vertex *v:vertices)
  {
    verts.push_back(v->position);
  }
}

//returns the half edge starting at v1 and ending at v2
//returns NULL if no such edge could be found
HalfEdge* HE_Object::findEdge(Vertex *v1, Vertex *v2)
{
  HalfEdge *original = v1->halfEdge;

  //if v1 has no outgoing edges, then it trivially has no edge
  //going from it to v2
  if(original==NULL)
    return NULL;

  HalfEdge *current = original;
  do
  {
    Vertex* v = current->next->source;
    if(v==v2)
      return current;
    current = current->pair->next;
  }
  while(current!=original);

  return current;
}

//returns a vector containing all the half edges contained in the
//same face as e, returns 0 otherwise. This means that if there is
//no cycle starting at e, it returns an empty vector 
vector<HalfEdge*> HE_Object::findCyle(HalfEdge *e)
{
  HalfEdge *original = e;
  HalfEdge *current = e;

  vector<HalfEdge*> cycle;
  do
  {
    cycle.push_back(current);
    if(current==NULL)
      return vector<HalfEdge*>();

    current = current->next;
  }
  while(current !=original);

  return cycle;
}

//find all the vertices that share an edge with vertex vert
//returns an empty vector if no such vertices exist
//only works for triangular meshes and only for 2d manifolds
//If errors happen suspect this!
vector<Vertex*> HE_Object::findVertexNeighbours(Vertex *vert)
{
  HalfEdge *original = vert->halfEdge;

  //if there is no edge comming out of vertex vert, trivially,there are no
  //vertices sharing an edge with vert
  if(original==NULL)
    return vector<Vertex*>();

  HalfEdge *current = original;
  vector<Vertex*> neighbours;
  //we iterate through the edges counter-clockwise
  do
  {
    Vertex* v = current->next->source;
    neighbours.push_back(v);
    current = current->pair->next;
  }
  while(current!=original);

  return neighbours;
}

HE_Object::HE_Object(vector<vec3> verts)
{
  vector<uint> indexes;
  for(uint i=0; i<verts.size(); i++)
  {
    indexes.push_back(i);
  }

  HE_Object *temp = new HE_Object(verts, indices);
  this->vertices = temp->vertices;
  this->faces = temp->faces;
  this->edges = temp->edges;
  this->indices = indexes;
}

HE_Object::HE_Object(vector<vec3> verts, vector<uint> indices)
{
  //Ensure this HE_Object is empty
  faces.clear();
  edges.clear();
  vertices =  vector<Vertex*>(verts.size(), NULL);
  this->indices = indices;

  //Assuming every 3 indices specify a triangle
  for(uint i=0; i<indices.size(); i+=3)
  {
    for(uint j=0; j<3; j++)
    {
      if(vertices[indices[j+i]]==NULL)
      {
        vertices[indices[j+i]] = new Vertex(verts[indices[j+i]], this);
      }
    }

    Vertex *local[3];
    local[0]= vertices[indices[i]];
    local[1]= vertices[indices[i+1]];
    local[2]= vertices[indices[i+2]];

    for(uint i=0; i<3; i++)
    {
      //HalfEdge *e = findEdge(local[i], local[(i+1)%3]);
      //if(e==NULL)
    }
    

  }

    
}

HE_Object::HE_Object(){}






Vertex::Vertex(vec3 p, HE_Object *obj)
{
  position = p;
  myObject = obj;
}

Vertex::Vertex(HalfEdge *h, vec3 p, HE_Object *obj)
{
  halfEdge = h;
  position = p;
  myObject = obj;
}








HalfEdge::HalfEdge(Vertex *s, HE_Object *obj)
{
  source = s;
  myObject = obj;
}

HalfEdge::HalfEdge(Face *f, Vertex *s, HalfEdge *n, HalfEdge *p, HE_Object *obj)
{
  face = f;
  source = s;
  next = n;
  pair = p;
  myObject = obj;
}





Face::Face(HalfEdge *h, vec3 n, HE_Object *he)
{
  halfEdge = h;
  normal = n;
  myObject = he;
}

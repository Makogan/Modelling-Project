class Vertx;
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
  HalfEdge *halfEdge;
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

  HE_Object(vector<vec3> vertices, vector<uint> indices);
  HE_Object(vector<vec3> vertices);

  HalfEdge* findEdge(Vertex *v1, Vertex *v2);

  vector<Vertex*> findVertexNeighbours(Vertex* vert);
  vector<HalfEdge*> findCyle(HalfEdge *e);
};

HalfEdge* HE_Object::findEdge(Vertex *v1, Vertex *v2)
{
  HalfEdge *original = v1->halfEdge;

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

vector<Vertex*> HE_Object::findVertexNeighbours(Vertex *vert)
{
  HalfEdge *original = vert->halfEdge;

  if(original==NULL)
    return vector<Vertex*>();

  HalfEdge *current = original;
  vector<Vertex*> neighbours;
  do
  {
    Vertex* v = current->next->source;
    neighbours.push_back(v);
    current = current->pair->next;
  }
  while(current!=original);

  return neighbours;
}

HE_Object::HE_Object(vector<vec3> verts, vector<uint> indices)
{
  //Ensure this HE_Object is empty
  vertices.clear();
  faces.clear();
  edges.clear();

  //create all teh vertices of the object
  for(vec3 v: verts)
  {
    vertices.push_back(new Vertex(v, this));
  }

  //Assuming every 3 indices specify a triangle
  for(uint i=0; i<indices.size(); i+=3)
  {
    //get the 3 vertices that make up teh triangle
    Vertex *local[3];
    local[0]= vertices[indices[i]];
    local[1]= vertices[indices[i+1]];
    local[2]= vertices[indices[i+2]];

    //Create any missing edges
    for(uint j=0; j<3; j++)
    {
      Vertex *vert = local[j];
      //get all the vertices that share an edge with the current vertex
      vector<Vertex*> neighbours = findVertexNeighbours(vert);
      bool edgeExists = false;
      for(Vertex *neighour: neighbours)
      {
        //if the next point in the triangle shares an edge with teh current
        //vertex the edge exists, so we mark it
        if(neighour == local[(j+1)%3])
        {
          edgeExists = true;
          break;
        }
      }

      //if we failed to find an edge between the current vertex in teh triangle
      //and the next vertex in teh triangle, we create the edge
      if(!edgeExists)
      {
        Vertex::connect(local[j], local[(j+1)%3]);
      }
    }

    //get one of the edges in the triangle
    HalfEdge *edge = findEdge(local[0], local[1]);
    if(edge == NULL)
      cerr << "Camilo Fucked up!" << endl;

    //find all the half edges that are contained in the current triangle
    vector<HalfEdge*> cycle = findCyle(edge);

    //get the counter-clockwise normal of the triangle
    vec3 normal = normalize(cross(  local[1]->position - local[0]->position,
                                    local[2]->position - local[1]->position));

    //create a face for the triangle and update the half edges                                
    Face *face = new Face(edge, normal, this);

    for(HalfEdge *edge: cycle)
      edge->face = face;

  }
}






void Vertex::connect(Vertex *v1, Vertex *v2)
{
  HalfEdge *halfE1 = new HalfEdge(v1, v1->myObject);
  HalfEdge *halfE2 = new HalfEdge(v2, v2->myObject);
  halfE1->pair=halfE2;
  halfE2->pair=halfE1;

  halfE1->myObject = v1->myObject;
  halfE2->myObject =v2->myObject;

  if(v1->halfEdge==NULL)
    v1->halfEdge = halfE1;

  if(v2->halfEdge==NULL)
    v2->halfEdge = halfE2;
}

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

//#include "Half-Edge.h"

class Room;
class Wall;

class Wall
{
public:
	vector<vec3> vertices;
	vector<uint> indices;

	Wall(vec3 corner1, vec3 corner2);

	void getGeometry(vector<vec3> &verts, vector<uint> &indices);
	void move(vec3 offset);
	void rotate(float angle, vec3 normal);
	void scale(float scale);
};

class Room
{
public:
	int index;
	vec3 offset = vec3(0);

	vector<Room> neighbours;
	vector<Wall*> walls;

	Room();
	Room(vec3 position);

	void getGeometry(vector<vec3> &verts, vector<uint> &indices);
	void move(vec3 movement);
	void rotate(float angle);
	void scale(float scale);
};

void Room::scale(float scale)
{
	for(Wall *w:walls)
		w->move(-offset);

	for(Wall *w:walls)
		w->scale(scale);

	for(Wall *w:walls)
		w->move(offset);
}

void Room::rotate(float angle)
{
	for(Wall *w:walls)
		w->move(-offset);

	for(Wall *w:walls)
		w->rotate(angle, vec3(0,0,1));

	for(Wall *w:walls)
		w->move(offset);
}

void Room::move(vec3 movement)
{
	offset += movement;

	for(Wall *w:walls)
		w->move(movement);
}

Room::Room():Room(vec3(0)){}

Room::Room(vec3 position)
{
	offset = position;
	//floor
	walls.push_back(new Wall(vec3(-15,-15,0)+position, vec3(15,15,-1)+position));
	//left wall
	walls.push_back(new Wall(vec3(-15,-15,0)+position, vec3(-13,15,15)+position));
	//right wall
	walls.push_back(new Wall(vec3(15,-15,0)+position, vec3(13,15,15)+position));	
	//back wall
	walls.push_back(new Wall(vec3(15,15,0)+position, vec3(-15,15,15)+position));
}

void Room::getGeometry(vector<vec3> &verts, vector<uint> &indices)
{
	verts.clear();
	indices.clear();
	vector<vec3> tvertices;
	vector<uint> tindices;

	for(auto& wall:walls)
	{
		wall->getGeometry(tvertices, tindices);
		for(uint i=0; i<tindices.size(); i++)
		{
			tindices[i]+=verts.size();
		}

		verts.insert( verts.end(), tvertices.begin(), tvertices.end());
		indices.insert( indices.end(), tindices.begin(), tindices.end() );
	}
}

Wall::Wall(vec3 corner1, vec3 corner2)
{
	vertices.push_back(corner1);
	vertices.push_back(corner2);

	for(uint i=0; i<3; i++)
	{
		vec3 side = vec3(0);
		side[i] = vec3(corner2-corner1)[i];

		vertices.push_back(corner1+side);
		vertices.push_back(corner2-side);
	}

	//BAD CODING RIGHT HERE VVVVVVVV
	indices = 	{	
					0,2,4,	2,7,4, 
					6,0,4,	4,3,6, 
					0,6,2,	2,6,5, 
					6,3,1,	1,5,6,
					5,7,2,	5,1,7,
					1,7,4,	4,3,1
			 	};

}

void Wall::rotate(float angle, vec3 normal)
{
	normal = normalize(normal);

	for(uint i=0; i<vertices.size(); i++)
	{
		vec4 rotVec = vec4(vertices[i], 1);
		rotVec = glm::rotate(angle, normal)*rotVec;
		vertices[i] = vec3(rotVec);
	}
}

void Wall::move(vec3 offset)
{
	for(uint i=0; i<vertices.size(); i++)
	{
		vertices[i]+=offset;
	}
}

void Wall::scale(float scale)
{
	for(uint i=0; i<vertices.size(); i++)
	{
		vertices[i]*=scale;
	}
}

void Wall::getGeometry(vector<vec3> &verts, vector<uint> &indices)
{
	verts = this->vertices;
	indices = this->indices;
}
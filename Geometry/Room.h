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
};

class Room
{
public:
	int index;
	vector<Room> neighbours;

	vector<Wall*> walls;

	Room();

	void getGeometry(vector<vec3> &verts, vector<uint> &indices);
};

Room::Room()
{
	//floor
	walls.push_back(new Wall(vec3(-15,-15,0), vec3(15,15,-1)));
	//left wall
	walls.push_back(new Wall(vec3(-15,-15,0), vec3(-13,15,15)));
	//right wall
	walls.push_back(new Wall(vec3(15,-15,0), vec3(13,15,15)));	
	//back wall
	walls.push_back(new Wall(vec3(15,15,0), vec3(-15,15,15)));

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

void Wall::getGeometry(vector<vec3> &verts, vector<uint> &indices)
{
	verts = this->vertices;
	indices = this->indices;
}
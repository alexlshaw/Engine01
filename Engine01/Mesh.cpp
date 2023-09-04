#include "Mesh.h"

Mesh::Mesh() : vbo(-1), vao(-1), ibo(-1), indexCount(0) {}

Mesh::Mesh(std::vector<SimpleVertex> vertices, std::vector<unsigned int> indices)
{
	indexCount = indices.size();
	//push the mesh
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SimpleVertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	setSimpleVertexAttribs();
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Mesh::Mesh(std::vector<ColouredVertex> vertices, std::vector<unsigned int> indices)
{
	indexCount = indices.size();
	//push the mesh
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ColouredVertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	setColouredVertexAttribs();
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}

void Mesh::draw()
{
	//bind the mesh and draw it
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void*)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//Loads a mesh from a wavefront obj file. Only supports basic mesh functionality (no parameter space vertices, no materials, no line elements, no object groups, triangle polygons only)
//Also currently only supports attribs matching the "Vertex" struct: position, normal, and tex coords
void Mesh::LoadFromObj(std::string fileName)
{
	//Open the file
	std::ifstream input(fileName);
	if (!input.is_open())
	{
		DEBUG_PRINT("Failed to open mesh file: ");
		DEBUG_PRINT(fileName.c_str());
		DEBUG_PRINT("\n");
		return;	//If we can't open the file, we're done here
	}

	//initialise mesh
	std::vector<ColouredVertex> vertices;
	std::vector<glm::vec4> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned int> indices;
	unsigned int vertexPositionCount = 0, vertexUVCount = 0, vertexNormalCount = 0, indicesCount = 0;
	countObj(input, vertexPositionCount, vertexUVCount, vertexNormalCount, indicesCount);	//Might be worth comparing the performance of going through the file twice vs not pre-allocating vectors
	vertices.reserve(indicesCount);	//See further down - indices are being treated as representing unique vertices
	positions.reserve(vertexPositionCount);
	uvs.reserve(vertexUVCount);
	normals.reserve(vertexNormalCount);
	indices.reserve(indicesCount);
	//reset stream position
	input.clear();
	input.seekg(0);
	
	//read data
	char buffer[256];
	int err = 0;
	while (!input.eof())
	{
		input.getline(buffer, 256);
		switch (buffer[0])
		{
		case 'v':
			switch (buffer[1])
			{
			case ' ':
			{
				//dealing with a new vertex specification
				float x = 0.0f, y = 0.0f, z = 0.0f;
				err = sscanf(buffer, "v %f %f %f", &x, &y, &z);
				positions.push_back(glm::vec4(x, y, z, 1.0f));
				break;
			}
			case 't':
			{
				//dealing with a texcoord specification
				float u = 0.0f, v = 0.0f;
				err = sscanf(buffer, "vt %f %f", &u, &v);
				uvs.push_back(glm::vec2(u, v));
				break;
			}
			case 'n':
			{
				//dealing with a normal specification
				float x = 0.0f, y = 0.0f, z = 0.0f;
				err = sscanf(buffer, "vn %f %f %f", &x, &y, &z);
				normals.push_back(glm::vec3(x, y, z));
				break;
			}
			default:;
			}
			break;
		case 'f':
		{
			//we're dealing with a face specification, create the actual vertex objects and set up indices to reference them
			//for any non-curved surface, each combination of position/uv/normal should be unique
			//Even for curved surfaces, treating them as unique should be functional, if not the most memory-efficient 

			//We're assuming no negative (wrap-around) indices
			int pA = -1, pB = -1, pC = -1;	//position holders
			int tA = -1, tB = -1, tC = -1;	//uv holders
			int nA = -1, nB = -1, nC = -1;	//normal holders
			err = sscanf(buffer, "f %d/%d/%d %d/%d/%d %d/%d/%d", &pA, &tA, &nA, &pB, &tB, &nB, &pC, &tC, &nC);

			
			ColouredVertex v1, v2, v3;
			v1.position = positions[pA - 1];
			v2.position = positions[pB - 1];
			v3.position = positions[pC - 1];
			v1.normal = normals[nA - 1];
			v2.normal = normals[nB - 1];
			v3.normal = normals[nC - 1];
			v1.texCoords = uvs[tA - 1];
			v2.texCoords = uvs[tB - 1];
			v3.texCoords = uvs[tC - 1];
			v1.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			v2.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			v3.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

			vertices.push_back(v1);
			indices.push_back(vertices.size() - 1);
			vertices.push_back(v2);
			indices.push_back(vertices.size() - 1);
			vertices.push_back(v3);
			indices.push_back(vertices.size() - 1);
			break;
		}
		default:;
		}
	}

	//wrap up
	input.close();

	indexCount = indices.size();
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ColouredVertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	setColouredVertexAttribs();
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//Reads through the obj file represented by input, and passes back how many vertices and indices should be reserved for it
void Mesh::countObj(std::ifstream& input, unsigned int& vertexPositionCount, unsigned int& vertexUVCount, unsigned int& vertexNormalCount, unsigned int& indexCount)
{
	vertexPositionCount = 0;
	vertexUVCount = 0;
	vertexNormalCount = 0;
	indexCount = 0;
	char buffer[256];
	while (!input.eof())
	{
		input.getline(buffer, 256);
		switch (buffer[0])
		{
		case 'v':
			switch (buffer[1])
			{
			case ' ':
				vertexPositionCount++;
				break;
			case 't':
				vertexUVCount++;
				break;
			case 'n':
				vertexNormalCount++;
				break;
			default:;
			}
			break;
		case 'f':
			indexCount += 3;	//At the moment we're only allowing triangluar faces
			break;
		default:;
		}
	}
}
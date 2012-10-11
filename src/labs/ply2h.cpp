//Convert PLY format from stanford to simple header file

#include <string>
#include <algorithm>
#include <vector>
#include <cctype>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/core/func_geometric.hpp>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    unsigned face_count;
};

struct Face
{
    unsigned vertices[3];
    glm::vec3 normal;
    float area;
};

template<typename T>
inline bool isnan(T value)
{
    return value != value;
}

bool load(std::istream & in, std::vector<Vertex> & vertices, std::vector<Face> & faces)
{
    std::string line;
    //read header
	while(getline(in, line))
	{
		std::string a, b;
		unsigned count;
		std::istringstream ls(line);
		ls >> a >> b >> count;
		if(a == "element")
		{
			if(b == "vertex")
			{
				vertices.resize(count);
			}
			else if(b == "face")
			{
				faces.resize(count);
			}
		}
		else if(a == "end_header")
			break;
	}
	if((vertices.size() == 0) || (faces.size() == 0))
    {
        std::cerr << "Can't parse header\n";
		return false;
    }
	//read vertices
	for(unsigned i = 0; i < vertices.size(); ++i)
	{
		if(!getline(in, line))
        {
            std::cerr << "Can't read vertex\n";
			return false;
        }
		std::istringstream ls(line);
        float x, y, z, nx, ny, nz;
		if(!(ls >> x >> y >> z >> nx >> ny >> nz))
        {
            std::cerr << "Can't parse vertex\n";
			return false;
        }
        //store vertex
        vertices[i].position = glm::vec3(x, y, z);
        vertices[i].normal = glm::vec3(nx, ny, nz);
        vertices[i].face_count = 0;
	}
	//read faces
	for(unsigned i = 0; i < faces.size(); ++i)
	{
		if(!getline(in, line))
        {
            std::cerr << "Can't read face\n";
			return false;
        }
		std::istringstream ls(line);
		unsigned cnt, a, b, c;
		if(!(ls >> cnt >> a >> b >> c))
        {
            std::cerr << "Can't parse face\n";
			return false;
        }
        if(cnt != 3)
        {
            std::cerr << "Faces must be triangles\n";
			return false;
        }
        //store face
        faces[i].vertices[0] = a;
        faces[i].vertices[1] = b;
        faces[i].vertices[2] = c;
        //mark vertices
        ++vertices[a].face_count;
        ++vertices[b].face_count;
        ++vertices[c].face_count;
        //edge vectors
/*        glm::vec3 ab = vertices[b].position-vertices[a].position;
        glm::vec3 ac = vertices[c].position-vertices[a].position;
        //face normal, scaled by area(*2)
        glm::vec3 n = glm::cross(ab, ac);*/
        //add face normal to vertex normal
        for(unsigned j = 0; j < 3; ++j)
        {
//            vertices[faces[i].vertices[j]].normal += n;
            //check for out-of-bounds indices
            if(faces[i].vertices[j] >= vertices.size())
            {
                std::cerr << "Face out of bounds\n";
                return false;
            }
        }
	}
    unsigned unused_vertices = 0;
    //normalize vertex normals
    for(unsigned i = 0; i < vertices.size(); ++i)
    {
/*        vertices[i].normal = glm::normalize(vertices[i].normal);
        for(unsigned j = 0; j < 3; ++j)
            if(isnan(vertices[i].normal[j]))
                vertices[i].normal[j] = 0.0f;*/
        if(vertices[i].face_count == 0)
            ++unused_vertices;
    }
    //std::cerr << "Unused vertices : " << unused_vertices << std::endl;
    //if(unused_vertices > 0) TODO : remove unused vertices
    return true;
}

std::string lowercase(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), tolower);
    return str;
}

std::string capitalize(std::string str)
{
    if(!str.empty())
    {
        str[0] = toupper(str[0]);
        std::transform(str.begin()+1, str.end(), str.begin()+1, tolower);
    }
    return str;
}

std::string uppercase(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), toupper);
    return str;
}

bool save(std::ostream & out, const std::string & label, const std::vector<Vertex> & vertices, const std::vector<Face> & faces)
{
    const std::string name = lowercase(label);
    const std::string Name = capitalize(label);
    const std::string NAME = uppercase(label);

    std::string index_type, index_enum;

    if(vertices.size() < 256)
    {
        index_type = "unsigned char";
        index_enum = "GL_UNSIGNED_BYTE";
    }
    else if(vertices.size() < 65536)
    {
        index_type = "unsigned short";
        index_enum = "GL_UNSIGNED_SHORT";
    }
    else
    {
        index_type = "unsigned";
        index_enum = "GL_UNSIGNED_INT";
    }

    out << "#ifndef " << NAME << "_H\n"
        << "#define " << NAME << "_H\n\n"
        << "#include <GL/GL.h>\n\n"
        << "const GLenum " << name << "IndexType = " << index_enum << ";\n\n"
        << "struct " << Name << "Vertex {\n"
        << "\tfloat position[3];\n"
        << "\tfloat normal[3];\n"
        << "} const " << name << "Vertices[" << vertices.size() << "] = {\n";

    for(unsigned i = 0; i < vertices.size(); ++i)
    {
        out << "\t{ { " << vertices[i].position[0] << ", " << vertices[i].position[1] << ", " << vertices[i].position[2]
            << " }, { " << vertices[i].normal[0] << ", " << vertices[i].normal[1] << ", " << vertices[i].normal[2] << " } }"
            << (i < (vertices.size()-1) ? ",\n" : "\n");
    }

    out << "};\n\n"
        << "const " << index_type << " " << name << "[" << faces.size() << "][3] = {\n";

    for(unsigned i = 0; i < faces.size(); ++i)
    {
        out << "\t{ " << faces[i].vertices[0] << ", " << faces[i].vertices[1] << ", " << faces[i].vertices[2] << " }"
            << (i < (faces.size()-1) ? ",\n" : "\n");
    }

    out << "};\n\n"
        << "#endif\n";

    return out;
}

struct AABB
{
    float x0, x1, y0, y1, z0, z1;
};

AABB getAABB(const std::vector<Vertex> & vertices)
{
    const float ninf = std::numeric_limits<float>::min();
    const float pinf = std::numeric_limits<float>::max();
    AABB aabb = { pinf, ninf, pinf, ninf, pinf, ninf };
    for(unsigned i = 0; i < vertices.size(); ++i)
    {
        aabb.x0 = std::min(aabb.x0, vertices[i].position.x);
        aabb.x1 = std::max(aabb.x1, vertices[i].position.x);
        aabb.y0 = std::min(aabb.y0, vertices[i].position.y);
        aabb.y1 = std::max(aabb.y1, vertices[i].position.y);
        aabb.z0 = std::min(aabb.z0, vertices[i].position.z);
        aabb.z1 = std::max(aabb.z1, vertices[i].position.z);
    }
    return aabb;
}

void convertAABB(std::vector<Vertex> & vertices, const AABB & from, const AABB & to)
{
    glm::vec3 move((to.x0+to.x1-from.x0-from.x1)/2, (to.y0+to.y1-from.y0-from.y1)/2, (to.z0+to.z1-from.z0-from.z1)/2);
    glm::vec3 scale((to.x1-to.x0)/(from.x1-from.x0), (to.y1-to.y0)/(from.y1-from.y0), (to.z1-to.z0)/(from.z1-from.z0));
    for(unsigned i = 0; i < vertices.size(); ++i)
    {
        vertices[i].position += move;
        vertices[i].position *= scale;
    }
}

int main(int argc, char * argv[])
{
    if(argc != 2)
    {
        std::cerr << "Usage : ply2h <label> <file.ply >file.h\n";
        return EXIT_FAILURE;
    }

    std::vector<Vertex> vertices;
    std::vector<Face> faces;

    if(!load(std::cin, vertices, faces))
    {
        std::cerr << "Input corrupted\n";
        return EXIT_FAILURE;
    }

    AABB aabb = getAABB(vertices);
    AABB new_aabb = { -1, 1, -1, 1, -1, 1 };

    convertAABB(vertices, aabb, new_aabb);

    if(!save(std::cout, argv[1], vertices, faces))
    {
        std::cerr << "Error while saving\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

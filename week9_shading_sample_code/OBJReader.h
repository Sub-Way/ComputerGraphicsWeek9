#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include "Vector3.h"
#include <algorithm>
#include <glm/glm.hpp>

class OBJReader
{
public:
	std::vector<glm::vec3 > pos_stack_;
	std::vector<glm::tvec3<unsigned int> > ix_stack_;
	std::vector<glm::vec3> nor_stack_;

	//Note: vector push_back is slow
	//Note: use unsigned int for indices

	void readObj(const char* filename)
	{
		using namespace std;

		std::cout << "Start reading OBJ file " << filename << std::endl;

		// to check if this obj file contains vt or vn data 
		bool read_vt(false), read_vn(false);

		const float max_limit = std::numeric_limits<float>::max();
		const float min_limit = std::numeric_limits<float>::min();

		Vector3<float> min(max_limit, max_limit, max_limit);
		Vector3<float> max(min_limit, min_limit, min_limit);

		ifstream file(filename);

		// check if file is opened correctly
		if (file.is_open() == false) {
			std::cout << filename << " does not exist. Program terminated." << std::endl; exit(-1);
		}

		char c[255];

		while (true)
		{
			file >> c;

			if (file.eof() != 0) break;						// finish reading if file is ended

			if (strcmp(c, "#") == 0) file.getline(c, 255);  // comments (less than 255 characters)
			else if (strcmp(c, "v") == 0) // vertices
			{
				float x, y, z;
				file >> x >> y >> z;
				//std::cout << x << " " << y << " " << z << std::endl;

				pos_stack_.push_back(glm::vec3(x, y, z));

				// bounding box
				max.x_ = std::max(max.x_, x);
				max.y_ = std::max(max.y_, y);
				max.z_ = std::max(max.z_, z);
				min.x_ = std::min(min.x_, x);
				min.y_ = std::min(min.y_, y);
				min.z_ = std::min(min.z_, z);
			}
			else if (strcmp(c, "vt") == 0)
			{
				read_vt = true;

				float u, v;
				file >> u >> v;

				//TODO: save texture coordinates

			}
			else if (strcmp(c, "vn") == 0)
			{
				read_vn = true;

				float nx, ny, nz;
				file >> nx >> ny >> nz;

				nor_stack_.push_back(glm::vec3(nx, ny, nz));
			}
			else if (strcmp(c, "f") == 0)
			{
				int v[3], vt[3], vn[3];
				if (read_vt == true && read_vn == true)
				{
					for (int i = 0; i < 3; i++)
					{
						file >> v[i]; file.get(c, 2);
						file >> vt[i]; file.get(c, 2);
						file >> vn[i];

						v[i]--;
						vt[i]--;
						vn[i]--;
					}
				}
				else if (read_vt == false && read_vn == true)
				{
					for (int i = 0; i < 3; i++)
					{
						file >> v[i]; file.get(c, 2); file.get(c, 2);
						file >> vn[i];
						v[i]--;
						vn[i]--;
					}
				}
				else if (read_vt == false && read_vn == false)
				{
					for (int i = 0; i < 3; i++)
					{
						file >> v[i];
						v[i]--;
					}
				}

				ix_stack_.push_back(glm::tvec3<unsigned int>(v[0], v[1], v[2]));

				if (read_vt == true) {
					//TODO
				}

				if (read_vn == true) {
					//TODO
				}

				//std::cout << v[0] << " " << v[1] << " " << v[2] << std::endl;
			}
		}

		file.clear();
		file.close();

		std::cout << "Reading complete." << std::endl;
		std::cout << "# of vertices " << pos_stack_.size() << std::endl;
		std::cout << "# of triangles " << ix_stack_.size() << std::endl;

		min.print();
		std::cout << " ";
		max.print();

		// move center to origin
		for (int i = 0; i < pos_stack_.size(); i++)
		{
			pos_stack_[i].x -= (max.x_ + min.x_)*0.5;
			pos_stack_[i].y -= (max.y_ + min.y_)*0.5;
			pos_stack_[i].z -= (max.z_ + min.z_)*0.5;
		}

		const float dx = max.x_ - min.x_;
		const float dy = max.y_ - min.y_;
		const float dz = max.z_ - min.z_;
		const float dl = std::max(dz, std::max(dx, dy));

		// scale down the BB to a unit cube
		for (int i = 0; i < pos_stack_.size(); i++)
		{
			pos_stack_[i].x /= dl;
			pos_stack_[i].y /= dl;
			pos_stack_[i].z /= dl;
		}

		recalculateNormalVector();
	}

	void dump()
	{
		for (int i = 0; i < pos_stack_.size(); i++)
		{
			std::cout << pos_stack_[i].x << " " << pos_stack_[i].y << " " << pos_stack_[i].z;
			std::cout << std::endl;
		}

		// scale down the BB to a unit cube
		for (int i = 0; i < pos_stack_.size(); i++)
		{
			std::cout << ix_stack_[i].x << " " << ix_stack_[i].y << " " << ix_stack_[i].z;
			std::cout << std::endl;
		}
	}

	void recalculateNormalVector()
	{
		using namespace std;
		using namespace glm;
		// 1. calculate face normals

		// prepare for a temporary memory
		vector<vec3> face_normals; //face를 담을 vector를 만든다.
		face_normals.resize(ix_stack_.size());
		// use cross product to calculate face normals
		for (int i = 0; i < ix_stack_.size(); i++) 
		{
			const unsigned int v0 = ix_stack_[i][0];
			const unsigned int v1 = ix_stack_[i][1];
			const unsigned int v2 = ix_stack_[i][2];

			const vec3 l0 = pos_stack_[v1] - pos_stack_[v0];
			const vec3 l1 = pos_stack_[v2] - pos_stack_[v1];

			const vec3 normal = glm::cross(l0, l1);

			face_normals[i] = normal;
		}

		// 2. find vertex normals

		// prepare for memory to store normal vectors
		nor_stack_.clear();
		nor_stack_.resize(pos_stack_.size());

		// make normal vectors zeros
		for (int i = 0; i < nor_stack_.size(); i++) {
			nor_stack_[i] = vec3(0.0f, 0.0f, 0.0f);
		}

		// accumulate face normals
		for (int i = 0; i < ix_stack_.size(); i++) {
			glm::vec3 &n0 = nor_stack_[ix_stack_[i][0]];
			glm::vec3 &n1 = nor_stack_[ix_stack_[i][1]];
			glm::vec3 &n2 = nor_stack_[ix_stack_[i][2]];

			n0 += face_normals[i];
			n1 += face_normals[i];
			n2 += face_normals[i];
		}

		// normalize vertex normals for smooth average face normals
		for (int i = 0; i < nor_stack_.size(); i++) {
			nor_stack_[i] = glm::normalize(nor_stack_[i]);

		}

	}
};

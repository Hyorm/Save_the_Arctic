#ifndef polar_H
#define polar_H



#define UVAR(name, i)	glGetUniformLocation(program[i], name) 
#define UVARS(name)	UVAR(name, light_mode) 
#define IS_MIPMAP(flag) ((flag) == GL_LINEAR_MIPMAP_LINEAR || (flag) == GL_LINEAR_MIPMAP_NEAREST || (flag) == GL_NEAREST_MIPMAP_LINEAR || (flag) == GL_NEAREST_MIPMAP_NEAREST)
#define MAP_FIND(map_obj, item) ((map_obj).find(item) != (map_obj).end())

#define BUFF_SIZE 1024

#include <GL/glew.h>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include "loadobj.h"
#include <stdexcept>
#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"

typedef std::vector<GLfloat> GLvec;

using namespace std;
using namespace glm;
using namespace tinyobj;

struct Model {
	string model_files;
	GLuint vao[2], vbo[4];
	float model_scales;
	vector<real_t> vertices;
	vector<real_t> normals;
	vector<real_t> colors;
	vector<vector<size_t>> vertex_map;
	vector<vector<size_t>> material_map;
	vector<shape_t> shapes;
	vector<material_t> materials;
	vector<real_t> texcoords;
	map<string, size_t> texmap;
	bool is_obj_valid = false;
	bool is_tex_valid = false;

	Model() {
		this->model_files = "models/nothing";
		this->vao[0] = { 0 };
		this->vao[1] = { 0 };

		this->vbo[0] = { 0 };
		this->vbo[1] = { 0 };
		this->vbo[2] = { 0 };
		this->vbo[3] = { 0 };
		this->model_scales = 1.0f;

	}

	Model(string model_files) {
		this->model_files = model_files;
		this->vao[0] = { 0 };
		this->vao[1] = { 0 };

		this->vbo[0] = { 0 };
		this->vbo[1] = { 0 };
		this->vbo[2] = { 0 };
		this->vbo[3] = { 0 };
		this->model_scales = 1.0f;
	}

	virtual void init(GLuint* program) {
		for (int i = 0; i < 2; i++) {
			attrib_t attrib;

			glGenVertexArrays(4, &vao[i]);
			glBindVertexArray(vao[i]);
			is_obj_valid = load_obj(
				model_files.c_str(), "models/", vertices,
				normals, vertex_map, material_map,
				attrib, shapes, materials, model_scales
			);

			glActiveTexture(GL_TEXTURE0);
			if (!is_tex_valid) {
				is_tex_valid = load_tex(
					"models/", texcoords, texmap,
					attrib.texcoords, shapes, materials,
					GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR
				);

				colors.resize(vertices.size());
			}
			glGenBuffers(4, vbo);

			bind_buffer(vbo[0], vertices, program[i], "vPosition_mc", 3);
			bind_buffer(vbo[1], normals, program[i], "vNormal_mc", 3);
			bind_buffer(vbo[2], texcoords, program[i], "vTexcoord", 2);
			bind_buffer(vbo[3], colors, program[i], "vColor", 3);

			bind_buffer(vbo[0], program[i], "vPosition_mc", 3);
			bind_buffer(vbo[1], program[i], "vNormal_mc", 3);
			bind_buffer(vbo[2], program[i], "vTexcoord", 2);
			bind_buffer(vbo[3], program[i], "vColor", 3);
		}
	}

	virtual void bind_buffer(GLint buffer, GLvec& vec, int program, const GLchar* attri_name, GLint attri_size) {
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vec.size(), vec.data(), GL_STATIC_DRAW);

		GLuint location = glGetAttribLocation(program, attri_name);

		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glVertexAttribPointer(location, attri_size, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(location);

	}

	virtual void bind_buffer(GLint buffer, int program, const GLchar* attri_name, GLint attri_size) {
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		GLuint location = glGetAttribLocation(program, attri_name);
		glVertexAttribPointer(location, attri_size, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(location);
	}

	virtual void draw_obj_model(int color_mode, int object_code, int light_mode, GLuint* program) {

		if (is_tex_valid) {
			glUniform1i(UVARS("is_tex_valid"), 1);
		}
		else
			glUniform1i(UVARS("is_tex_valid"), 0);
		glUniform1i(UVARS("ColorMode"), color_mode);
		glUniform1i(UVARS("ObjectCode"), object_code);
		glBindVertexArray(vao[light_mode]);

		auto& _shapes = shapes;
		auto& _materials = materials;
		auto& _vertex_map = vertex_map;
		auto& _material_map = material_map;
		auto& _texmap = texmap;
		for (size_t i = 0; i < _shapes.size(); ++i) {
			for (size_t j = 0; j < _material_map[i].size(); ++j) {

				int m_id = _material_map[i][j];
				if (m_id < 0) {
					glUniform1f(UVARS("n"), 10.0f);
					glUniform3f(UVARS("Ka"), 0.3f, 0.3f, 0.3f);
					glUniform3f(UVARS("Kd"), 1.0f, 1.0f, 1.0f);
					glUniform3f(UVARS("Ks"), 0.8f, 0.8f, 0.8f);
				}
				else {
					glUniform1f(UVARS("n"), _materials[m_id].shininess);
					glUniform3fv(UVARS("Ka"), 1, _materials[m_id].ambient);
					int diffuse_flag = 0;
					for (int d = 0; d < 3; d++) {
						if (_materials[m_id].diffuse[d] != 0) {
							diffuse_flag++;
						}
					}
					if (diffuse_flag != 0) 
						glUniform3fv(UVARS("Kd"), 1, _materials[m_id].diffuse);
					else
						glUniform1f(UVARS("Kd"), 0.75f);

					int specular_flag = 0;
					for (int d = 0; d < 3; d++) {
						if (_materials[m_id].specular[d] != 0) {
							specular_flag++;
						}
					}
					if (specular_flag != 0)
						glUniform3fv(UVARS("Ks"), 1, _materials[m_id].specular);
					else
						glUniform1f(UVARS("Ks"), 0.75f);

					auto texitem = _texmap.find(_materials[m_id].diffuse_texname);

					if (texitem != _texmap.end()) {
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, texitem->second);
						glUniform1i(UVARS("sampler"), 0);
					}
				}
				glDrawArrays(GL_TRIANGLES, _vertex_map[i][j], _vertex_map[i][j + 1] - _vertex_map[i][j]);
			}
		}
		/*
		auto& _shapes = shapes;
		auto& _materials = materials;
		auto& _vertex_map = vertex_map;
		auto& _material_map = material_map;
		auto& _texmap = texmap;

		for (size_t i = 0; i < _shapes.size(); ++i) {
			for (size_t j = 0; j < _material_map[i].size(); ++j) {
				int m_id = _material_map[i][j];
				if (m_id < 0) {
					glUniform1f(UVARS("n"), 10.0f);
					glUniform3f(UVARS("Ka"), 0.3f, 0.3f, 0.3f);
					glUniform3f(UVARS("Kd"), 1.0f, 1.0f, 1.0f);
					glUniform3f(UVARS("Ks"), 0.8f, 0.8f, 0.8f);
				}
				else {

					glUniform1f(UVARS("n"), _materials[m_id].shininess);
					glUniform3fv(UVARS("Ka"), 1, _materials[m_id].ambient);
					glUniform3fv(UVARS("Kd"), 1, _materials[m_id].diffuse);
					glUniform1f(UVARS("Ks"), 0.5);// _materials[m_id].specular);
					auto texitem = _texmap.find(_materials[m_id].diffuse_texname);

					if (texitem != _texmap.end()) {
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, texitem->second);
						glUniform1i(UVARS("sampler"), 0);
					}
				}

				glDrawArrays(GL_TRIANGLES, _vertex_map[i][j], _vertex_map[i][j + 1] - _vertex_map[i][j]);
			}
		}
		*/
	}


	virtual bool has_file(string filepath) {
		FILE* fp;
		if (fopen_s(&fp, filepath.c_str(), "rb") == 0) {
			fclose(fp);
			return true;
		}
		return false;
	
	}


	virtual GLuint generate_tex(
		const char* tex_file_path,
		GLint min_filter,
		GLint mag_filter)
	{
		int width, height, num_of_components;
		unsigned char* image = stbi_load(tex_file_path, &width, &height, &num_of_components, STBI_default);
		if (!image) {
			fprintf(stderr, "Failed to open %s\n", tex_file_path);
			return false;
		}
		// Generate a texture object and set its parameters. 
		GLuint texture_id;
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
		bool is_supported = true;
		switch (num_of_components) {
		case 3:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			break;
		case 4:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
			break;
		default:
			is_supported = false;
			break;
		}
		if (IS_MIPMAP(min_filter) || IS_MIPMAP(mag_filter))
			glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0);

		if (!is_supported) {
			fprintf(stderr, "Unsupported image format: %d components\n", num_of_components);
			glDeleteTextures(1, &texture_id);
			texture_id = -1;
		}

		return texture_id;
	}


	virtual bool load_tex(
		const char* basedir, vector<real_t>& texcoords_out, map<string, size_t>& texmap_out, const vector<real_t>& texcoords,
		const vector<shape_t>& shapes, const vector<material_t>& materials, GLint min_filter = GL_LINEAR_MIPMAP_LINEAR, GLint mag_filter = GL_LINEAR_MIPMAP_LINEAR) {
		size_t total_num_of_vertices = 0;
		size_t num_of_shapes = shapes.size();

		for (size_t s = 0; s < num_of_shapes; ++s) {
			total_num_of_vertices += shapes[s].mesh.indices.size();
		}
		texcoords_out.resize(total_num_of_vertices * 2);

		real_t* texcoords_dst_ptr = texcoords_out.data();
		const real_t* texcoords_src_ptr = texcoords.size() > 0 ? texcoords.data() : NULL;

		for (size_t s = 0; s < num_of_shapes; ++s) {
			const mesh_t& mesh = shapes[s].mesh;
			size_t num_of_faces = mesh.indices.size() / 3;

			for (size_t f = 0; f < num_of_faces; ++f) {
				// Get indices to the three vertices of a triangle.
				int idx[3] = {
					mesh.indices[3 * f + 0].texcoord_index,
					mesh.indices[3 * f + 1].texcoord_index,
					mesh.indices[3 * f + 2].texcoord_index
				};
				// Compute and copy valid texture coordinates. ......
				real_t tc[3][2];

				if (texcoords_src_ptr != NULL) {
					if (idx[0] < 0 || idx[1] < 0 || idx[2] < 0) {
						fprintf(stderr, "Invalid texture coordinate index\n");
						return false;
					}
					for (size_t i = 0; i < 3; ++i) {
						memcpy(tc[i], texcoords_src_ptr + idx[i] * 2, sizeof(real_t) * 2);
						tc[i][1] = 1.0f - tc[i][1];// flip the t coordinate. 
					}
				}
				else {
					tc[0][0] = tc[0][1] = 0;
					tc[1][0] = tc[1][1] = 0;
					tc[2][0] = tc[2][1] = 0;
				}
				memcpy(texcoords_dst_ptr, tc, sizeof(real_t) * 6);
				texcoords_dst_ptr += 6;

			}
		}


		// 2. Make a texture object for each material. 

		GLuint texture_id;
		size_t num_of_materials = materials.size();
		int tex_flag = 0;
		for (size_t m = 0; m < num_of_materials; ++m) {
			const material_t& mat = materials[m];
			const string& texname = mat.diffuse_texname;

			if (texname.empty()) continue;
			if (MAP_FIND(texmap_out, texname)) continue; 
			
			tex_flag++;
			
			// Open the texture image file. 
			string full_texpath = texname;
			if (!has_file(full_texpath)) {
				full_texpath = basedir + texname;
				if (!has_file(full_texpath)) {
					fprintf(stderr, "Failed to find %s\n", texname.c_str());
					return false;
				}
			}
			// Generate a texture object.
			texture_id = generate_tex(full_texpath.c_str(), min_filter, mag_filter);
			if (texture_id < 0) {
				return false;
			}

			// Register the texture id. 
			texmap_out[texname] = texture_id;
		}
		if(tex_flag < 1)
			return false;
		return true;
	
	}
};

struct Bear : public Model {
	Model* body;
	Model* rfl;
	Model* lfl;
	Model* rbl;
	Model* lbl;

	Bear(Model* body, Model* rfl, Model* lfl, Model* rbl, Model* lbl) {
		this->body = body;
		this->rfl = rfl;
		this->lfl = lfl;
		this->rbl = rbl;
		this->lbl = lbl;
	}

	virtual void draw() {
		
	
	}

};


#endif
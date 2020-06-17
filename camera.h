#ifndef camera_H
#define camera_H

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

struct Camera {
	enum { ORTHOGRAPHIC, PERSPECTIVE };
	vec3 eye;
	vec3 center;
	vec3 up;
	float zoom_factor;
	int projection_mode;
	float z_near;
	float z_far;
	float fovy;
	float x_right;
	Camera() :
		eye(-110, 7, 0),//-110, 7, 0
		center(0, 0, 0),
		up(0, 1, 0),
		zoom_factor(0.7f),
		projection_mode(ORTHOGRAPHIC),
		z_near(0.01f),
		z_far(110.0f),
		fovy((float)(M_PI / 180.0 * (30.0))),
		x_right(1.2f)
	{}

	mat4 get_viewing() {
		return lookAt(eye, center, up);
	}

	mat4 get_projection(float aspect) {
		mat4 P;
		switch (projection_mode) {
		case ORTHOGRAPHIC:
			P = parallel(zoom_factor * x_right, aspect, z_near, z_far);
			break;
		case PERSPECTIVE:
			P = glm::perspective(zoom_factor * fovy, aspect, z_near, z_far);
			break;
		}
		return P;
	}
	mat4 parallel(double r, double aspect, double n, double f) {
		double l = -r;
		double width = 2 * r;
		double height = width / aspect;
		double t = height / 2;
		double b = -t;

		return ortho(l, r, b, t, n, f);
	}

	void set_eye_viewing() {

		this->eye = vec3{ -110, 7, 0 };
		this->center = vec3{ 0, 0, 0 };
	}

	void set_add_eye_viewing(float x, float y, float z) {

		this->eye = vec3{eye[0] + x / 1000, eye[1] + y / 1000, z};
		this->center = vec3{ center[0] + x / 1000, center[1] + y / 1000, z };
	}
	
};
#endif
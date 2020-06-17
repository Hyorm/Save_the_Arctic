#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"

using namespace std;
using namespace glm;

Camera::Camera() {
	this->eye = vec3(0, 0, 8);
	this->center = vec3(0, 0, 0);
	this->up = vec3(0, 1, 0);
	this->zoom_factor = 1.0f;
	this->projection_mode = ORTHOGRAPHIC;
	this->z_near = 0.01f;
	this->z_far = 100.0f;
	this->fovy = (float)(M_PI / 180.0 * (30.0));
	this->x_right = 1.2f;
};

Camera::~Camera() {};

void Camera::setName(string name) {
	this->name = name;
}

void Camera::setEye(vec3 eye) {
	this->eye = eye;
}

void Camera::setCenter(vec3 center) {
	this->center = center;
}

void Camera::setUp(vec3 up) {
	this->up = up;
}


void Camera::setZoom_factor(float zoom_factor) {
	this->zoom_factor = zoom_factor;
}

void Camera::setProjection_mode(int projection_mode) {
	this->projection_mode = projection_mode;
}


void Camera::setZ_near(float z_near) {
	this->z_near = z_near;
}

void Camera::setZ_far(float z_far) {
	this->z_far = z_far;
}

void Camera::setFovy(float fovy) {
	this->fovy = fovy;
}

void Camera::setX_right(float x_right) {
	this->x_right = x_right;
}

vec3 Camera::getEye() {
	return this->eye;
}

vec3 Camera::getCenter() {
	return this->center;
}

vec3 Camera::getUp() {
	return this->up;
}

float Camera::getZoom_factor() {
	return this->zoom_factor;
}

float Camera::getProjection_mode() {
	return this->projection_mode;
}

float Camera::getZ_near() {
	return this->z_near;
}

float Camera::getZ_far() {
	return this->z_far;
}

float Camera::getFovy() {
	return this->fovy;
}

float Camera::getX_right() {
	return this->x_right;
}

mat4 Camera::get_viewing() {
	return lookAt(this->eye, this->center, this->up);
}

string Camera::getName() {
	return this->name;
}

mat4 Camera::get_projection(float aspect) {
	mat4 P;
	switch (this->projection_mode) {
	case ORTHOGRAPHIC:
		P = parallel(this->zoom_factor * this->x_right, aspect, this->z_near, this->z_far);
		break;
	case PERSPECTIVE:
		P = perspective(this->zoom_factor * this->fovy, aspect, this->z_near, this->z_far);
		break;
	}
	return P;
}

mat4 Camera::parallel(double r, double aspect, double n, double f) {
	double l = -r;
	double width = 2 * r;
	double height = width / aspect;
	double t = height / 2;
	double b = -t;

	return ortho(l, r, b, t, n, f);
}

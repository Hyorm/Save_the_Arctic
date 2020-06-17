#define dDOUBLE

#include <stdio.h>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>
#include "LoadShaders.h"
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#include "loadobj.h"
#include <ode/ode.h>
#include <drawstuff/drawstuff.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdexcept>
#include <windows.h>
#include "LoadShaders.h"
#include "camera.h"
#include "polar.h"

#define FPUSH_VTX3_AT(p, i, vx, vy, vz) do {size_t i3 = 3*(i);	p[i3 + 0] = (float)(vx);	p[i3 + 1] = (float)(vy); 	p[i3 + 2] = (float)(vz);} while(0)
#define FPUSH_VTX3(p, vx, vy, vz) do{p.push_back(vx); p.push_back(vy); p.push_back(vz);}while(0)
#define FSET_VTX3(vx, vy, vz, valx, valy, valz)do{vx = (float)(valx); vy = (float)(valy); vz = (float)(valz);}while(0)


using namespace glm;
using namespace std;
using namespace tinyobj;

void init();
void display();

void motion(int x, int y);
void mouse(int button, int state, int x, int y);
void mouse_wheel(int wheel, int dir, int x, int y);
void keyboard(unsigned char key, int x, int y);

void cb_menu(int value);
void cb_special(int key, int x, int y);

void render(int color_mode = 0);
void set_position_draw_model(mat4 m, int i, int color_mode);

//mat4 compute_modelling_transf(dBodyID body);
//static void nearCallback(void* data, dGeomID o1, dGeomID o2);

Camera camera;

GLuint program[2];

//Light modify
int light_mode = 1;

//mouse
int button_pressed[3] = { GLUT_UP, GLUT_UP, GLUT_UP };
int mouse_pos[2] = { 0, 0 };

int time_flag = 0;

int now_state = 0, game_in = 0;

//model
enum { MODEL_OCEAN, MODEL_GLACIER, 
	MODEL_POLAR_BODY, MODEL_POLAR_RFL, MODEL_POLAR_RBL, MODEL_POLAR_LFL, MODEL_POLAR_LBL, 
	MODEL_BOAT, MODEL_BULLET,
	MODEL_OIL, MODEL_U_BING,
	MODEL_FISH1, MODEL_FISH2, MODEL_FISH3, MODEL_FISH4,
	MODEL_CUBE,
	NUM_OF_MODELS }; 
const char* model_files[NUM_OF_MODELS] = { "models/ocean_final_real.obj", "models/glacier.obj", 
			"models/body.obj",  "models/lfl.obj", "models/lbl.obj" , "models/rfl.obj" , "models/rbl.obj" ,
			"models/boat.obj", "models/bullet.obj", 
			"models/oil.obj", "models/ubing_final.obj",
			"models/fish1.obj", "models/fish2.obj", "models/fish3.obj", "models/fish4.obj",
			"models/realcube.obj"
};//
float model_scales[NUM_OF_MODELS] = { 30.0f , 5.0f, 
			0.5f, 0.23f , 0.23f, 0.23f, 0.23f, 
			0.5f, 0.1f,
			20.0f, 12.0f,
			0.1f, 0.1f, 0.1f, 0.1f,
			1.3f
};
std::vector<Model*> models;

//model
int idx_selected = 0;

//polar bear move
float polar_x = -10.0f, polar_y = 0.0f, polar_z = 0.0f;
float leg_flag = 0, in = 0, leg_offset = 0;
int move_x = 0, move_z = 0;
float polar_theta = 90;

//dyingflag
float bullet_show = 2.5f;
void dying_ani();

//fish
float fish1 = 0.8, fish2 = 1, fish3 = -0.2, fish4 = -0.5;
int fish1_f = 0, fish2_f = 0, fish3_f = 0, fish4_f = 0;
void main(int argc, char** argv) {
	glewExperimental = GL_TRUE;

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(1000, 1000);

	glutCreateWindow("Save the arctic! - Soeun & Hyorim");

	GLenum err = glewInit();

	if (err != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	init();

	glutDisplayFunc(display);
	glutSpecialFunc(cb_special);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(motion);
	glutMouseFunc(mouse);
	//Add #include "freeglut_ext.h" to glut.h in external Dependencies
	glutMouseWheelFunc(mouse_wheel);
	glutMainLoop();
}

void motion(int x, int y) {

	int modifiers = glutGetModifiers();
	int is_alt_active = modifiers & GLUT_ACTIVE_ALT;
	int is_ctrl_active = modifiers & GLUT_ACTIVE_CTRL;
	int is_shift_active = modifiers & GLUT_ACTIVE_SHIFT;
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	GLfloat dx = 1.f * (x - mouse_pos[0]) / w;
	GLfloat dy = -1.f * (y - mouse_pos[1]) / h;

	if (button_pressed[GLUT_LEFT_BUTTON] == GLUT_DOWN) {
		if (is_alt_active) {
			printf("left alt_active\n");
			vec4 disp(camera.eye - camera.center, 1);
			GLfloat alpha = 2.0f;
			mat4 V = camera.get_viewing();
			mat4 Rx = rotate(mat4(1.0f), alpha * dy, vec3(transpose(V)[0]));
			mat4 Ry = rotate(mat4(1.0f), -alpha * dx, vec3(0, 1, 0));
			mat4 R = Ry * Rx; camera.eye = camera.center + vec3(R * disp);
			camera.up = mat3(R) * camera.up;
		}
	}

	if (button_pressed[GLUT_MIDDLE_BUTTON] == GLUT_DOWN) {
		if (is_alt_active) {
			printf("middle alt_active\n");
			mat4 VT = transpose(camera.get_viewing());
			camera.eye += vec3(-dx * VT[0] + -dy * VT[1]);
			camera.center += vec3(-dx * VT[0] + -dy * VT[1]);
		}
	}
	mouse_pos[0] = x;
	mouse_pos[1] = y;
	glutPostRedisplay();

}


void init() {

	srand((unsigned)time(NULL));
	for (int i = 0; i < NUM_OF_MODELS; i++) {
		models.push_back(new Model(model_files[i]));
	}
	//printf("%s\n", models[0]->name);
	ShaderInfo shaders[2][3] = {
		{{GL_VERTEX_SHADER, "gouraud.vert" },
		{GL_FRAGMENT_SHADER, "gouraud.frag"},
		{GL_NONE, NULL} },
		{{GL_VERTEX_SHADER, "phong.vert" },
		{GL_FRAGMENT_SHADER, "phong.frag"},
		{GL_NONE, NULL} }
	};

	for (int i = 0; i < 2; i++) {
		program[i] = LoadShaders(shaders[i]);
	}

	int num_of_models = (int)models.size();
	for (int j = 0; j < num_of_models; ++j) {
		models[j]->init(program);
	}

	//change light mode using cb_menu
	int menu_id = glutCreateMenu(cb_menu);

	glutAddMenuEntry("gouraud mode", 0);
	glutAddMenuEntry("phong mode", 1);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

}

void display() {
	glUseProgram(program[light_mode]);

	render();
	glFlush();
	glutPostRedisplay();

}

void mouse(int button, int state, int x, int y) {
	button_pressed[button] = state;
	//printf("state: %d\n", state);

	if (game_in == 0) {
		now_state += state;
	}
	if (now_state == 4) {
		game_in = 1;
	}
	mouse_pos[0] = x;
	mouse_pos[1] = y;
}

void mouse_wheel(int wheel, int dir, int x, int y) {
	int is_alt_active = glutGetModifiers() & GLUT_ACTIVE_ALT;

	if (is_alt_active) {
		glm::vec3 disp = camera.eye - camera.center;
		printf("wheel alt_active %f, %f, %f\n", disp[0], disp[1], disp[2]);
		if (dir > 0)
			camera.eye = camera.center + 0.95f * disp;
		else
			camera.eye = camera.center + 1.05f * disp;
	}
	else {
		printf("wheel alt_inactive %f \n", camera.zoom_factor);
		if (dir > 0)
			camera.zoom_factor *= 0.95f;
		else camera.zoom_factor *= 1.05f;
	}
	glutPostRedisplay();
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {

}

void cb_menu(int value) {
	//camera.setProjection_mode() = value;
	printf("light mode %d\n", value);
	light_mode = value;
	glutPostRedisplay();

}

void cb_special(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT) {
		move_z = 1;
		glutPostRedisplay();
	}
	if (key == GLUT_KEY_RIGHT) {
		move_z = -1;
		glutPostRedisplay();
	}
	if (key == GLUT_KEY_UP) {
		move_x = 1;
		glutPostRedisplay();
	}
}

void render(int color_mode) {

	GLfloat theta = 0.0001f * clock();

	glClearColor(1.f, 1.f, 1.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


		if(game_in == 1)time_flag++;
		for (int i = 0; i < NUM_OF_MODELS; i++) {
			if (time_flag > 2500) {
				dying_ani();
				time_flag = 0;
				continue;
			}
			if (models[i]->is_obj_valid) {
				//printf("%s\n", model->name);
				mat4 m(1.0f);
				switch (i) {
				case MODEL_OCEAN:
					if (game_in == 0)continue;
					m = translate(m, vec3(0.0f, 0.0f, 0.0f));
					m = rotate(m, theta, vec3(0.0f, 1.0f, 0.0f));
					break;
				case MODEL_GLACIER:
					if (game_in == 0)continue;
					m = translate(m, vec3(-8.0f, 0.5f, 0.0f));
					continue;
					break;
				case MODEL_POLAR_BODY:
					if (game_in == 0)continue;
					if (move_x == 1) {
						polar_x += 0.01f;
						camera.set_add_eye_viewing(-polar_x, 0.0f, polar_z);
					}
					if (move_z == -1) {
						polar_z += 0.01f;
						camera.set_add_eye_viewing(0.0f, 0.0f, polar_z);
					}
					if (move_z == 1) {
						polar_z -= 0.01f;
						camera.set_add_eye_viewing(0.0f, 0.0f, polar_z);
					}
					move_z = 0;
					m = translate(m, vec3(polar_x, polar_y, polar_z));
					m = rotate(m, radians(polar_theta), vec3(0.0f, -1.0f, 0.0f));
					break;
				case MODEL_POLAR_LFL:
					if (game_in == 0)continue;
					m = translate(m, vec3(polar_x, polar_y, polar_z));
					m = rotate(m, radians(polar_theta), vec3(0.0f, -1.0f, 0.0f));
					if (move_x == 1) {
						if (in == 0) {

							m = rotate(m, radians(leg_flag++), vec3(1.0f, 0.0f, 0.0f));
							if (leg_flag > 45) {
								in = 1;
							}
						}
						else {

							m = rotate(m, radians(leg_flag--), vec3(1.0f, 0.0f, 0.0f));
							if (leg_flag < -45) {
								in = 0;
							}
						}

					}
					break;
				case MODEL_POLAR_LBL:
					if (game_in == 0)continue;
					m = translate(m, vec3(polar_x, polar_y, polar_z));
					m = rotate(m, radians(polar_theta), vec3(0.0f, -1.0f, 0.0f));
					if (move_x == 1) {
						if (in == 0) {
							m = rotate(m, radians(leg_flag / 2), vec3(1.0f, 0.0f, 0.0f));
						}
						else {
							m = rotate(m, radians(leg_flag / 2), vec3(1.0f, 0.0f, 0.0f));
						}
						move_x = 0;
					}
					break;
				case MODEL_POLAR_RFL:
					if (game_in == 0)continue;
					m = translate(m, vec3(polar_x, polar_y, polar_z));
					m = rotate(m, radians(polar_theta), vec3(0.0f, -1.0f, 0.0f));
					if (move_x == 1) {
						if (in == 1) {
							m = rotate(m, radians(-leg_flag), vec3(1.0f, 0.0f, 0.0f));
						}
						else {
							m = rotate(m, radians(-leg_flag), vec3(1.0f, 0.0f, 0.0f));
						}

					}
					break;
				case MODEL_POLAR_RBL:
					if (game_in == 0)continue;
					m = translate(m, vec3(polar_x, polar_y, polar_z));
					m = rotate(m, radians(polar_theta), vec3(0.0f, -1.0f, 0.0f));
					if (move_x == 1) {
						if (in == 1) {
							m = rotate(m, -radians(leg_flag / 2), vec3(1.0f, 0.0f, 0.0f));
						}
						else {
							m = rotate(m, -radians((leg_flag / 2)), vec3(1.0f, 0.0f, 0.0f));
						}
					}

					break;
				case MODEL_BOAT:
					if (game_in == 0)continue;
					m = translate(m, vec3(2.5f, 0.05f, 0.0f));
					//m = rotate(m, radians(90.0f), vec3(0.0f, -1.0f, 0.0f));
					break;
				case MODEL_BULLET:
					if (game_in == 0)continue;
					if (polar_x < -6.5f || polar_z> 1.0f || polar_z < -0.1f)continue;
					bullet_show -= 0.1f;
					m = translate(m, vec3(3.0f + bullet_show, 0.2f, polar_z));
					m = rotate(m, radians(90.0f), vec3(0.0f, -1.0f, 0.0f));
					if (bullet_show < polar_x - 3.0f) {
						dying_ani();
						continue;
					}
					break;
				case MODEL_OIL:
					if (game_in == 0)continue;
					m = translate(m, vec3(10.0f, -0.3f, 13.0f));
					m = rotate(m, radians(70.0f), vec3(0.0f, 1.0f, 0.0f));
					if (polar_x < -9.1) {
						//printf("hear1\n");
						if (polar_z < -4.2)dying_ani();
					}
					else if (polar_x < -6.1) {
						//printf("hear2\n");
						if (polar_z < -2.5)dying_ani();
					}
					else if (polar_x < -4.1) {
						//printf("hear3\n");
						if (polar_z < -1.8)dying_ani();
					}
					else if (polar_x < -3.5) {
						//printf("hear4\n");
						if (polar_z < -1.5)dying_ani();
					}
					else if (polar_x < -1.65) {
						//printf("hear5\n");
						if (polar_z < -1.4)dying_ani();
					}
					else if (polar_x < -0.02 && polar_z < -1.0) {
						//printf("hear6\n");
						polar_x--;
						polar_z--;

						dying_ani();

					}
					else;
					break;
				case MODEL_U_BING:
					if (game_in == 0)continue;
					//printf("%f, %f\n", polar_x, polar_z);
					m = translate(m, vec3(-5.5f, 0.0f, 3.0f));
					if (polar_x < -9.1) {
						//printf("hear1\n");
						if (polar_z > 6.2)dying_ani();
					}
					else if (polar_x < -6.5) {
						//printf("hear2\n");
						if (polar_z > 4.7)dying_ani();
					}
					else if (polar_x < -3.8) {
						//printf("hear4\n");
						if (polar_z > 1.2)dying_ani();
					}
					else if (polar_x < -1.65) {
						//printf("hear5\n");
						if (polar_z > 2.4)dying_ani();
					}
					else if (polar_x < -0.02 && polar_z > 1.0) {
						//printf("hear6\n");
						polar_x--;
						polar_z--;

						dying_ani();

					}
					else;
					break;
					break;
				case MODEL_FISH1:
					if (game_in == 0)continue;
					if (fish1_f == 1) {
						fish1 += 0.01;
						if (fish1 > 0.2)fish1_f = 0;
					}
					else {
						fish1 -= 0.01;
						if (fish1 < -0.1)fish1_f = 1;
					}
					m = translate(m, vec3(0.5f, 0.0f, fish1 - 0.5f));
					m = rotate(m, radians(90.f), vec3(0.0f, 1.0f, 0.0f));

					break;
				case MODEL_FISH2:
					if (game_in == 0)continue;
					if (fish2_f == 1) {
						fish2 += 0.01;
						if (fish2 > 0.2)fish2_f = 0;
					}
					else {
						fish2 -= 0.01;
						if (fish1 < -0.1)fish2_f = 1;
					}
					m = translate(m, vec3(-3.7f, 0.0f, fish2 + 1.0f));
					m = rotate(m, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
					break;
				case MODEL_FISH3:
					if (game_in == 0)continue;
					if (fish3_f == 1) {
						fish3 += 0.01;
						if (fish3 > 0.2)fish3_f = 0;
					}
					else {
						fish3 -= 0.01;
						if (fish3 < -0.1)fish3_f = 1;
					}
					m = translate(m, vec3(-6.0f, 0.0f, fish3 + 0.5f));
					m = rotate(m, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
					break;
				case MODEL_FISH4:
					if (game_in == 0)continue;
					if (fish4_f == 1) {
						fish4 += 0.01;
						if (fish4 > 0.2)fish4_f = 0;
					}
					else {
						fish4 -= 0.01;
						if (fish4 < -0.1)fish4_f = 1;
					}
					m = translate(m, vec3(-1.0f, 0.0f, fish4));
					m = rotate(m, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
					break;
				case MODEL_CUBE:
					if (game_in == 1)continue;
					switch (now_state) {
					case 0:
						m = rotate(m, radians(180.f), vec3(1.0f, 0.0f, 0.0f));
						//m = rotate(m, radians(90.f), vec3(0.0f, 0.0f, 1.0f));
						break;
					case 1:
						m = rotate(m, radians(180.f), vec3(1.0f, 0.0f, 0.0f));
						m = rotate(m, radians(90.f), vec3(0.0f, 0.0f, 1.0f));
						break;
					case 2:
						m = rotate(m, radians(90.f), vec3(-1.0f, 0.0f, 0.0f));
						m = rotate(m, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
						break;
					case 3:
						m = rotate(m, radians(90.f), vec3(0.0f, 0.0f, -1.0f));
						break;
					case 5:
						m = rotate(m, -radians(90.f), vec3(1.0f, 0.0f, 0.0f));
						m = rotate(m, radians(90.f), vec3(0.0f, 0.0f, 1.0f));
						m = rotate(m, radians(90.f), vec3(0.0f, 0.0f, 2.0f));
						m = rotate(m, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
						break;
					default:
						m = rotate(m, -radians(90.f), vec3(1.0f, 0.0f, 0.0f));
						m = rotate(m, radians(90.f), vec3(0.0f, 0.0f, 1.0f));
						m = rotate(m, radians(90.f), vec3(0.0f, 0.0f, 2.0f));
						break;
					}
					break;
				}

				set_position_draw_model(m, i, color_mode);
			}
		}
	
	if (color_mode != 2) {
		glutSwapBuffers();
	}


}

void set_position_draw_model(mat4 m, int i, int color_mode) {
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);

	double aspect = 1.0 * width / height;
	m = scale(m, vec3(model_scales[i]));
	glUniformMatrix4fv(UVARS("LightPos_wc"), 1, GL_FALSE, value_ptr(vec4(10, 50, 3, 1)));//value_ptr(camera.get_viewing()));
	glUniformMatrix4fv(UVARS("M"), 1, GL_FALSE, value_ptr(m));
	glUniformMatrix4fv(UVARS("V"), 1, GL_FALSE, value_ptr(camera.get_viewing()));
	glUniformMatrix4fv(UVARS("P"), 1, GL_FALSE, value_ptr(camera.get_projection(aspect)));
	models[i]->draw_obj_model(color_mode, 1, light_mode, program);
}

void dying_ani() {
	game_in = 0;
	now_state = 5;
	camera.set_eye_viewing();
	//printf("dye");
}
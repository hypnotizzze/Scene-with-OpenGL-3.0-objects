#define GLEW_STATIC
#include <GL/glew.h> // GLEW
#include <GLFW/glfw3.h> // GLFW
#include <Windows.h>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <filesystem>
#include "V.h"
#include "Camera.h"
#include "Model.h"

#define PI 3.14159265



int WIDTH = 1000, HEIGHT = 800, current_sphere = 1;

bool keys[1024], mouses[10];
bool firstMouse = true;

float red_int = 0.1f, green_int = 0.1f, blue_int = 0.1f;
double x_1 = 0, z_1 = 0, x_2 = 0, z_2 = 0, x_3 = 0, z_3 = 0, speed_1 = 0.0f, speed_2 = 0.0f, speed_3 = 0.0f;

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;
GLfloat lastX = WIDTH / 2, lastY = HEIGHT / 2;
GLfloat radius_1 = 2.0f, radius_2 = 3.0f, radius_3 = 4.0f;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void do_movement();

glm::mat4 model = glm::mat4(1.0f); // default model mat

glm::mat4 bob_model = glm::mat4(1.0f); // Bob model mat

glm::mat4 sph_1_model = glm::mat4(1.0f); // Sphere 1 model mat

glm::mat4 sph_2_model = glm::mat4(1.0f); // Sphere 2 model mat

glm::mat4 sph_3_model = glm::mat4(1.0f); // Sphere 3 model mat

glm::vec3 sph_1_pos = glm::vec3(0.0f, 2.5f, radius_1); // Sphere 1 position

glm::vec3 sph_2_pos = glm::vec3(0.0f, 3.5f, radius_2); // Sphere 2 position

glm::vec3 sph_3_pos = glm::vec3(0.0f, 4.5f, radius_3); // Sphere 3 position

glm::vec3 sph_1_clr = glm::vec3(1.0f, 0.0f, 0.0f); // Sphere 1 color

glm::vec3 sph_2_clr = glm::vec3(0.0f, 1.0f, 0.0f); // Sphere 2 color

glm::vec3 sph_3_clr = glm::vec3(0.0f, 0.0f, 1.0f); // Sphere 3 color

glm::vec3 objectColor = glm::vec3(1.0f, 1.0f, 1.0f); // obj color

glm::vec3 floor_pos = glm::vec3(0.65f, 0.0f, -2.0f);

// Camera
Camera camera(glm::vec3(0.0f, 1.5f, 2.0f));

int main()
{
	//Инициализация GLFW
	if (!glfwInit())
		return -1;
	//Настройка GLFW
	//Задается минимальная требуемая версия OpenGL. 
	//Мажорная 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//Минорная
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//Установка профайла для которого создается контекст
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Выключение возможности изменения размера окна
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(1000, 800, "Kursovaya", nullptr, nullptr);

	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	glViewport(0, 0, WIDTH, HEIGHT);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	GLint nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

//	Shader_2 floor_Shader("suit_vert.txt", "suit_frag.txt");
	Shader_2 floor_Shader("suit_vert.txt", "f_universal.txt");
	//Shader_2 bob_Shader("suit_vert.txt", "suit_frag.txt");
	Shader_2 bob_Shader("suit_vert.txt", "f_universal.txt");
	Shader_2 red_Shader("vertex_1.txt", "f_red.txt");
	Shader_2 green_Shader("vertex_1.txt", "f_green.txt");
	Shader_2 blue_Shader("vertex_1.txt", "f_blue.txt");

	Model Floor("C:/GL/Project5/Kursovaya/pol/untitled.obj");
	Model Bob("C:/GL/Project5/Kursovaya/bob/meet-bob.obj");
	Model Sphere("C:/GL/Project5/Kursovaya/sphere/globe-sphere.obj");

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();

	while (!glfwWindowShouldClose(window))
	{
		GLfloat value_time = glfwGetTime();
		deltaTime = value_time - lastFrame;
		lastFrame = value_time; 
		glfwPollEvents(); // Проверка вызова события
		do_movement();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		view = camera.GetViewMatrix();

		if (x_1 >= 360) // Sphere 1
			x_1 -= 360;
		if (z_1 >= 360)
			z_1 -= 360;
		x_1 += speed_1;
		z_1 += speed_1;
		sph_1_model = glm::mat4(1.0f); // Sphere 1 model
		sph_1_pos = glm::vec3(sin(x_1 * PI / 180) * radius_1, 2.5f, cos(z_1 * PI / 180) * radius_1);
		red_Shader.use();
		red_Shader.setMat4("projection", projection);
		red_Shader.setMat4("view", view);
		sph_1_model = glm::translate(sph_1_model, sph_1_pos);
		red_Shader.setMat4("model", sph_1_model);
		red_Shader.setFloat("Intense", red_int);
		Sphere.Draw(red_Shader);

		if (x_2 >= 360) // Sphere 2 
			x_2 -= 360;
		if (z_2 >= 360)
			z_2 -= 360;
		x_2 += speed_2;
		z_2 += speed_2;
		sph_2_model = glm::mat4(1.0f); // Sphere 2 model
		sph_2_pos = glm::vec3(sin(x_2 * PI / 180) * radius_2, 3.5f, cos(z_2 * PI / 180) * radius_2);
		green_Shader.use();
		green_Shader.setMat4("projection", projection);
		green_Shader.setMat4("view", view);
		sph_2_model = glm::translate(sph_2_model, sph_2_pos);
		green_Shader.setMat4("model", sph_2_model);
		green_Shader.setFloat("Intense", green_int);
		Sphere.Draw(green_Shader);

		if (x_3 >= 360) // Sphere 3 
			x_3 -= 360;
		if (z_3>= 360)
			z_3 -= 360;
		x_3 += speed_3;
		z_3 += speed_3;
		sph_3_model = glm::mat4(1.0f); // Sphere 1 model
		sph_3_pos = glm::vec3(sin(x_3 * PI / 180) * radius_3, 4.5f, cos(z_3 * PI / 180) * radius_3);
		blue_Shader.use();
		blue_Shader.setMat4("projection", projection);
		blue_Shader.setMat4("view", view);
		sph_3_model = glm::translate(sph_3_model, sph_3_pos);
		blue_Shader.setMat4("model", sph_3_model);
		blue_Shader.setFloat("Intense", blue_int);
		Sphere.Draw(blue_Shader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, floor_pos);
		floor_Shader.use(); // floor
		floor_Shader.setFloat("f1", red_int);
		floor_Shader.setFloat("f2", green_int);
		floor_Shader.setFloat("f3", blue_int);
		floor_Shader.setVec3("objectColor", objectColor);
		floor_Shader.setVec3("lightColor_1", sph_1_clr);
		floor_Shader.setVec3("lightPos_1", sph_1_pos);
		floor_Shader.setVec3("lightColor_2", sph_2_clr);
		floor_Shader.setVec3("lightPos_2", sph_2_pos);
		floor_Shader.setVec3("lightColor_3", sph_3_clr);
		floor_Shader.setVec3("lightPos_3", sph_3_pos);
		bob_Shader.setMat4("projection", projection);
		bob_Shader.setMat4("view", view);
		bob_Shader.setMat4("model", model);
		Floor.Draw(floor_Shader);


		bob_Shader.use(); // Bob
		bob_Shader.setFloat("f1", red_int);
		bob_Shader.setFloat("f2", green_int);
		bob_Shader.setFloat("f3", blue_int);
		bob_Shader.setVec3("objectColor", objectColor);
		bob_Shader.setVec3("lightColor_1", sph_1_clr);
		bob_Shader.setVec3("lightPos_1", sph_1_pos);
		bob_Shader.setVec3("lightColor_2", sph_2_clr);
		bob_Shader.setVec3("lightPos_2", sph_2_pos);
		bob_Shader.setVec3("lightColor_3", sph_3_clr);
		bob_Shader.setVec3("lightPos_3", sph_3_pos);
		bob_Shader.setMat4("projection", projection);
		bob_Shader.setMat4("view", view);
		bob_Shader.setMat4("model", bob_model);
		Bob.Draw(bob_Shader);

		glfwSwapBuffers(window); // Смена местами буффера
	}
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
	if (key == GLFW_KEY_PAGE_UP && action == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
		bob_model = glm::rotate(bob_model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
		bob_model = glm::rotate(bob_model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		current_sphere = 1;
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		current_sphere = 2;
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		current_sphere = 3;
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		red_int += 0.1f;
		if (red_int >= 1.1f)
			red_int = 0.0f;
	}
	if (key == GLFW_KEY_G && action == GLFW_PRESS) {
		green_int += 0.1f;
		if (green_int >= 1.1f)
			green_int = 0.0f;
	}
	if (key == GLFW_KEY_B && action == GLFW_PRESS) {
		blue_int += 0.1f;
		if (blue_int >= 1.1f)
			blue_int = 0.0f;
	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		if (current_sphere == 1)
			speed_1 = 0.0f;
		else if (current_sphere == 2)
			speed_2 = 0.0f;
		else
			speed_3 = 0.0f;
	}
	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		if (current_sphere == 1)
			red_int = 0.99f;
		else if (current_sphere == 2)
			green_int = 0.99f;
		else
			blue_int = 0.99f;
	}
	if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		if (current_sphere == 1)
			red_int = 0.0f;
		else if (current_sphere == 2)
			green_int = 0.0f;
		else
			blue_int = 0.0f;
	}
}

void do_movement()
{
	// Camera controls
	GLfloat cameraSpeed = 5.0f * deltaTime;
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (keys[GLFW_KEY_UP])
		bob_model = glm::translate(bob_model, glm::vec3(0.0f, 0.0f, 0.005f));
	if (keys[GLFW_KEY_DOWN])
		bob_model = glm::translate(bob_model, glm::vec3(0.0f, 0.0f, -0.005f));
	if (keys[GLFW_KEY_LEFT])
		bob_model = glm::translate(bob_model, glm::vec3(0.005f, 0.0f, 0.0f));
	if (keys[GLFW_KEY_RIGHT])
		bob_model = glm::translate(bob_model, glm::vec3(-0.005f, 0.0f, 0.0f));
	if (keys[GLFW_KEY_Z]) {
		if (current_sphere == 1)
			speed_1 += 0.01f;
		else if (current_sphere == 2)
			speed_2 += 0.01f;
		else
			speed_3 += 0.01f;
	}
  if (keys[GLFW_KEY_X]) {
		if (current_sphere == 1)
			speed_1 -= 0.01f;
		else if (current_sphere == 2)
			speed_2 -= 0.01f;
		else
			speed_3 -= 0.01f;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button >= 0 && button < 10)
	{
		if (action == GLFW_PRESS)
			mouses[button] = true;
		else if (action == GLFW_RELEASE)
			mouses[button] = false;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
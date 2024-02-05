#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ew/shader.h>
#include <ew/model.h>
#include <ew/texture.h>
#include <ew/transform.h>
#include <ew/camera.h>
#include <ew/cameraController.h>
#include <xoxo/FrameBuffer.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title, int width, int height);
void drawUI();

//Global state
int screenWidth = 1080;
int screenHeight = 720;
float prevFrameTime;
float deltaTime;
ew::Camera camera;
ew::CameraController cameraController;

glm::vec3 lightDir(0.0, -1.0, 0.0);
glm::vec3 lightColor(1.0);
glm::vec3 ambientLight(0.3, 0.4, 0.46);

struct Material {
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float Shininess = 128;
}material;

GLfloat vertices[] =
{
	0,0,
	0,1,
	1,0,
	1,1,
};

GLuint indicies[] =
{
	0, 1, 2,
	2, 3, 0
};

int kernalIndex = 0;

int main() {	
	GLFWwindow* window = initWindow("Assignment 1", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f); //Look at the center of the scene
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f; //Vertical field of view, in degrees

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); //Back face culling
	glEnable(GL_DEPTH_TEST); //Depth testing

	ew::Shader shader = ew::Shader("assets/lit.vert", "assets/lit.frag");
	ew::Shader postProcessingShader = ew::Shader("assets/postProc.vert", "assets/postProc.frag");
	ew::Model monkeyModel = ew::Model("assets/suzanne.fbx");
	ew::Transform monkeyTransform;

	GLuint rockTexture = ew::loadTexture("assets/rock.jpg");
	xoxo::Framebuffer framebuffer = xoxo::createFramebuffer(screenWidth, screenHeight, GL_RGBA8);

	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebuffer incomplete: %d", fboStatus);
	}



	shader.use();
	shader.setInt("_MainTex", 0);

	unsigned int unintelligentVAO;
	glCreateVertexArrays(1, &unintelligentVAO);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		if (kernalIndex <= 2)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
			glViewport(0, 0, framebuffer.width, framebuffer.height);
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		//RENDER
		glClearColor(0.6f,0.8f,0.92f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		glBindTextureUnit(0, rockTexture);

		monkeyTransform.rotation = glm::rotate(monkeyTransform.rotation, deltaTime, glm::vec3(0.0, 1.0, 0.0));

		shader.use();
		shader.setVec3("_EyePos", camera.position);
		shader.setVec3("_LightDirection", lightDir);
		shader.setVec3("_LightColor", lightColor);
		shader.setVec3("_AmbientColor", ambientLight);

		shader.setFloat("_Material.Ka", material.Ka);
		shader.setFloat("_Material.Kd", material.Kd);
		shader.setFloat("_Material.Ks", material.Ks);
		shader.setFloat("_Material.Shininess", material.Shininess);

		shader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		shader.setMat4("_Model", monkeyTransform.modelMatrix());

		monkeyModel.draw(); //Draws monkey model using current shader

		cameraController.move(window, &camera, deltaTime);




		if (kernalIndex <= 2)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			postProcessingShader.use();
			postProcessingShader.setInt("_KernalIndex", kernalIndex);
		}
		
		glBindTextureUnit(0, *framebuffer.colorBuffer);
		glBindVertexArray(unintelligentVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		drawUI();

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void resetCamera(ew::Camera* camera, ew::CameraController* contoller)
{
	camera->position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera->target = glm::vec3(0);
	contoller->yaw = contoller->pitch = 0;
}

void drawUI() {
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Settings");

	if (ImGui::Button("Reset Camera! (Ohhhh)"))
	{
		resetCamera(&camera, &cameraController);
	}

	if (ImGui::CollapsingHeader("Material")) {
		ImGui::SliderFloat("AmbientK", &material.Ka, 0.0f, 1.0f);
		ImGui::SliderFloat("DiffuseK", &material.Kd, 0.0f, 1.0f);
		ImGui::SliderFloat("SpecularK", &material.Ks, 0.0f, 1.0f);
		ImGui::SliderFloat("Shininess", &material.Shininess, 2.0f, 1024.0f);
	}

	if (ImGui::CollapsingHeader("Light Controls"))
	{
		float lightPosition[3] = { lightDir.x, lightDir.y, lightDir.z };
		float lightColorData[3] = { lightColor.r * 255, lightColor.g * 255, lightColor.b * 255 };
		float ambientLightdata[3] = { ambientLight.r * 255, ambientLight.g * 255, ambientLight.b * 255 };

		ImGui::SliderFloat3("Light Direction", lightPosition, -2.5, 2.5);
		ImGui::SliderFloat3("Light Color", lightColorData, 0, 255);
		ImGui::SliderFloat3("Ambient Light", ambientLightdata, 0, 255);

		lightDir = glm::vec3(lightPosition[0], lightPosition[1], lightPosition[2]);
		lightColor = glm::vec3(lightColorData[0] / 255, lightColorData[1] / 255, lightColorData[2] / 255);
		ambientLight = glm::vec3(ambientLightdata[0] / 255, ambientLightdata[1] / 255, ambientLightdata[2] / 255);
	}

	if (ImGui::CollapsingHeader("Kernal Selection"))
	{
		ImGui::SliderInt("Kernal Index", &kernalIndex, 0, 3);
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;

	camera.aspectRatio = (float)screenWidth / screenHeight;
}

/// <summary>
/// Initializes GLFW, GLAD, and IMGUI
/// </summary>
/// <param name="title">Window title</param>
/// <param name="width">Window width</param>
/// <param name="height">Window height</param>
/// <returns>Returns window handle on success or null on fail</returns>
GLFWwindow* initWindow(const char* title, int width, int height) {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return nullptr;
	}

	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return nullptr;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	return window;
}


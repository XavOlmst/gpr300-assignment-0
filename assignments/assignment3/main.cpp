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
#include <ew/procGen.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title, int width, int height);
void drawUI();
void drawScene(ew::Shader shader, ew::Camera camera);

//Global state
int screenWidth = 1080;
int screenHeight = 720;
float prevFrameTime;
float deltaTime;
ew::Camera camera;
ew::Camera shadowCamera;
ew::CameraController cameraController;

glm::vec3 lightDir(0.01f, -1.0f, 0.01f);
glm::vec3 lightColor(1.0f);

struct Material {
	float Ka = 0.15f;
	float Kd = 0.5f;
	float Ks = 0.5f;
	float Shininess = 128.0f;
}material;

xoxo::Framebuffer shadowBuffer;
xoxo::Framebuffer gBuffer;
ew::Transform monkeyTransform;
ew::Transform planeTransform;

ew::Model monkeyModel;
ew::Mesh planeMesh;

GLuint rockTexture;
GLuint brickTexture;

int main() {
	GLFWwindow* window = initWindow("Assignment 3", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f); //Look at the center of the scene
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f; //Vertical field of view, in degrees

	shadowCamera.orthographic = true;
	shadowCamera.target = glm::vec3(0.0f, 0.0f, 0.0f);
	shadowCamera.position = -lightDir;
	shadowCamera.aspectRatio = 1;
	shadowCamera.orthoHeight = 10.0f;
	shadowCamera.farPlane = 14.0f;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); //Back face culling
	glEnable(GL_DEPTH_TEST); //Depth testing

	glm::mat4 lightProjectionView;

	//ew::Shader shader = ew::Shader("assets/lit.vert", "assets/lit.frag");
	ew::Shader shadowShader = ew::Shader("assets/shadow.vert", "assets/shadow.frag");
	ew::Shader geometryShader = ew::Shader("assets/geometryPass.vert", "assets/geometryPass.frag");
	ew::Shader deferredShader = ew::Shader("assets/deferredLit.vert", "assets/deferredLit.frag");
	shadowBuffer = xoxo::createDepthbuffer();
	gBuffer = xoxo::createGBuffer(screenWidth, screenHeight);


	monkeyModel = ew::Model("assets/suzanne.fbx");
	planeMesh = ew::Mesh(ew::createPlane(10, 10, 5));

	rockTexture = ew::loadTexture("assets/rock.jpg");
	brickTexture = ew::loadTexture("assets/brick_color.jpg");

	//deferredShader.use();
	//deferredShader.setInt("_MainTex", 0);

	planeTransform.position.y -= 1.2f;

	unsigned int unintelligentVAO;
	glCreateVertexArrays(1, &unintelligentVAO);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		//Geometry pass
		{
			glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.fbo);
			glViewport(0, 0, gBuffer.width, gBuffer.height);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			drawScene(geometryShader,camera);
		}

		//Shadow pass
		{
			//RENDER
			glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer.fbo);
			glViewport(0, 0, shadowBuffer.width, shadowBuffer.height);
			glClear(GL_DEPTH_BUFFER_BIT);

			//glCullFace(GL_FRONT);

			shadowCamera.position = -lightDir;
			drawScene(shadowShader, shadowCamera);

			//glCullFace(GL_BACK);
		}

		//Lighting pass
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, screenWidth, screenHeight);
			glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			deferredShader.use();
			deferredShader.setMat4("_LightViewProjection", shadowCamera.projectionMatrix() * shadowCamera.viewMatrix());
			deferredShader.setVec3("_EyePos", camera.position);
			deferredShader.setVec3("_LightDirection", glm::normalize(lightDir));
			deferredShader.setVec3("_LightColor", lightColor);

			deferredShader.setFloat("_Material.Ka", material.Ka);
			deferredShader.setFloat("_Material.Kd", material.Kd);
			deferredShader.setFloat("_Material.Ks", material.Ks);
			deferredShader.setFloat("_Material.Shininess", material.Shininess);

			glBindTextureUnit(0, gBuffer.colorBuffer[0]);
			glBindTextureUnit(1, gBuffer.colorBuffer[1]);
			glBindTextureUnit(2, gBuffer.colorBuffer[2]);
			glBindTextureUnit(3, shadowBuffer.depthBuffer);

			glBindVertexArray(unintelligentVAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
		

		cameraController.move(window, &camera, deltaTime);

		drawUI();

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void drawScene(ew::Shader shader, ew::Camera camera)
{
	shader.use();
	shader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());

	glBindTextureUnit(0, rockTexture);
	shader.setMat4("_Model", monkeyTransform.modelMatrix());
	monkeyModel.draw();

	glBindTextureUnit(0, brickTexture);
	shader.setMat4("_Model", planeTransform.modelMatrix());
	planeMesh.draw();
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

		ImGui::SliderFloat3("Light Direction", lightPosition, -5, 5);
		ImGui::SliderFloat3("Light Color", lightColorData, 0, 255);

		lightDir = glm::vec3(lightPosition[0], lightPosition[1], lightPosition[2]);
		lightColor = glm::vec3(lightColorData[0] / 255, lightColorData[1] / 255, lightColorData[2] / 255);
	}

	ImGui::End();

	ImGui::Begin("Shadow Map");
	ImGui::BeginChild("Shadow Map");

	ImVec2 windowSize = ImGui::GetWindowSize();

	ImGui::Image((ImTextureID)shadowBuffer.depthBuffer, windowSize, ImVec2(0, 1), ImVec2(1, 0));

	ImGui::EndChild();
	ImGui::End();

	ImGui::Begin("GBuffers"); {
		ImVec2 texSize = ImVec2(gBuffer.width / 4, gBuffer.height / 4);
		for (size_t i = 0; i < 3; i++)
		{
			ImGui::Image((ImTextureID)gBuffer.colorBuffer[i], texSize, ImVec2(0, 1), ImVec2(1, 0));
		}
		ImGui::End();
	}

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


#include "Renderer.h"
#include "GeometryNode.h"
#include "Tools.h"
#include <algorithm>
#include "ShaderProgram.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "OBJLoader.h"

// RENDERER
Renderer::Renderer()
{
	m_vbo_fbo_vertices = 0;
	m_vao_fbo = 0;
	
	m_geometric_object1 = nullptr;
	m_geometric_object2 = nullptr;
	m_geometric_object3 = nullptr;

	m_fbo = 0;
	m_fbo_texture = 0;

	m_rendering_mode = RENDERING_MODE::TRIANGLES;	
	m_continous_time = 0.0;

	// initialize the camera parameters
	m_camera_position = glm::vec3(1, 3, -6);
	m_camera_target_position = glm::vec3(0, 0, 0);
	m_camera_up_vector = glm::vec3(0, 1, 0);
}

Renderer::~Renderer()
{
	// delete g_buffer
	glDeleteTextures(1, &m_fbo_texture);
	glDeleteFramebuffers(1, &m_fbo);

	// delete common data
	glDeleteVertexArrays(1, &m_vao_fbo);
	glDeleteBuffers(1, &m_vbo_fbo_vertices);

	delete m_geometric_object1;
	delete m_geometric_object2;
	delete m_geometric_object3;
}

bool Renderer::Init(int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
	this->m_screen_width = SCREEN_WIDTH;
	this->m_screen_height = SCREEN_HEIGHT;

	// Initialize OpenGL functions

	//Set clear color
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//This enables depth and stencil testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);
	// glClearDepth sets the value the depth buffer is set at, when we clear it

	// Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// open the viewport
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); //we set up our viewport

	bool techniques_initialization = InitRenderingTechniques();
	bool buffers_initialization = InitDeferredShaderBuffers();
	bool items_initialization = InitCommonItems();
	bool lights_sources_initialization = InitLightSources();
	bool meshes_initialization = InitGeometricMeshes();

	//If there was any errors
	if (Tools::CheckGLError() != GL_NO_ERROR)
	{
		printf("Exiting with error at Renderer::Init\n");
		return false;
	}

	//If everything initialized
	return techniques_initialization && items_initialization && buffers_initialization && meshes_initialization && lights_sources_initialization;
}

void Renderer::Update(float dt)
{
	float movement_speed = 2.0f;
	// compute the direction of the camera
	glm::vec3 direction = glm::normalize(m_camera_target_position - m_camera_position);

	// move the camera towards the direction of the camera
	m_camera_position += m_camera_movement.x *  movement_speed * direction * dt;
	m_camera_target_position += m_camera_movement.x * movement_speed * direction * dt;

	// move the camera sideways
	glm::vec3 right = glm::normalize(glm::cross(direction, m_camera_up_vector));
	m_camera_position += m_camera_movement.y *  movement_speed * right * dt;
	m_camera_target_position += m_camera_movement.y * movement_speed * right * dt;

	glm::mat4 rotation = glm::mat4(1.0f);
	float angular_speed = glm::pi<float>() * 0.0025f;
	
	// compute the rotation of the camera based on the mouse movement
	rotation *= glm::rotate(glm::mat4(1.0), m_camera_look_angle_destination.y * angular_speed, right);
	rotation *= glm::rotate(glm::mat4(1.0), -m_camera_look_angle_destination.x  * angular_speed, m_camera_up_vector);
	m_camera_look_angle_destination = glm::vec2(0);
	
	// rotate the camera direction
	direction = rotation * glm::vec4(direction, 0);
	float dist = glm::distance(m_camera_position, m_camera_target_position);
	m_camera_target_position = m_camera_position + direction * dist;

	// compute the view matrix
	m_view_matrix = glm::lookAt(m_camera_position, m_camera_target_position, m_camera_up_vector);

	m_continous_time += dt;

	// update meshes tranformations
	m_geometric_object1_transformation_matrix = glm::scale(glm::mat4(1.f), glm::vec3(0.25f));
	m_geometric_object1_transformation_normal_matrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_geometric_object1_transformation_matrix))));

	glm::mat4 object_translation = glm::translate(glm::mat4(1.0), glm::vec3(2, 5, 0));
	glm::mat4 object_rotation = glm::rotate(glm::mat4(1.0), 1.5f*m_continous_time, glm::vec3(0, 1, 0));
	glm::mat4 object_scale = glm::scale(glm::mat4(1.0), glm::vec3(0.8f));
	m_geometric_object2_transformation_matrix = glm::translate(glm::mat4(1.0), glm::vec3(-4, 2, -2)) * object_translation * object_rotation * object_scale;
	m_geometric_object2_transformation_normal_matrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_geometric_object2_transformation_matrix))));

	m_geometric_object3_transformation_matrix = glm::translate(glm::mat4(1.f), glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1.f), glm::radians(10.f), glm::vec3(0,0,1)) * glm::scale(glm::mat4(1.0), glm::vec3(0.07f));
	m_geometric_object3_transformation_normal_matrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_geometric_object3_transformation_matrix))));
}

bool Renderer::InitCommonItems()
{
	glGenVertexArrays(1, &m_vao_fbo);
	glBindVertexArray(m_vao_fbo);

	GLfloat fbo_vertices[] = {
		-1, -1,
		1, -1,
		-1, 1,
		1, 1,
	};

	glGenBuffers(1, &m_vbo_fbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_fbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);

	return true;
}

bool Renderer::InitRenderingTechniques()
{
	bool initialized = true;

	// Geometry Rendering Program
	std::string vertex_shader_path = "../Data/Shaders/basic_rendering.vert";
	std::string fragment_shader_path = "../Data/Shaders/basic_rendering.frag";
	m_geometry_rendering_program.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	m_geometry_rendering_program.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	initialized = m_geometry_rendering_program.CreateProgram();
	m_geometry_rendering_program.LoadUniform("uniform_projection_matrix");
	m_geometry_rendering_program.LoadUniform("uniform_view_matrix");
	m_geometry_rendering_program.LoadUniform("uniform_model_matrix");
	m_geometry_rendering_program.LoadUniform("uniform_normal_matrix");
	m_geometry_rendering_program.LoadUniform("uniform_diffuse");
	m_geometry_rendering_program.LoadUniform("uniform_specular");
	m_geometry_rendering_program.LoadUniform("uniform_shininess");
	m_geometry_rendering_program.LoadUniform("uniform_has_texture");
	m_geometry_rendering_program.LoadUniform("diffuse_texture");
	m_geometry_rendering_program.LoadUniform("uniform_camera_position");
	// Light Source Uniforms
	m_geometry_rendering_program.LoadUniform("uniform_light_projection_matrix");
	m_geometry_rendering_program.LoadUniform("uniform_light_view_matrix");
	m_geometry_rendering_program.LoadUniform("uniform_light_position");
	m_geometry_rendering_program.LoadUniform("uniform_light_direction");
	m_geometry_rendering_program.LoadUniform("uniform_light_color");
	m_geometry_rendering_program.LoadUniform("uniform_light_umbra");
	m_geometry_rendering_program.LoadUniform("uniform_light_penumbra");
	m_geometry_rendering_program.LoadUniform("uniform_cast_shadows");
	m_geometry_rendering_program.LoadUniform("shadowmap_texture");

	// Post Processing Program
	vertex_shader_path = "../Data/Shaders/postproc.vert";
	fragment_shader_path = "../Data/Shaders/postproc.frag";
	m_postprocess_program.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	m_postprocess_program.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	initialized = initialized && m_postprocess_program.CreateProgram();
	m_postprocess_program.LoadUniform("uniform_texture");
	m_postprocess_program.LoadUniform("uniform_time");
	m_postprocess_program.LoadUniform("uniform_depth");
	m_postprocess_program.LoadUniform("uniform_projection_inverse_matrix");

	// Shadow mapping Program
	vertex_shader_path = "../Data/Shaders/shadow_map_rendering.vert";
	fragment_shader_path = "../Data/Shaders/shadow_map_rendering.frag";
	m_spot_light_shadow_map_program.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	m_spot_light_shadow_map_program.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	initialized = initialized && m_spot_light_shadow_map_program.CreateProgram();
	m_spot_light_shadow_map_program.LoadUniform("uniform_projection_matrix");
	m_spot_light_shadow_map_program.LoadUniform("uniform_view_matrix");
	m_spot_light_shadow_map_program.LoadUniform("uniform_model_matrix");
	
	return initialized;
}

bool Renderer::ReloadShaders()
{
	bool reloaded = true;

	reloaded = reloaded && m_geometry_rendering_program.ReloadProgram();
	reloaded = reloaded && m_postprocess_program.ReloadProgram();
	reloaded = reloaded && m_spot_light_shadow_map_program.ReloadProgram();

	return reloaded;
}

bool Renderer::InitDeferredShaderBuffers()
{
	// generate texture handles 
	glGenTextures(1, &m_fbo_texture);
	glGenTextures(1, &m_fbo_depth_texture);

	// framebuffer to link everything together
	glGenFramebuffers(1, &m_fbo);

	return ResizeBuffers(m_screen_width, m_screen_height);
}

// resize post processing textures and save the screen size
bool Renderer::ResizeBuffers(int width, int height)
{
	m_screen_width = width;
	m_screen_height = height;

	// texture
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_screen_width, m_screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	// depth texture
	glBindTexture(GL_TEXTURE_2D, m_fbo_depth_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_screen_width, m_screen_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	// framebuffer to link to everything together
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_fbo_depth_texture, 0);

	GLenum status = Tools::CheckFramebufferStatus(m_fbo);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_projection_matrix = glm::perspective(glm::radians(60.f), width / (float)height, 0.1f, 1500.0f);
	m_view_matrix = glm::lookAt(m_camera_position, m_camera_target_position, m_camera_up_vector);

	return true;
}

// Initialize the light sources
bool Renderer::InitLightSources()
{
	// Initialize the spot light
	m_spotlight_node.SetPosition(glm::vec3(8, 14, -3));
	m_spotlight_node.SetTarget(glm::vec3(0, 4, 0));
	m_spotlight_node.SetColor(40.0f * glm::vec3(255, 255, 251) / 255.f);
	m_spotlight_node.SetConeSize(50, 70);
	m_spotlight_node.CastShadow(true);

	return true;
}

// Load Geometric Meshes
bool Renderer::InitGeometricMeshes()
{
	bool initialized = true;
	OBJLoader loader;
	// load geometric object 1
	auto mesh = loader.load("../Data/Knossos/knossos.obj");
	if (mesh != nullptr)
	{
		m_geometric_object1 = new GeometryNode();
		m_geometric_object1->Init(mesh);
	}
	else
		initialized = false;

	// load geometric object 2
	mesh = loader.load("../Data/Pirates/weight.obj");
	if (mesh != nullptr)
	{
		m_geometric_object2 = new GeometryNode();
		m_geometric_object2->Init(mesh);
	}
	else
		initialized = false;

	// load geometric object 3
	mesh = loader.load("../Data/Pirates/skeleton.obj");
	if (mesh != nullptr)
	{
		m_geometric_object3 = new GeometryNode();
		m_geometric_object3->Init(mesh);
	}
	else
		initialized = false;

	return initialized;
}

void Renderer::SetRenderingMode(RENDERING_MODE mode)
{
	m_rendering_mode = mode;
}

// Render the Scene
void Renderer::Render()
{
	// Draw the geometry to the shadow maps
	RenderShadowMaps();

	// Draw the geometry
	RenderGeometry();

	// Render to screen
	RenderToOutFB();

	GLenum error = Tools::CheckGLError();
	if (error != GL_NO_ERROR)
	{
		printf("Reanderer:Draw GL Error\n");
		system("pause");
	}
}

void Renderer::RenderShadowMaps()
{
	// if the light source casts shadows
	if (m_spotlight_node.GetCastShadowsStatus())
	{
		int m_depth_texture_resolution = m_spotlight_node.GetShadowMapResolution();

		// bind the framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, m_spotlight_node.GetShadowMapFBO());
		glViewport(0, 0, m_depth_texture_resolution, m_depth_texture_resolution);
		GLenum drawbuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawbuffers);
		
		// clear depth buffer
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		
		// Bind the shadow mapping program
		m_spot_light_shadow_map_program.Bind();

		// pass the projection and view matrix to the uniforms
		glUniformMatrix4fv(m_spot_light_shadow_map_program["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(m_spotlight_node.GetProjectionMatrix()));
		glUniformMatrix4fv(m_spot_light_shadow_map_program["uniform_view_matrix"], 1, GL_FALSE, glm::value_ptr(m_spotlight_node.GetViewMatrix()));

		// draw the first object
		DrawGeometryNodeToShadowMap(m_geometric_object1, m_geometric_object1_transformation_matrix, m_geometric_object1_transformation_normal_matrix);

		// draw the second object
		DrawGeometryNodeToShadowMap(m_geometric_object2, m_geometric_object2_transformation_matrix, m_geometric_object2_transformation_normal_matrix);

		// draw the third object
		DrawGeometryNodeToShadowMap(m_geometric_object3, m_geometric_object3_transformation_matrix, m_geometric_object3_transformation_normal_matrix);
		
		glBindVertexArray(0);

		// Unbind shadow mapping program
		m_spot_light_shadow_map_program.Unbind();
		
		glDisable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}


void Renderer::RenderGeometry()
{
	// Bind the Intermediate framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, m_screen_width, m_screen_height);
	GLenum drawbuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawbuffers);
	
	// clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glEnable(GL_DEPTH_TEST);
	
	switch (m_rendering_mode)
	{
	case RENDERING_MODE::TRIANGLES:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case RENDERING_MODE::LINES:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case RENDERING_MODE::POINTS:
		glPointSize(2);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	};
	
	// Bind the geometry rendering program
	m_geometry_rendering_program.Bind();

	// pass camera parameters to uniforms
	glUniformMatrix4fv(m_geometry_rendering_program["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(m_projection_matrix));
	glUniformMatrix4fv(m_geometry_rendering_program["uniform_view_matrix"], 1, GL_FALSE, glm::value_ptr(m_view_matrix));
	glUniform3f(m_geometry_rendering_program["uniform_camera_position"], m_camera_position.x, m_camera_position.y, m_camera_position.z);
	
	// pass the light source parameters to uniforms
	glm::vec3 light_position = m_spotlight_node.GetPosition();
	glm::vec3 light_direction = m_spotlight_node.GetDirection();
	glm::vec3 light_color = m_spotlight_node.GetColor();
	glUniformMatrix4fv(m_geometry_rendering_program["uniform_light_projection_matrix"], 1, GL_FALSE, glm::value_ptr(m_spotlight_node.GetProjectionMatrix()));
	glUniformMatrix4fv(m_geometry_rendering_program["uniform_light_view_matrix"], 1, GL_FALSE, glm::value_ptr(m_spotlight_node.GetViewMatrix()));
	glUniform3f(m_geometry_rendering_program["uniform_light_position"], light_position.x, light_position.y, light_position.z);
	glUniform3f(m_geometry_rendering_program["uniform_light_direction"], light_direction.x, light_direction.y, light_direction.z);
	glUniform3f(m_geometry_rendering_program["uniform_light_color"], light_color.x, light_color.y, light_color.z);
	glUniform1f(m_geometry_rendering_program["uniform_light_umbra"], m_spotlight_node.GetUmbra());
	glUniform1f(m_geometry_rendering_program["uniform_light_penumbra"], m_spotlight_node.GetPenumbra());
	glUniform1i(m_geometry_rendering_program["uniform_cast_shadows"], (m_spotlight_node.GetCastShadowsStatus())? 1 : 0);
	// Set the sampler2D uniform to use texture unit 1
	glUniform1i(m_geometry_rendering_program["shadowmap_texture"], 1);
	// Bind the shadow map texture to texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, (m_spotlight_node.GetCastShadowsStatus()) ? m_spotlight_node.GetShadowMapDepthTexture() : 0);
	
	// Enable Texture Unit 0
	glUniform1i(m_geometry_rendering_program["uniform_diffuse_texture"], 0);
	glActiveTexture(GL_TEXTURE0);

	// draw the first object
	DrawGeometryNode(m_geometric_object1, m_geometric_object1_transformation_matrix, m_geometric_object1_transformation_normal_matrix);

	// draw the second object
	DrawGeometryNode(m_geometric_object2, m_geometric_object2_transformation_matrix, m_geometric_object2_transformation_normal_matrix);

	// draw the third object
	DrawGeometryNode(m_geometric_object3, m_geometric_object3_transformation_matrix, m_geometric_object3_transformation_normal_matrix);

	glBindVertexArray(0);
	m_geometry_rendering_program.Unbind();

	glDisable(GL_DEPTH_TEST);
	if(m_rendering_mode != RENDERING_MODE::TRIANGLES)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPointSize(1.0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawGeometryNode(GeometryNode* node, glm::mat4 model_matrix, glm::mat4 normal_matrix)
{
	glBindVertexArray(node->m_vao);
	glUniformMatrix4fv(m_geometry_rendering_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(model_matrix));
	glUniformMatrix4fv(m_geometry_rendering_program["uniform_normal_matrix"], 1, GL_FALSE, glm::value_ptr(normal_matrix));
	for (int j = 0; j < node->parts.size(); j++)
	{
		glm::vec3 diffuseColor = node->parts[j].diffuseColor;
		glm::vec3 specularColor = node->parts[j].specularColor;
		float shininess = node->parts[j].shininess;
		glUniform3f(m_geometry_rendering_program["uniform_diffuse"], diffuseColor.r, diffuseColor.g, diffuseColor.b);
		glUniform3f(m_geometry_rendering_program["uniform_specular"], specularColor.r, specularColor.g, specularColor.b);
		glUniform1f(m_geometry_rendering_program["uniform_shininess"], shininess);
		glUniform1f(m_geometry_rendering_program["uniform_has_texture"], (node->parts[j].textureID > 0) ? 1.0f : 0.0f);
		glBindTexture(GL_TEXTURE_2D, node->parts[j].textureID);

		glDrawArrays(GL_TRIANGLES, node->parts[j].start_offset, node->parts[j].count);
	}
}

void Renderer::DrawGeometryNodeToShadowMap(GeometryNode* node, glm::mat4 model_matrix, glm::mat4 normal_matrix)
{
	glBindVertexArray(node->m_vao);
	glUniformMatrix4fv(m_spot_light_shadow_map_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(model_matrix));
	for (int j = 0; j < node->parts.size(); j++)
	{
		glDrawArrays(GL_TRIANGLES, node->parts[j].start_offset, node->parts[j].count);
	}
}

void Renderer::RenderToOutFB()
{
	// Bind the default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_screen_width, m_screen_height);
	
	// clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// disable depth testing and blending
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	// bind the post processing program
	m_postprocess_program.Bind();
	
	glBindVertexArray(m_vao_fbo);
	// Bind the intermediate color image to texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
	glUniform1i(m_postprocess_program["uniform_texture"], 0);
	// Bind the intermediate depth buffer to texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_fbo_depth_texture);	
	glUniform1i(m_postprocess_program["uniform_depth"], 1);

	glUniform1f(m_postprocess_program["uniform_time"], m_continous_time);
	glm::mat4 projection_inverse_matrix = glm::inverse(m_projection_matrix);
	glUniformMatrix4fv(m_postprocess_program["uniform_projection_inverse_matrix"], 1, GL_FALSE, glm::value_ptr(projection_inverse_matrix));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);

	// Unbind the post processing program
	m_postprocess_program.Unbind();
}

void Renderer::CameraMoveForward(bool enable)
{
	m_camera_movement.x = (enable)? 1 : 0;
}
void Renderer::CameraMoveBackWard(bool enable)
{
	m_camera_movement.x = (enable) ? -1 : 0;
}

void Renderer::CameraMoveLeft(bool enable)
{
	m_camera_movement.y = (enable) ? -1 : 0;
}
void Renderer::CameraMoveRight(bool enable)
{
	m_camera_movement.y = (enable) ? 1 : 0;
}

void Renderer::CameraLook(glm::vec2 lookDir)
{
	m_camera_look_angle_destination = glm::vec2(1, -1) * lookDir;
}

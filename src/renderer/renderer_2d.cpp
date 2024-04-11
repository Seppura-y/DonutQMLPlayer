#include "renderer_2d.h"

#include "renderer/vertex_array.h"
#include "renderer/shader.h"

#include "renderer/render_command.h"

#include "renderer/uniform_buffer.h"


#include "opengl/opengl_shader.h"
#include "opengl/opengl_vertex_array.h"
#include "opengl/opengl_texture.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Donut
{
	struct RectangleVertex;
	struct CircleVertex;
	struct LineVertex;
	struct TextVertex;
	struct Renderer2DData;

	static Renderer2DData s_data;

	struct RectangleVertex
	{
		glm::vec3 position_;
		glm::vec4 color_;
		glm::vec2 tex_coordinate_;

		float texture_index_;
		float tiling_factor_;

		int entity_id_;
	};

	struct CircleVertex
	{
		glm::vec3 world_position_;
		glm::vec3 local_position_;
		glm::vec4 color_;
		float thickness_;
		float fade_;

		int entity_id_;
	};

	struct LineVertex
	{
		glm::vec3 position_;
		glm::vec4 color_;

		int entity_id_;
	};

	struct TextVertex
	{
		glm::vec3 position_;
		glm::vec4 color_;
		glm::vec2 tex_coordinate_;

		int entity_id_;
	};

	struct Renderer2DData
	{
		static const uint32_t max_rects_ = 20000;
		static const uint32_t max_vertices_ = max_rects_ * 4;
		static const uint32_t max_indices_ = max_rects_ * 6;
		static const uint32_t max_texture_slots_ = 32;

		Ref<Texture2D> white_texture_;

		////////////////////////////////////////////////////////////
		// Rectangle
		Ref<VertexArray> rectangle_va_;
		Ref<VertexBuffer> rectangle_vb_;
		Ref<Shader> rectangle_shader_;

		uint32_t rect_indices_count_ = 0;
		RectangleVertex* rect_vertex_buffer_base_ = nullptr;
		RectangleVertex* rect_vertex_buffer_ptr_ = nullptr;

		////////////////////////////////////////////////////////////
		// Circle
		Ref<VertexArray> circle_va_;
		Ref<VertexBuffer> circle_vb_;
		Ref<Shader> circle_shader_;

		uint32_t circle_indices_count_ = 0;
		CircleVertex* circle_vertex_buffer_base_ = nullptr;
		CircleVertex* circle_vertex_buffer_ptr_ = nullptr;

		////////////////////////////////////////////////////////////
		// Line
		Ref<VertexArray> line_va_;
		Ref<VertexBuffer> line_vb_;
		Ref<Shader> line_shader_;

		uint32_t line_vertex_count_ = 0;
		LineVertex* line_vertex_buffer_base_ = nullptr;
		LineVertex* line_vertex_buffer_ptr_ = nullptr;

		////////////////////////////////////////////////////////////
		// Text
		//Ref<VertexArray> text_va_;
		//Ref<VertexBuffer> text_vb_;
		//Ref<Shader> text_shader_;

		//uint32_t text_indices_count_ = 0;
		//TextVertex* text_vertex_buffer_base_ = nullptr;
		//TextVertex* text_vertex_buffer_ptr_ = nullptr;

		float line_width_ = 2.0f;

		std::array<Ref<Texture2D>, max_texture_slots_> texture_slots_;
		uint32_t texture_index_ = 1;	//slot 0 for white texture

		//Ref<Texture2D> font_atlas_texture_;

		glm::vec4 rect_vertex_positions_[4];
		Renderer2D::Statistics statistics_;

		struct CameraData
		{
			glm::mat4 view_projection_;
		};

		CameraData camera_buffer;
		Ref<UniformBuffer> camera_uniform_buffer;
	};


	void Renderer2D::init()
	{
		s_data.rectangle_va_ = VertexArray::create();

		s_data.rectangle_vb_.reset(VertexBuffer::create(s_data.max_vertices_ * sizeof(RectangleVertex)));
		s_data.rectangle_vb_->setLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float4, "a_Color"},
				{ ShaderDataType::Float2, "a_TexCoord"},
				{ ShaderDataType::Float,  "a_TexIndex"},
				{ ShaderDataType::Float,  "a_TilingFactor"},
				{ ShaderDataType::Int,  "a_EntityID"}
			});
		s_data.rectangle_va_->addVertexBuffer(s_data.rectangle_vb_);

		s_data.rect_vertex_buffer_base_ = new RectangleVertex[s_data.max_vertices_];

		uint32_t* rectangle_indices = new uint32_t[s_data.max_indices_];
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_data.max_indices_; i += 6)
		{
			rectangle_indices[i + 0] = offset + 0;
			rectangle_indices[i + 1] = offset + 1;
			rectangle_indices[i + 2] = offset + 2;

			rectangle_indices[i + 3] = offset + 2;
			rectangle_indices[i + 4] = offset + 3;
			rectangle_indices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> rectangle_ib;
		rectangle_ib.reset(IndexBuffer::create(rectangle_indices, s_data.max_indices_));
		s_data.rectangle_va_->setIndexBuffer(rectangle_ib);
		delete[] rectangle_indices;

		s_data.circle_va_ = VertexArray::create();

		s_data.circle_vb_.reset(VertexBuffer::create(s_data.max_indices_ * sizeof(CircleVertex)));
		s_data.circle_vb_->setLayout({
			{ShaderDataType::Float3,	"a_WorldPosition"},
			{ShaderDataType::Float3,	"a_LocalPosition"},
			{ShaderDataType::Float4,	"a_Color"},
			{ShaderDataType::Float,		"a_Thickness"},
			{ShaderDataType::Float,		"a_Fade"},
			{ShaderDataType::Int,		"a_EntityID"},
			});
		s_data.circle_va_->addVertexBuffer(s_data.circle_vb_);
		s_data.circle_va_->setIndexBuffer(rectangle_ib);
		s_data.circle_vertex_buffer_base_ = new CircleVertex[s_data.max_vertices_];


		s_data.line_va_ = VertexArray::create();

		s_data.line_vb_.reset(VertexBuffer::create(s_data.max_indices_ * sizeof(CircleVertex)));
		s_data.line_vb_->setLayout({
			{ShaderDataType::Float3,	"a_Position"},
			{ShaderDataType::Float4,	"a_Color"},
			{ShaderDataType::Int,		"a_EntityID"}
			});
		s_data.line_va_->addVertexBuffer(s_data.line_vb_);

		s_data.line_vertex_buffer_base_ = new LineVertex[s_data.max_vertices_];


		//s_data.text_va_ = VertexArray::create();

		//s_data.text_vb_.reset(VertexBuffer::create(s_data.max_indices_ * sizeof(TextVertex)));
		//s_data.text_vb_->setLayout({
		//	{ ShaderDataType::Float3,	"a_Position" },
		//	{ ShaderDataType::Float4,	"a_Color"},
		//	{ ShaderDataType::Float2,	"a_TexCoord"},
		//	{ ShaderDataType::Int,		"a_EntityID"}
		//	});
		//s_data.text_va_->addVertexBuffer(s_data.text_vb_);
		//s_data.text_vertex_buffer_base_ = new TextVertex[s_data.max_vertices_];

		//s_data.text_va_->setIndexBuffer(rectangle_ib);

		s_data.white_texture_ = Texture2D::createTexture(TextureSpecification());
		uint32_t white_texture_data = 0xffffffff;
		s_data.white_texture_->setData(&white_texture_data, sizeof(uint32_t));

		int samplers[s_data.max_texture_slots_];
		for (uint32_t i = 0; i < s_data.max_texture_slots_; i++)
		{
			samplers[i] = i;
		}

		s_data.circle_shader_ = Shader::createShader("assets/shaders/c8_circle_rendering.glsl");
		s_data.rectangle_shader_ = Shader::createShader("assets/shaders/c7_spirv_shader.glsl");
		s_data.line_shader_ = Shader::createShader("assets/shaders/c9_line_rendering.glsl");
		//s_data.text_shader_ = Shader::createShader("assets/shaders/c10_text_rendering.glsl");
		//s_data.single_shader_ = Shader::createShader("assets/shaders/c6_batch_texture_rendering_v2.glsl");
		//s_data.single_shader_->bind();
		//s_data.single_shader_->setIntArray("u_textures", samplers, s_data.max_texture_slots_);

		s_data.texture_slots_[0] = s_data.white_texture_;

		s_data.rect_vertex_positions_[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_data.rect_vertex_positions_[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_data.rect_vertex_positions_[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_data.rect_vertex_positions_[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		s_data.camera_uniform_buffer = UniformBuffer::create(sizeof(Renderer2DData::CameraData), 0);
	}

	void Renderer2D::shutdown()
	{
		delete[] s_data.rect_vertex_buffer_base_;
	}
	
	void Renderer2D::beginScene(const Camera& camera, const glm::mat4& transform)
	{

		//glm::mat4 view_projection = camera.getProjection() * glm::inverse(transform);

		//s_data.single_shader_->bind();
		//s_data.single_shader_->setMat4("u_viewProjectionMatrix", view_projection);

		s_data.camera_buffer.view_projection_ = camera.getProjection() * glm::inverse(transform);
		s_data.camera_uniform_buffer->setData(&s_data.camera_buffer, sizeof(Renderer2DData::CameraData));

		s_data.rect_indices_count_ = 0;
		s_data.rect_vertex_buffer_ptr_ = s_data.rect_vertex_buffer_base_;

		s_data.circle_indices_count_ = 0;
		s_data.circle_vertex_buffer_ptr_ = s_data.circle_vertex_buffer_base_;

		s_data.line_vertex_count_ = 0;
		s_data.line_vertex_buffer_ptr_ = s_data.line_vertex_buffer_base_;

		//s_data.text_indices_count_ = 0;
		//s_data.text_vertex_buffer_ptr_ = s_data.text_vertex_buffer_base_;

		s_data.texture_index_ = 1;
	}

	void Renderer2D::beginScene(const OrthographicCamera& camera)
	{
		s_data.rectangle_shader_->bind();
		s_data.rectangle_shader_->setMat4("u_viewProjectionMatrix", camera.getViewProjectionMatrix());

		s_data.rect_indices_count_ = 0;
		s_data.rect_vertex_buffer_ptr_ = s_data.rect_vertex_buffer_base_;

		s_data.circle_indices_count_ = 0;
		s_data.circle_vertex_buffer_ptr_ = s_data.circle_vertex_buffer_base_;

		s_data.line_vertex_count_ = 0;
		s_data.line_vertex_buffer_ptr_ = s_data.line_vertex_buffer_base_;

		//s_data.text_indices_count_ = 0;
		//s_data.text_vertex_buffer_ptr_ = s_data.text_vertex_buffer_base_;

		s_data.texture_index_ = 1;
	}

	void Renderer2D::endScene()
	{
		flush();
	}

	void Renderer2D::flush()
	{
		if (s_data.rect_indices_count_)
		{
			uint32_t data_size = (uint32_t)((uint8_t*)s_data.rect_vertex_buffer_ptr_ - (uint8_t*)s_data.rect_vertex_buffer_base_);
			s_data.rectangle_vb_->setData(s_data.rect_vertex_buffer_base_, data_size);


			// bind textures
			for (uint32_t i = 0; i < s_data.texture_index_; i++)
			{
				s_data.texture_slots_[i]->bind(i);
			}
			s_data.rectangle_shader_->bind();
			RenderCommand::drawIndices(s_data.rectangle_va_, s_data.rect_indices_count_);
			s_data.statistics_.drawcalls_++;
		}

		if (s_data.circle_indices_count_)
		{
			uint32_t data_size = (uint32_t)((uint8_t*)s_data.circle_vertex_buffer_ptr_ - (uint8_t*)s_data.circle_vertex_buffer_base_);
			s_data.circle_vb_->setData(s_data.circle_vertex_buffer_base_, data_size);


			// bind textures
			//for (uint32_t i = 0; i < s_data.texture_index_; i++)
			//{
			//	s_data.texture_slots_[i]->bind(i);
			//}
			s_data.circle_shader_->bind();
			RenderCommand::drawIndices(s_data.circle_va_, s_data.circle_indices_count_);
			s_data.statistics_.drawcalls_++;
		}


		if (s_data.line_vertex_count_)
		{
			uint32_t data_size = (uint32_t)((uint8_t*)s_data.line_vertex_buffer_ptr_ - (uint8_t*)s_data.line_vertex_buffer_base_);
			s_data.line_vb_->setData(s_data.line_vertex_buffer_base_, data_size);

			s_data.line_shader_->bind();
			RenderCommand::setLineWidth(s_data.line_width_);
			RenderCommand::drawLines(s_data.line_va_, s_data.line_vertex_count_);
			s_data.statistics_.drawcalls_++;
		}

		//if (s_data.text_indices_count_)
		//{
		//	uint32_t data_size = (uint32_t)((uint8_t*)s_data.text_vertex_buffer_ptr_ - (uint8_t*)s_data.text_vertex_buffer_base_);
		//	s_data.text_vb_->setData(s_data.text_vertex_buffer_base_, data_size);

		//	s_data.font_atlas_texture_->bind(0);

		//	s_data.text_shader_->bind();
		//	RenderCommand::drawIndices(s_data.text_va_, s_data.text_indices_count_);
		//	s_data.statistics_.drawcalls_++;
		//}
	}


	void Renderer2D::flushAndReset()
	{
		endScene();
		s_data.rect_indices_count_ = 0;
		s_data.rect_vertex_buffer_ptr_ = s_data.rect_vertex_buffer_base_;
		s_data.texture_index_ = 1;
	}
}
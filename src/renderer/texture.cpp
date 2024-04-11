#include "texture.h"

#include "renderer/renderer.h"
#include "opengl/opengl_texture.h"

namespace Donut
{
	Ref<Texture2D> Texture2D::createTexture(const TextureSpecification& spec)
	{
		switch (RendererAPI::getCurrentAPIType())
		{
		case RendererAPI::RendererAPIType::None: DN_CORE_ASSERT(false, "RendererAPI::RendererAPIType::None:"); return nullptr;
		case RendererAPI::RendererAPIType::OpenGL: return createRef<OpenGLTexture2D>(spec);
		}
		DN_CORE_ASSERT(false, "unknown renderer api");

		return nullptr;
	}
	Ref<Texture2D> Texture2D::createTexture(const std::string& path)
	{
		switch (RendererAPI::getCurrentAPIType())
		{
		case RendererAPI::RendererAPIType::None: DN_CORE_ASSERT(false, "RendererAPI::RendererAPIType::None:"); return nullptr;
		case RendererAPI::RendererAPIType::OpenGL: return createRef<OpenGLTexture2D>(path);
		}
		DN_CORE_ASSERT(false, "unknown renderer api");

		return nullptr;
	}
}
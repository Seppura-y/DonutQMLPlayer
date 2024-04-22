#ifndef OPENGL_FRAMEBUFFER_H
#define OPENGL_FRAMEBUFFER_H

#include <memory>
#include <vector>

#include "renderer_global.h"

namespace Donut
{
	enum class FramebufferTextureFormat
	{
		None = 0,

		RGBA8,
		RED_INTEGER,

		DEPTH24STENCIL8,

		// default
		Depth = DEPTH24STENCIL8
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
			: texture_format_(format) {}

		FramebufferTextureFormat texture_format_ = FramebufferTextureFormat::None;
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> textures_specs)
			: texture_specifications_(textures_specs) {}

		std::vector<FramebufferTextureSpecification> texture_specifications_;
	};

	struct FramebufferSpecification
	{
		uint32_t width_, height_;
		uint32_t samples_ = 1;
		FramebufferAttachmentSpecification attachments_specifications_;

		bool swapchain_target_ = false;
	};

	class OpenGLFramebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();

		virtual const FramebufferSpecification& getSpecification() const;

		void invalidate();
		virtual void resize(uint32_t width, uint32_t height);

		virtual int readPixel(uint32_t attachment, int x, int y);

		virtual void clearAttachment(uint32_t index, int value);

		virtual void bind();
		virtual void unBind();

		virtual uint32_t getColorAttachmentID(uint32_t index = 0) const
		{
			DN_CORE_ASSERT((index < color_attachments_.size()), "");
			return color_attachments_[index];
		}
	private:
		uint32_t object_id_ = 0;

		std::vector<FramebufferTextureSpecification> color_attachment_specifications_;
		FramebufferTextureSpecification depth_attachment_specification_ = FramebufferTextureFormat::None;

		std::vector<uint32_t> color_attachments_;
		uint32_t depth_attachment_ = 0;

		FramebufferSpecification specification_;
	};
}
#endif
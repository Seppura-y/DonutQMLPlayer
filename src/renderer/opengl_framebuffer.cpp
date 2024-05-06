#include "opengl_framebuffer.h"

//#include "donut_gl_item.h"
#include "donut_scene.h"
#include "renderer_global.h"

namespace Donut
{
	static const uint32_t s_max_framebuffer_size = 8192;


	namespace Utils {

		static GLenum textureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void createTextures(bool multisampled, uint32_t* outID, uint32_t count)
		{
			OPENGL_EXTRA_FUNCTIONS(glCreateTextures(textureTarget(multisampled), count, outID));
		}

		static void bindTexture(bool multisampled, uint32_t id)
		{
			OPENGL_EXTRA_FUNCTIONS(glBindTexture(textureTarget(multisampled), id));
		}

		static void attachColorTexture(uint32_t id, int samples, GLenum internal_format, GLenum format, uint32_t width, uint32_t height, int index)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				OPENGL_EXTRA_FUNCTIONS(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internal_format, width, height, GL_FALSE));
			}
			else
			{
				OPENGL_EXTRA_FUNCTIONS(glTexImage2D(
					GL_TEXTURE_2D,
					0,
					internal_format,			//internal format (how the opengl store the data)
					width, height,
					0,
					format,		//access format (how the opengl access tha data)
					GL_UNSIGNED_BYTE,
					nullptr
				));

				OPENGL_EXTRA_FUNCTIONS(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
				OPENGL_EXTRA_FUNCTIONS(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
				OPENGL_EXTRA_FUNCTIONS(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
				OPENGL_EXTRA_FUNCTIONS(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
				OPENGL_EXTRA_FUNCTIONS(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			}

			OPENGL_EXTRA_FUNCTIONS(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, textureTarget(multisampled), id, 0));
		}

		static void attachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				OPENGL_EXTRA_FUNCTIONS(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE));
			}
			else
			{
				OPENGL_EXTRA_FUNCTIONS(glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height));

				OPENGL_EXTRA_FUNCTIONS(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
				OPENGL_EXTRA_FUNCTIONS(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
				OPENGL_EXTRA_FUNCTIONS(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
				OPENGL_EXTRA_FUNCTIONS(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
				OPENGL_EXTRA_FUNCTIONS(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			}

			OPENGL_EXTRA_FUNCTIONS(glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, textureTarget(multisampled), id, 0));
		}

		static bool isDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
			}

			return false;
		}

		static GLenum texFormatToGLTextureFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::RED_INTEGER:
			{
				return GL_RED_INTEGER;
			}
			case FramebufferTextureFormat::RGBA8:
			{
				return GL_RGBA8;
			}

			default:
			{
				DN_CORE_ASSERT(false, "texFormatToGLTextureFormat -- unknown format");
				return GL_NONE;
			}

			}
		}

		static GLenum texFormatToGLTexureDataType(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::RED_INTEGER:
			{
				return GL_INT;
			}
			case FramebufferTextureFormat::RGBA8:
			{
				return GL_FLOAT;
			}

			default:
			{
				DN_CORE_ASSERT(false, "texFormatToGLTextureFormat -- unknown format");
				return GL_NONE;
			}

			}
		}

	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: specification_(spec)
	{
		for (auto spec : specification_.attachments_specifications_.texture_specifications_)
		{
			if (!Utils::isDepthFormat(spec.texture_format_))
			{
				color_attachment_specifications_.emplace_back(spec);
			}
			else
			{
				depth_attachment_specification_ = spec;
			}
		}

		invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		OPENGL_EXTRA_FUNCTIONS(glDeleteFramebuffers(1, &object_id_));
		OPENGL_EXTRA_FUNCTIONS(glDeleteTextures(color_attachments_.size(), color_attachments_.data()));
		OPENGL_EXTRA_FUNCTIONS(glDeleteTextures(1, &depth_attachment_));
	}

	const FramebufferSpecification& OpenGLFramebuffer::getSpecification() const
	{
		return specification_;
	}

	void OpenGLFramebuffer::invalidate()
	{
		if (object_id_)
		{
			OPENGL_EXTRA_FUNCTIONS(glDeleteFramebuffers(1, &object_id_));
			OPENGL_EXTRA_FUNCTIONS(glDeleteTextures(color_attachments_.size(), color_attachments_.data()));
			OPENGL_EXTRA_FUNCTIONS(glDeleteTextures(1, &depth_attachment_));
		}

		OPENGL_EXTRA_FUNCTIONS(glCreateFramebuffers(1, &object_id_));
		OPENGL_EXTRA_FUNCTIONS(glBindFramebuffer(GL_FRAMEBUFFER, object_id_));

		bool is_multisample = specification_.samples_ > 1;

		if (color_attachment_specifications_.size())
		{
			color_attachments_.resize(color_attachment_specifications_.size());
			Utils::createTextures(is_multisample, color_attachments_.data(), color_attachments_.size());

			for (size_t i = 0; i < color_attachments_.size(); i++)
			{
				Utils::bindTexture(is_multisample, color_attachments_[i]);
				switch (color_attachment_specifications_[i].texture_format_)
				{
				case FramebufferTextureFormat::RGBA8:
				{
					Utils::attachColorTexture(
						color_attachments_[i],
						specification_.samples_,
						GL_RGBA8,
						GL_RGBA,
						specification_.width_,
						specification_.height_,
						i
					);
					break;
				}

				case FramebufferTextureFormat::RED_INTEGER:
				{
					Utils::attachColorTexture(
						color_attachments_[i],
						specification_.samples_,
						GL_R32I,
						GL_RED_INTEGER,
						specification_.width_,
						specification_.height_,
						i
					);
					break;
				}
				}
			}
		}

		if (depth_attachment_specification_.texture_format_ != FramebufferTextureFormat::None)
		{
			Utils::createTextures(is_multisample, &depth_attachment_, 1);
			Utils::bindTexture(is_multisample, depth_attachment_);

			switch (depth_attachment_specification_.texture_format_)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8:
			{
				Utils::attachDepthTexture(
					depth_attachment_,
					specification_.samples_,
					GL_DEPTH24_STENCIL8,
					GL_DEPTH_STENCIL_ATTACHMENT,
					specification_.width_, specification_.height_
				);
				break;
			}
			}
		}

		if (color_attachments_.size() > 1)
		{
			DN_CORE_ASSERT(color_attachments_.size() <= 4, "");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			OPENGL_EXTRA_FUNCTIONS(glDrawBuffers(color_attachments_.size(), buffers));
		}
		else if (color_attachments_.empty())
		{
			// only depth
			OPENGL_EXTRA_FUNCTIONS(glDrawBuffer(GL_NONE));
		}

		DN_CORE_ASSERT(OPENGL_EXTRA_FUNCTIONS(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!"));

		OPENGL_EXTRA_FUNCTIONS(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	void OpenGLFramebuffer::resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_max_framebuffer_size || height > s_max_framebuffer_size)
		{
			return;
		}
		specification_.width_ = width;
		specification_.height_ = height;
		invalidate();
	}

	int OpenGLFramebuffer::readPixel(uint32_t attachment, int x, int y)
	{
		DN_CORE_ASSERT(attachment < color_attachments_.size(), "invalid attachment id");
		OPENGL_EXTRA_FUNCTIONS(glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment));

		int pixel_data;
		OPENGL_EXTRA_FUNCTIONS(glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel_data));

		return pixel_data;
	}

	void OpenGLFramebuffer::clearAttachment(uint32_t index, int value)
	{
		DN_CORE_ASSERT(index < color_attachments_.size(), "clearAttachment -- invalid index!");
		auto format = color_attachment_specifications_[index].texture_format_;
		switch (format)
		{
		case FramebufferTextureFormat::RED_INTEGER:
		{
			OPENGL_EXTRA_FUNCTIONS(glClearTexImage(color_attachments_[index], 0,
				Utils::texFormatToGLTextureFormat(format),
				GL_INT,
				&value
			));
			break;
		}
		}

	}

	void OpenGLFramebuffer::bind()
	{
		OPENGL_EXTRA_FUNCTIONS(glBindFramebuffer(GL_FRAMEBUFFER, object_id_));
		OPENGL_EXTRA_FUNCTIONS(glViewport(0, 0, specification_.width_, specification_.height_));
	}

	void OpenGLFramebuffer::unBind()
	{
		OPENGL_EXTRA_FUNCTIONS(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}
}
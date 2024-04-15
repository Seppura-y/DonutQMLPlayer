#include "opengl_texture.h"

#include "stb_image.h"
#include "render_global.h"

namespace Donut
{
	namespace Utils
	{
		static GLenum donutImageFormatToGLDataFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB8: return GL_RGB;
			case ImageFormat::RGBA8: return GL_RGBA;
			}

			DN_CORE_ASSERT(false, "donutImageFormatToGLDataFormat : unknown format");
			return 0;
		}

		static GLenum donutImageFormatToGLInternalFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB8: return GL_RGB8;
			case ImageFormat::RGBA8: return GL_RGBA8;
			}

			DN_CORE_ASSERT(false, "donutImageFormatToGLDataFormat : unknown format");
			return 0;
		}

	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& spec)
		: texture_spec_(spec), width_(spec.width_), height_(spec.height_)
	{
		internal_format_ = Utils::donutImageFormatToGLInternalFormat(texture_spec_.format_);
		data_format_ = Utils::donutImageFormatToGLDataFormat(texture_spec_.format_);

		OPENGL_EXTRA_FUNCTIONS(glGenTextures(1, &object_id_));
		OPENGL_EXTRA_FUNCTIONS(glBindTexture(GL_TEXTURE_2D, object_id_));
		OPENGL_EXTRA_FUNCTIONS(glTexStorage2D(GL_TEXTURE_2D, 1, internal_format_, width_, height_));

		OPENGL_EXTRA_FUNCTIONS(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		OPENGL_EXTRA_FUNCTIONS(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

		OPENGL_EXTRA_FUNCTIONS(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		OPENGL_EXTRA_FUNCTIONS(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: file_path_(path)
	{
		stbi_set_flip_vertically_on_load(1);
		int width, height, channels;

		stbi_uc* data = nullptr;
		{
			//DN_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string& path)");

			data = stbi_load(file_path_.c_str(), &width, &height, &channels, 0);
		}

		if (data)
		{
			is_loaded_ = true;

			width_ = width;
			height_ = height;

			GLenum internal_format = 0, data_format = 0;
			if (channels == 4)
			{
				internal_format = GL_RGBA8;
				data_format = GL_RGBA;
			}
			else if (channels == 3)
			{
				internal_format = GL_RGB8;
				data_format = GL_RGB;
			}

			internal_format_ = internal_format;
			data_format_ = data_format;

			DN_CORE_ASSERT(internal_format_ & data_format_, "image format is not supported!");

			width_ = width;
			height_ = height;
			channels_ = channels;

			OPENGL_EXTRA_FUNCTIONS(glGenTextures(1, &object_id_));
			OPENGL_EXTRA_FUNCTIONS(glBindTexture(GL_TEXTURE_2D, object_id_));
			OPENGL_EXTRA_FUNCTIONS(glTexStorage2D(GL_TEXTURE_2D, 1, internal_format_, width_, height_));

			OPENGL_EXTRA_FUNCTIONS(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			OPENGL_EXTRA_FUNCTIONS(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

			OPENGL_EXTRA_FUNCTIONS(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
			OPENGL_EXTRA_FUNCTIONS(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

			OPENGL_EXTRA_FUNCTIONS(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_, height_, data_format_, GL_UNSIGNED_BYTE, data));

			stbi_image_free(data);
		}
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		OPENGL_EXTRA_FUNCTIONS(glDeleteTextures(1, &object_id_));
	}

	void OpenGLTexture2D::setData(void* data, uint32_t size)
	{
		uint32_t bytes_per_pixel = data_format_ == GL_RGBA ? 4 : 3;
		DN_CORE_ASSERT(size == width_ * height_ * bytes_per_pixel, "data must be entire texture!");
		OPENGL_EXTRA_FUNCTIONS(glTexSubImage2D(object_id_, 0, 0, 0, width_, height_, data_format_, GL_UNSIGNED_BYTE, data));
	}

	uint32_t OpenGLTexture2D::getObjectId() const
	{
		return object_id_;
	}

	void OpenGLTexture2D::bind(uint32_t slot) const
	{
		OPENGL_EXTRA_FUNCTIONS(glActiveTexture(GL_TEXTURE0 + slot));
		OPENGL_EXTRA_FUNCTIONS(glBindTexture(GL_TEXTURE_2D, object_id_));
	}


}

#include "opengl_texture_2d.h"

#include "stb_image.h"

#include "donut_scene.h"
#include "renderer_global.h"

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
			case ImageFormat::R8: return GL_RED;
			}

			//DN_CORE_ASSERT(false, "donutImageFormatToGLDataFormat : unknown format");
			return 0;
		}

		static GLenum donutImageFormatToGLInternalFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB8: return GL_RGB8;
			case ImageFormat::RGBA8: return GL_RGBA8;
			case ImageFormat::R8: return GL_RED;
			}

			//DN_CORE_ASSERT(false, "donutImageFormatToGLDataFormat : unknown format");
			return 0;
		}

	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& spec)
		: texture_spec_(spec), width_(spec.width_), height_(spec.height_)
	{
		internal_format_ = Utils::donutImageFormatToGLInternalFormat(texture_spec_.format_);
		data_format_ = Utils::donutImageFormatToGLDataFormat(texture_spec_.format_);

		OPENGL_EXTRA_FUNCTIONS(glCreateTextures(GL_TEXTURE_2D, 1, &object_id_));
		OPENGL_EXTRA_FUNCTIONS(glTextureStorage2D(object_id_, 1, internal_format_, width_, height_));

		OPENGL_EXTRA_FUNCTIONS(glTextureParameteri(object_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		OPENGL_EXTRA_FUNCTIONS(glTextureParameteri(object_id_, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

		OPENGL_EXTRA_FUNCTIONS(glTextureParameteri(object_id_, GL_TEXTURE_WRAP_S, GL_REPEAT));
		OPENGL_EXTRA_FUNCTIONS(glTextureParameteri(object_id_, GL_TEXTURE_WRAP_T, GL_REPEAT));
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

			OPENGL_EXTRA_FUNCTIONS(glCreateTextures(GL_TEXTURE_2D, 1, &object_id_));
			OPENGL_EXTRA_FUNCTIONS(glTextureStorage2D(object_id_, 1, internal_format_, width_, height_));

			OPENGL_EXTRA_FUNCTIONS(glTextureParameteri(object_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			OPENGL_EXTRA_FUNCTIONS(glTextureParameteri(object_id_, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

			OPENGL_EXTRA_FUNCTIONS(glTextureParameteri(object_id_, GL_TEXTURE_WRAP_S, GL_REPEAT));
			OPENGL_EXTRA_FUNCTIONS(glTextureParameteri(object_id_, GL_TEXTURE_WRAP_T, GL_REPEAT));

			OPENGL_EXTRA_FUNCTIONS(glTextureSubImage2D(object_id_, 0, 0, 0, width_, height_, data_format_, GL_UNSIGNED_BYTE, data));

			stbi_image_free(data);
		}
	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		: width_(width), height_(height)
	{
		internal_format_ = GL_RGBA8, data_format_ = GL_RGBA;
		OPENGL_EXTRA_FUNCTIONS(glCreateTextures(GL_TEXTURE_2D, 1, &object_id_));
		OPENGL_EXTRA_FUNCTIONS(glTextureStorage2D(object_id_, 1, internal_format_, width_, height_));

		OPENGL_EXTRA_FUNCTIONS(glTextureParameteri(object_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		OPENGL_EXTRA_FUNCTIONS(glTextureParameteri(object_id_, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

		OPENGL_EXTRA_FUNCTIONS(glTextureParameteri(object_id_, GL_TEXTURE_WRAP_S, GL_REPEAT));
		OPENGL_EXTRA_FUNCTIONS(glTextureParameteri(object_id_, GL_TEXTURE_WRAP_T, GL_REPEAT));
	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, TextureFormat format)
	{
		if (format == TextureFormat::TEXTURE_FORMAT_YUV420)
		{
			width_ = width;
			height_ = height;
			channels_ = 1;


			internal_format_ = GL_RED;
			data_format_ = GL_RED;

			OPENGL_EXTRA_FUNCTIONS(glCreateTextures(GL_TEXTURE_2D, 1, &object_id_));
			//OPENGL_EXTRA_FUNCTIONS(glTextureStorage2D(object_id_, 1, internal_format_, width_, height_));
			OPENGL_EXTRA_FUNCTIONS(glBindTexture(GL_TEXTURE_2D, object_id_));

			OPENGL_EXTRA_FUNCTIONS(glTextureParameteri(object_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			OPENGL_EXTRA_FUNCTIONS(glTextureParameteri(object_id_, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

			OPENGL_EXTRA_FUNCTIONS(glTextureParameteri(object_id_, GL_TEXTURE_WRAP_S, GL_REPEAT));
			OPENGL_EXTRA_FUNCTIONS(glTextureParameteri(object_id_, GL_TEXTURE_WRAP_T, GL_REPEAT));

			//OPENGL_EXTRA_FUNCTIONS(glTextureStorage2D(object_id_, 1, internal_format_, width_, height_));
			OPENGL_EXTRA_FUNCTIONS(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0));
		}
		//else
		//{
		//	OpenGLTexture2D(width, height);
		//}
	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, uint32_t size, TextureFormat format)
	{
		if (format == TextureFormat::TEXTURE_FORMAT_YUV420)
		{
			width_ = width;
			height_ = height;
			channels_ = 1;


			internal_format_ = GL_RED;
			data_format_ = GL_RED;

			OPENGL_EXTRA_FUNCTIONS(glCreateTextures(GL_TEXTURE_2D, 1, &object_id_));
			OPENGL_EXTRA_FUNCTIONS(glTextureStorage2D(object_id_, 1, internal_format_, width_, height_));

			OPENGL_EXTRA_FUNCTIONS(glTextureParameteri(object_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			OPENGL_EXTRA_FUNCTIONS(glTextureParameteri(object_id_, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

			OPENGL_EXTRA_FUNCTIONS(glTextureParameteri(object_id_, GL_TEXTURE_WRAP_S, GL_REPEAT));
			OPENGL_EXTRA_FUNCTIONS(glTextureParameteri(object_id_, GL_TEXTURE_WRAP_T, GL_REPEAT));
		}
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		OPENGL_EXTRA_FUNCTIONS(glDeleteTextures(1, &object_id_));
	}

	void OpenGLTexture2D::setData(void* data, uint32_t size)
	{
		//uint32_t bytes_per_pixel = data_format_ == GL_RGBA ? 4 : 3;
		uint32_t bytes_per_pixel = 0;
		switch (data_format_)
		{
		case GL_RGBA:
			bytes_per_pixel = 4;
			break;
		case GL_RGB:
			bytes_per_pixel = 3;
			break;
		case GL_RED:
			bytes_per_pixel = 1;
			break;
		}
		DN_CORE_ASSERT(size == width_ * height_ * bytes_per_pixel, "data must be entire texture!");
		OPENGL_EXTRA_FUNCTIONS(glBindTexture(GL_TEXTURE_2D, object_id_));
		OPENGL_EXTRA_FUNCTIONS(glTextureSubImage2D(object_id_, 0, 0, 0, width_, height_, data_format_, GL_UNSIGNED_BYTE, data));
		is_loaded_ = true;
	}

	uint32_t OpenGLTexture2D::getObjectId() const
	{
		return object_id_;
	}

	void OpenGLTexture2D::bind(uint32_t slot) const
	{
		OPENGL_EXTRA_FUNCTIONS(glBindTextureUnit(slot, object_id_));
	}


}

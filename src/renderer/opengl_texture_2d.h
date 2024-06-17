#ifndef OPENGL_TEXTURE_2D_H
#define OPENGL_TEXTURE_2D_H

#include "renderer_global.h"

#include <stdint.h>
#include <string>

#include <QOpenGLTexture>

namespace Donut
{
	enum class ImageFormat
	{
		None = 0,
		R8,
		RGB8,
		RGBA8,
		RGBA32F
	};

	struct TextureSpecification
	{
		uint32_t width_ = 1;
		uint32_t height_ = 1;
		ImageFormat format_ = ImageFormat::RGBA8;
		bool generate_mips_ = true;
	};

	class OpenGLTexture2D
	{
	public:
		OpenGLTexture2D(const TextureSpecification& spec);
		OpenGLTexture2D(const std::string& path);
		OpenGLTexture2D(void* data);
		OpenGLTexture2D(uint32_t width, uint32_t height);
		OpenGLTexture2D(uint32_t width, uint32_t height, TextureFormat format);
		OpenGLTexture2D(uint32_t width, uint32_t height, uint32_t size, TextureFormat format);

		virtual ~OpenGLTexture2D();

		virtual const TextureSpecification& getSpecification() const { return texture_spec_; }

		virtual uint32_t getWidth() const { return width_; }
		virtual uint32_t getHeight() const { return height_; }
		float getRatio() const { return (float)width_ / (float)height_; }

		virtual void setData(void* data, uint32_t size);

		virtual uint32_t getObjectId() const;

		virtual const std::string& getPath() const { return file_path_; }

		virtual void bind(uint32_t slot = 0) const;

		virtual bool isLoaded() const { return is_loaded_; }

		virtual bool operator==(const OpenGLTexture2D& other) const
		{
			//return object_id_ == ((OpenGLTexture2D&)other).object_id_;
			return object_id_ == other.getObjectId();
		}
	private:
		uint32_t width_;
		uint32_t height_;
		uint32_t channels_;
		float aspect_ratio_;

		uint32_t object_id_;

		std::string file_path_;

		uint32_t internal_format_, data_format_;

		bool is_loaded_ = false;

		TextureSpecification texture_spec_;
	};
}


#endif
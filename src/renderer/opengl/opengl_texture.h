#ifndef OPENGL_TEXTURE_H
#define OPENGL_TEXTURE_H

#include "renderer/texture.h"

namespace Donut
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const TextureSpecification& spec);
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		virtual const TextureSpecification& getSpecification() const override { return texture_spec_; }

		virtual uint32_t getWidth() const override { return width_; }
		virtual uint32_t getHeight() const override { return height_; }

		virtual void setData(void* data, uint32_t size) override;

		virtual uint32_t getObjectId() const override;

		virtual const std::string& getPath() const override { return file_path_; }

		virtual void bind(uint32_t slot = 0) const override;

		virtual bool isLoaded() const override { return is_loaded_; }

		virtual bool operator==(const Texture& other) const override
		{
			//return object_id_ == ((OpenGLTexture2D&)other).object_id_;
			return object_id_ == other.getObjectId();
		}
	private:
		uint32_t width_;
		uint32_t height_;
		uint32_t channels_;

		uint32_t object_id_;

		std::string file_path_;

		GLenum internal_format_, data_format_;

		bool is_loaded_ = false;

		TextureSpecification texture_spec_;
	};
}

#endif
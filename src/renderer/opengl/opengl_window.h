//#ifndef OPENGL_WINDOW_H
//#define OPENGL_WINDOW_H
//
//
//#include "renderer/graphics_context.h"
//
//#include <QOpenGLWidget>
//
//namespace Donut
//{
//	class OpenGLWindow : public Window, public QOpenGLWidget
//	{
//		Q_OBJECT
//	public:
//		OpenGLWindow() = default;
//		OpenGLWindow(const WindowProps& props, QWidget* parent = nullptr);
//		virtual ~OpenGLWindow();
//
//		void onUpdate() override;
//
//		inline uint32_t getWidth() const override { return win_data_.width_; }
//		inline uint32_t getHeight() const override { return win_data_.height_; }
//
//		// Window attributes
//		inline void setEventCallback(const EventCallbackFn& callback) override { win_data_.event_callback_ = callback; }
//		void setVSync(bool enabled) override;
//		bool isVSync() const override;
//
//		inline virtual void* getNativeWindow() const override { return nullptr; };
//
//		GraphicsContext* getGraphicsContext() override;
//
//	protected:
//		void initializeGL() override;
//		void paintGL() override;
//		void resizeGL(int width, int height) override;
//
//	private:
//		virtual void init(const WindowProps& props);
//		virtual void shutdown();
//
//	private:
//		GraphicsContext* graphics_ctx_;
//
//		struct WindowData
//		{
//			std::string title_;
//			unsigned int width_, height_;
//			bool is_vsync_;
//
//			EventCallbackFn event_callback_;
//		};
//		const WindowProps& win_props_;
//		WindowData win_data_;
//	};
//}
//
//#endif
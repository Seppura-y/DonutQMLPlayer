//#include "pch.h"
//
//#include "opengl_window.h"
//#include "events/application_event.h"
//#include "events/mouse_event.h"
//#include "events/key_event.h"
//
//#include "platform/opengl/opengl_context.h"
//
//
//namespace Donut
//{
//	Window* Window::create(const WindowProps& props)
//	{
//		return new OpenGLWindow(props);
//	}
//
//	OpenGLWindow::OpenGLWindow(const WindowProps& props, QWidget* parent)
//		: QOpenGLWidget(parent), win_props_(props)
//	{
//		init(props);
//		this->QOpenGLWidget::setGeometry(0, 0, props.width, props.height);
//		this->QOpenGLWidget::show();
//	}
//
//	OpenGLWindow::~OpenGLWindow()
//	{
//		//DN_PROFILE_FUNCTION();
//
//		shutdown();
//	}
//
//	void OpenGLWindow::init(const WindowProps& props)
//	{
//		//DN_PROFILE_FUNCTION();
//
//		win_data_.title_ = props.title;
//		win_data_.width_ = props.width;
//		win_data_.height_ = props.height;
//
//		auto context = QOpenGLContext::currentContext();
//		//auto context = this->context();
//		if (!context)
//		{
//			context = new QOpenGLContext();
//		}
//
//
//		//QSurfaceFormat format;
//		//format.setMajorVersion(4);
//		//format.setMinorVersion(5);
//		//format.setProfile(QSurfaceFormat::CoreProfile);
//
//		//context->setFormat(format);
//
//		auto render_surface = new QOffscreenSurface();
//		render_surface->setFormat(context->format());
//		render_surface->create();
//
//		context->create();
//		context->makeCurrent(render_surface);
//
//		graphics_ctx_ = new OpenGLContext(context, render_surface);
//		graphics_ctx_->init();
//
//		//glfwMakeContextCurrent(glfw_window_);
//		//int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
//
//		//glfwSetWindowUserPointer(glfw_window_, &win_data_);
//		setVSync(true);
//
//		//// Set GLFW callbacks
//		//glfwSetWindowSizeCallback(glfw_window_, [](GLFWwindow* window, int width, int height)
//		//	{
//		//		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
//		//data.width = width;
//		//data.height = height;
//
//		//WindowResizeEvent event(width, height);
//		//data.event_callback(event);
//		//	});
//
//		//glfwSetWindowCloseCallback(glfw_window_, [](GLFWwindow* window)
//		//	{
//		//		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
//		//WindowCloseEvent event;
//		//data.event_callback(event);
//		//	});
//
//		//glfwSetKeyCallback(glfw_window_, [](GLFWwindow* window, int key, int scancode, int action, int mods)
//		//	{
//		//		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
//
//		//switch (action)
//		//{
//		//case GLFW_PRESS:
//		//{
//		//	KeyPressedEvent event(key, false);
//		//	data.event_callback(event);
//		//	break;
//		//}
//		//case GLFW_RELEASE:
//		//{
//		//	KeyReleasedEvent event(key);
//		//	data.event_callback(event);
//		//	break;
//		//}
//		//case GLFW_REPEAT:
//		//{
//		//	KeyPressedEvent event(key, true);
//		//	data.event_callback(event);
//		//	break;
//		//}
//		//}
//		//	});
//
//		//glfwSetMouseButtonCallback(glfw_window_, [](GLFWwindow* window, int button, int action, int mods)
//		//	{
//		//		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
//
//		//switch (action)
//		//{
//		//case GLFW_PRESS:
//		//{
//		//	MouseButtonPressedEvent event(button);
//		//	data.event_callback(event);
//		//	break;
//		//}
//		//case GLFW_RELEASE:
//		//{
//		//	MouseButtonReleasedEvent event(button);
//		//	data.event_callback(event);
//		//	break;
//		//}
//		//}
//		//	});
//
//		//glfwSetScrollCallback(glfw_window_, [](GLFWwindow* window, double x_offset, double y_offset)
//		//	{
//		//		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
//
//		//MouseScrolledEvent event((float)x_offset, (float)y_offset);
//		//data.event_callback(event);
//		//	});
//
//		//glfwSetCursorPosCallback(glfw_window_, [](GLFWwindow* window, double x_pos, double y_pos)
//		//	{
//		//		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
//
//		//MouseMovedEvent event((float)x_pos, (float)y_pos);
//		//data.event_callback(event);
//		//	});
//
//		//glfwSetCharCallback(glfw_window_, [](GLFWwindow* window, unsigned int keycode)
//		//	{
//		//		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
//		//KeyTypedEvent ev(keycode);
//		//data.event_callback(ev);
//		//	});
//
//	}
//
//	void OpenGLWindow::shutdown()
//	{
//		//DN_PROFILE_FUNCTION();
//
//		//glfwDestroyWindow(glfw_window_);
//	}
//
//	void OpenGLWindow::onUpdate()
//	{
//		//DN_PROFILE_FUNCTION();
//
//		//glfwPollEvents();
//		this->QOpenGLWidget::update();
//		graphics_ctx_->swapBuffers();
//	}
//
//	void OpenGLWindow::setVSync(bool enabled)
//	{
//		//DN_PROFILE_FUNCTION();
//		graphics_ctx_->setVsync(enabled);
//
//		win_data_.is_vsync_ = enabled;
//	}
//
//	bool OpenGLWindow::isVSync() const
//	{
//		return win_data_.is_vsync_;
//	}
//
//	GraphicsContext* OpenGLWindow::getGraphicsContext()
//	{
//		return graphics_ctx_;
//	}
//
//	void OpenGLWindow::initializeGL()
//	{
//		DN_CORE_INFO("initializeGL");
//		//init(win_props_);
//	}
//
//	void OpenGLWindow::paintGL()
//	{
//		DN_CORE_INFO("paintGL");
//	}
//
//	void OpenGLWindow::resizeGL(int width, int height)
//	{
//		DN_CORE_INFO("resizeGL {0}, {1}", width, height);
//	}
//}
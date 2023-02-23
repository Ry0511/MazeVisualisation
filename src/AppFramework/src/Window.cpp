//
// Header File: Window.cpp
// Date       : 16/01/2023
// Project    : MazeVisualisation
// Author     : -Ry
//

#include "Window.h"

#include "Logging.h"
#include "Renderer/GLUtil.h"

#include <exception>

namespace app {

    #define GET_SELF(x) static_cast<Window*>(glfwGetWindowUserPointer(x))

    size_t Window::s_ActiveWindowCount = 0;

    //############################################################################//
    // | WINDOW CREATION & DELETION |
    //############################################################################//

    Window::Window(
            const char* title,
            int width,
            int height
    ) : m_WindowState({ .size = { width, height }}) {

        if (glfwInit() == GLFW_FALSE) {
            ERR("GLFW was not initialised...");
            throw std::exception();
        }

        glfwWindowHint(GLFW_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_VERSION_MINOR, 3);

        if (s_ActiveWindowCount == 0) {
            glfwSetErrorCallback([](int code, const char* msg) {
                ERR("GLFW_ERROR: Code='{}', Message='{}'", code, msg);
            });
        }

        m_Handle = glfwCreateWindow(width, height, title, nullptr, nullptr);
        glfwSetWindowUserPointer(m_Handle, this);

        if (!m_Handle) {
            ERR(
                    "Window Creation Failed args:"
                    "(Width='{}', Height='{}', Title='{}')",
                    width, height, title
            );
            glfwTerminate();
            throw std::exception();
        } else {
            ++s_ActiveWindowCount;
            impl_bind_callbacks();
        }

        INFO(
                "Window Created: Ref='{:#0x}', Window Count='{}'",
                (uintptr_t) m_Handle, s_ActiveWindowCount
        );
    }

    Window::~Window() {
        INFO(
                "Destroying Window: Ref='{:#0x}', Window Count='{}'",
                (uintptr_t) m_Handle, s_ActiveWindowCount - 1
        );
        glfwDestroyWindow(m_Handle);
        --s_ActiveWindowCount;

        if (s_ActiveWindowCount == 0) {
            INFO("Terminating GLFW...");
            glfwTerminate();
        }
    }

    void Window::impl_bind_callbacks() {
        glfwSetWindowSizeCallback(m_Handle, [](auto ref, int w, int h) {
            GET_SELF(ref)->m_WindowState.size = glm::ivec2{ w, h };
        });

        glfwSetWindowPosCallback(m_Handle, [](auto ref, int x, int y) {
            GET_SELF(ref)->m_WindowState.pos = glm::ivec2{ x, y };
        });

        glfwSetCursorPosCallback(m_Handle, [](auto ref, double x, double y) {
            auto self = GET_SELF(ref);
            self->m_WindowState.mouse_pos = glm::vec2{ x, y };
            if (self->m_IsCursorLocked) {
                glm::vec2 pos = self->m_WindowState.size / 2;
                glfwSetCursorPos(self->m_Handle, pos.x, pos.y);
            }
        });

        glfwSetScrollCallback(m_Handle, [](auto ref, double x, double y) {
            GET_SELF(ref)->m_WindowState.scroll_pos = glm::vec2{ x, y };
        });

        glfwSetKeyCallback(m_Handle, [](auto ref, int key, int scan, int act, int mods) {
            GET_SELF(ref)->m_WindowState.button_mods = mods;
        });
    }

    //############################################################################//
    // | GETTERS |
    //############################################################################//

    GLFWwindow* Window::get_handle() {
        return m_Handle;
    }

    const WindowState& Window::get_window_state() const {
        return m_WindowState;
    }

    glm::ivec2 Window::get_window_size() const {
        return m_WindowState.size;
    }

    glm::ivec2 Window::get_window_pos() const {
        return m_WindowState.pos;
    }

    glm::vec2 Window::get_mouse_pos() const {
        return m_WindowState.mouse_pos;
    }

    glm::vec2 Window::get_scroll_pos() const {
        return m_WindowState.scroll_pos;
    }

    int Window::get_button_mods() const {
        return m_WindowState.button_mods;
    }

    //############################################################################//
    // | WINDOW OPERATIONS |
    //############################################################################//

    void Window::show() {
        glfwShowWindow(m_Handle);
    }

    void Window::hide() {
        glfwHideWindow(m_Handle);
    }

    void Window::close() {
        glfwSetWindowShouldClose(m_Handle, GLFW_TRUE);
    }

    bool Window::should_close() {
        return glfwWindowShouldClose(m_Handle) == GLFW_TRUE;
    }

    void Window::make_context_current() {
        glfwMakeContextCurrent(m_Handle);
        if (glewInit() != GLEW_OK) {
            ERR("GLEW Failed to initialise...");
            throw std::exception();
        } else {
            auto* gl_version = (const char*) GL(glGetString(GL_VERSION));
            INFO("GLEW {} Initialised; OpenGL Version: {}",
                 (const char*) glewGetString(GLEW_VERSION),
                 gl_version
            );
        }
    }

    void Window::swap_buffers() {
        glfwSwapBuffers(m_Handle);
    }

    void Window::set_title(const char* title) {
        glfwSetWindowTitle(m_Handle, title);
    }

    void Window::set_size(glm::ivec2 size) {
        glfwSetWindowSize(m_Handle, size.x, size.y);
    }

    void Window::set_pos(glm::ivec2 pos) {
        glfwSetWindowPos(m_Handle, pos.x, pos.y);
    }

    bool Window::is_key_pressed(Key key) {
        int state = glfwGetKey(m_Handle, static_cast<int>(key));
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Window::is_mouse_pressed(MouseButton button) {
        int state = glfwGetMouseButton(m_Handle, static_cast<int>(button));
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    glm::vec2 Window::get_mouse_pos() {
        glm::dvec2 pos{};
        glfwGetCursorPos(m_Handle, &pos.x, &pos.y);
        return glm::vec2{ pos.x, pos.y };
    }

    glm::vec2 Window::get_mouse_vel(glm::vec2 sens) {
        glm::vec2 pos = get_mouse_pos();
        auto      vel = (m_WindowState.mouse_pos - pos) * sens;
        return vel;
    }

    void Window::lock_cursor_to_centre(bool is_cursor_locked) {
        m_IsCursorLocked = is_cursor_locked;
    }

    void Window::compute_key_state(bool* states, std::initializer_list<Key> keys) {
        size_t i = 0;
        for (auto key : keys) {
            states[i]  = is_key_pressed(key);
            ++i;
        }
    }

    bool Window::is_modifier_set(Modifier mod) {
        return has_modifier(m_WindowState.button_mods, mod);
    }

} // app
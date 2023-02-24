//
// Header File: Window.h
// Date       : 16/01/2023
// Project    : MazeVisualisation
// Author     : -Ry
//

#ifndef MAZEVISUALISATION_WINDOW_H
#define MAZEVISUALISATION_WINDOW_H

#include <gl/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <functional>
#include <queue>

namespace app {

    //############################################################################//
    // | BUTTON ENUMERATIONS |
    //############################################################################//

    // Regex Search & Replace
    enum class Key : int {
        Q     = GLFW_KEY_Q, W = GLFW_KEY_W,
        E     = GLFW_KEY_E, R = GLFW_KEY_R,
        T     = GLFW_KEY_T, Y = GLFW_KEY_Y,
        U     = GLFW_KEY_U, I = GLFW_KEY_I,
        O     = GLFW_KEY_O, P = GLFW_KEY_P,
        A     = GLFW_KEY_A, S = GLFW_KEY_S,
        D     = GLFW_KEY_D, F = GLFW_KEY_F,
        G     = GLFW_KEY_G, H = GLFW_KEY_H,
        J     = GLFW_KEY_J, K = GLFW_KEY_K,
        L     = GLFW_KEY_L, Z = GLFW_KEY_Z,
        X     = GLFW_KEY_X, C = GLFW_KEY_C,
        V     = GLFW_KEY_V, B = GLFW_KEY_B,
        N     = GLFW_KEY_N, M = GLFW_KEY_M,
        NUM_1 = GLFW_KEY_1, NUM_2 = GLFW_KEY_2,
        NUM_3 = GLFW_KEY_3, NUM_4 = GLFW_KEY_4,
        NUM_5 = GLFW_KEY_5, NUM_6 = GLFW_KEY_6,
        NUM_7 = GLFW_KEY_7, NUM_8 = GLFW_KEY_8,
        NUM_9 = GLFW_KEY_9, NUM_0 = GLFW_KEY_0,
        FN_1  = GLFW_KEY_F1, FN_2 = GLFW_KEY_F2,
        FN_3  = GLFW_KEY_F3, FN_4 = GLFW_KEY_F4,
        FN_5  = GLFW_KEY_F5, FN_6 = GLFW_KEY_F6,
        FN_7  = GLFW_KEY_F7, FN_8 = GLFW_KEY_F8,
        FN_9  = GLFW_KEY_F9, FN_10 = GLFW_KEY_F10,
        FN_11 = GLFW_KEY_F11, FN_12 = GLFW_KEY_F12,

        SPACE  = GLFW_KEY_SPACE,
        COMMA  = GLFW_KEY_COMMA,
        PERIOD = GLFW_KEY_PERIOD,
        SLASH  = GLFW_KEY_SLASH,
        BSLASH = GLFW_KEY_BACKSLASH
    };

    enum class MouseButton : int {
        MOUSE_LEFT   = GLFW_MOUSE_BUTTON_LEFT,
        MOUSE_RIGHT  = GLFW_MOUSE_BUTTON_RIGHT,
        MOUSE_MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE,
        MOUSE_4      = GLFW_MOUSE_BUTTON_4,
        MOUSE_5      = GLFW_MOUSE_BUTTON_5
    };

    enum class Modifier : int {
        CTRL  = GLFW_MOD_CONTROL,
        SHIFT = GLFW_MOD_SHIFT,
        ALT   = GLFW_MOD_ALT,
        TAB   = GLFW_MOD_ALT
    };

    static constexpr bool has_modifier(int mod_value, Modifier mod) {
        return (mod_value & static_cast<int>(mod)) != 0;
    }

    static constexpr bool has_modifiers(int mod_value, std::initializer_list<Modifier> mods) {
        int expected = 0;

        for (const Modifier mod : mods) expected |= static_cast<int>(mod);
        return expected == mod_value;
    }

    //############################################################################//
    // | WINDOW RUNTIME STATE |
    //############################################################################//

    struct WindowState {
        glm::ivec2                                         size        = { 0, 0 };
        glm::ivec2                                         pos         = { 0, 0 };
        glm::vec2                                          mouse_pos   = { 0, 0 };
        glm::vec2                                          scroll_pos  = { 0, 0 };
        int                                                button_mods = 0;
        std::unordered_map<int, std::tuple<int, int, int>> key_state_map{};
    };

    //############################################################################//
    // | GLFW WINDOW WRAPPER |
    //############################################################################//

    class Window {

    public:
        static void poll_events() {
            glfwPollEvents();
        }

    private:
        static size_t s_ActiveWindowCount;

    private:
        GLFWwindow* m_Handle = nullptr;
        WindowState m_WindowState    = {};
        bool        m_IsCursorLocked = false;

    public:
        explicit Window(const char* title, int width, int height);
        ~Window();

    private:
        void impl_bind_callbacks();

    public:
        GLFWwindow* get_handle();
        const WindowState& get_window_state() const;

    public:
        glm::ivec2 get_window_size() const;
        glm::ivec2 get_window_pos() const;
        glm::vec2 get_mouse_pos() const;
        glm::vec2 get_scroll_pos() const;
        int get_button_mods() const;

    public:
        void show();
        void hide();
        void close();
        bool should_close();
        void make_context_current();
        void swap_buffers();
        void set_title(const char* title);
        void set_size(glm::ivec2 size);
        void set_pos(glm::ivec2 pos);
        void lock_cursor_to_centre(bool is_cursor_locked = true);
        glm::vec2 get_mouse_pos();
        glm::vec2 get_mouse_vel(glm::vec2 sens = glm::vec2{ 0.02, 0.02 });
        bool is_key_pressed(Key key);
        bool is_key_down(Key key);
        void compute_key_state(bool* states, std::initializer_list<Key> keys);
        bool is_modifier_set(Modifier mod);
        bool is_mouse_pressed(MouseButton button);
    };

} // app

#endif

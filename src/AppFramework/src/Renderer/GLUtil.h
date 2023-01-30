//
// Created by -Ry on 22/01/2023.
//

#ifndef MAZEVISUALISATION_GLUTIL_H
#define MAZEVISUALISATION_GLUTIL_H

#include "Logging.h"
#include <gl/glew.h>
#include <string>
#include <optional>

namespace app {

    //############################################################################//
    // | STATIC FUNCTIONS |
    //############################################################################//

    static std::optional<std::string> flush_gl_errors() {
        std::string   buffer{};
        buffer.reserve(6 * 3);

        while (GLenum error = glGetError()) {
            buffer.append(std::format("{:#06x} ", error));
        }

        if (buffer.empty()) {
            return std::nullopt;
        } else {
            return std::optional(buffer);
        }
    }

    static void check_gl_error(
            const char* code,
            const char* file,
            const int line,
            const char* function
    ) {

        auto error_str = flush_gl_errors();
        if (error_str.has_value()) {
            ERR(
                    "{:-<80}"
                    "\n\t#ERROR/S : {}"
                    "\n\t#File    : {}"
                    "\n\t#Function: {}"
                    "\n\t#Line    : {}"
                    "\n\t#Snippet : {}"
                    "\n{:-<100}",
                    "",
                    error_str.value(),
                    file,
                    function,
                    line,
                    code,
                    ""
            );

            PANIC;
        }
    }

    //############################################################################//
    // | MACROS |
    //############################################################################//

    #ifdef Debug

    #define GL(x) x; app::check_gl_error(#x, __FILE__, __LINE__, __FUNCTION__)

    #else
    #define GL(x) x
    #endif

}

#endif //MAZEVISUALISATION_GLUTIL_H

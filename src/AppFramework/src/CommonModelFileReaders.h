//
// Created by -Ry on 05/02/2023.
//

#ifndef MAZEVISUALISATION_COMMONMODELFILEREADERS_H
#define MAZEVISUALISATION_COMMONMODELFILEREADERS_H

#include "Logging.h"
#include "Renderer/VertexObjectBinding.h"

#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iterator>

namespace app::model_file {

    //############################################################################//
    // | WAVEFRONT (.OBJ) | SOURCE: http://www.martinreddy.net/gfx/3d/OBJ.spec
    //############################################################################//

    static void read_wavefront_file(const std::string& file, Mutable3DModel& model) {
        std::ifstream file_stream(file);

        if (!file_stream.is_open() || !file_stream.good()) {
            HERR("[WFL_READ]", " # Failed to read file '{}'", file);
            throw std::exception();
        }

        using Iter = std::istreambuf_iterator<char>;
        std::stringstream file_content_stream{ std::string(Iter(file_stream), Iter()) };

        char line_buffer[256]{};
        while (!file_content_stream.eof()) {
            file_content_stream.getline(line_buffer, 256);
            std::stringstream line_stream;
            line_stream << line_buffer;
            line_stream.unsetf(std::ios_base::skipws);

            char id[2]{};
            line_stream >> id[0] >> id[1];
            line_stream.setf(std::ios_base::skipws);

            // Vertex
            if (id[0] == 'v') {
                float x = 0.0, y = 0.0, z = 0.0;
                if (!(line_stream >> x >> y >> z)) line_stream.clear();

                switch (id[1]) {
                    case ' ': {
                        model.add_vert(x, y, z);
                        break;
                    }
                    case 'n': {
                        model.add_normal(x, y, z);
                        break;
                    }
                    case 't': {
                        model.add_tex_pos(x, y, z);
                        break;
                    }
                }

                // Face (Assuming triangular mesh) Format: 'file_stream v/n/vt'
            } else if (id[0] == 'f' && id[1] == ' ') {
                Index j = 0, k = 0, l = 0;
                char  dump{};

                line_stream >> std::skipws;
                for (int i = 0; i < 3; ++i) {
                    if (line_stream >> j >> dump >> k >> dump >> l) {
                        // f v/vt/vn
                        model.add_vertex_index(j - 1, l - 1, k - 1);
                    } else {
                        HWARN("[OBJ_READ]", " # Reading Indices from {} may have failed...", file);
                        line_stream.clear();
                    }
                }
            }
        }
        file_stream.close();
    }

}

#endif //MAZEVISUALISATION_COMMONMODELFILEREADERS_H

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

    static void read_wavefront_file(const std::string& file, Mutable3DModel& model) {
        std::ifstream file_stream(file);

        if (!file_stream.is_open() || !file_stream.good()) {
            HERR("[WFL_READ]", " # Failed to read file '{}'", file);
            throw std::exception();
        }

        using Iter = std::istreambuf_iterator<char>;
        std::stringstream file_content_stream{ std::string(Iter(file_stream), Iter()) };

        std::string line{};
        while (!file_content_stream.eof()) {
            std::getline(file_content_stream, line);
            std::stringstream line_stream{ line };

            char id[2]{};
            line_stream >> id;

            if (id[0] == 'v') {
                glm::vec3 vec{};
                line_stream >> vec.x >> vec.y >> vec.z;
                switch (id[1]) {
                    case ' ': {
                        model.add_vert(vec);
                    }
                    case 'n': {
                        model.add_normal(vec);
                    }
                    case 't': {
                        model.add_tex_pos(vec);
                    }
                }

                // Face (Assuming triangular mesh) Format: 'file_stream v/n/vt'
            } else if (id[0] == 'f' && id[1] == ' ') {
                glm::ivec3 vec{};
                char       dump{};
                line_stream >> vec.x >> dump >> vec.y >> dump >> vec.z;
            }
        }
        file_stream.close();
    }

}

#endif //MAZEVISUALISATION_COMMONMODELFILEREADERS_H

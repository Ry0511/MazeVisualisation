//
// Created by -Ry on 05/02/2023.
//

#ifndef MAZEVISUALISATION_WAVEFRONTFILE_H
#define MAZEVISUALISATION_WAVEFRONTFILE_H

#include "Logging.h"
#include "Renderer/VertexObjectBinding.h"

#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iterator>

namespace app {

    class WavefrontLoader {

    private:
        app::TriangularMeshModel m_Model{};

    public:
        explicit WavefrontLoader(const std::string& file) {
            std::ifstream f(file);

            if (!f.is_open() || !f.good()) {
                HERR("[WFL_READ]", " # Failed to read file '{}'", file);
                throw std::exception();
            }

            using Iter = std::istreambuf_iterator<char>;
            std::stringstream file_content_stream{ std::string(Iter(f), Iter()) };

            std::string line{};
            while (!file_content_stream.eof()) {
                std::getline(file_content_stream, line);
                std::stringstream line_stream{line};

                char id[2]{};
                line_stream >> id;

                // Vertex
                if (id[0] == 'v') {
                    glm::vec3 vertex{};

                    // Read 3 floats
                    if (id[1] == ' ' || id[1] == 'n') {
                        line_stream >> vertex.x >> vertex.y >> vertex.z;
                    }

                    if (id[1] == ' ') m_Model.add_vertex(vertex);
                    if (id[1] == 'n') m_Model.add_normal(vertex);


                    // Face
                } else if (id[0] == 'f' && id[1] == ' ') {

                }

            }

        }

    public:

    };

}

#endif //MAZEVISUALISATION_WAVEFRONTFILE_H

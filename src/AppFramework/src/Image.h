//
// Header File: Image.h
// Date       : 18/03/2023
// Project    : MazeVisualisation
// Author     : -Ry
//

#ifndef MAZEVISUALISATION_IMAGE_H
#define MAZEVISUALISATION_IMAGE_H

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#include <cstdint>
#include <vector>
#include <string>

namespace app {

    class Image {

    public:
        using PixelType = unsigned char;

    private:
        int                  m_Width;
        int                  m_Height;
        int                  m_ChannelCount;
        std::vector<PixelType> m_PixelBuffer;

        //############################################################################//
        // | CONSTRUCTORS |
        //############################################################################//

    public:
        Image(const std::string& file);
        Image(const Image&);
        Image(Image&&);

        //############################################################################//
        // | ASSIGNMENT OPERATORS |
        //############################################################################//

    public:
        Image& operator=(const Image& other);
        Image& operator=(Image&& other);

        //############################################################################//
        // | GETTERS |
        //############################################################################//

    public:
        int get_width() const {
            return m_Width;
        };

        int get_height() const {
            return m_Height;
        };

        int get_channel_count() const {
            return m_ChannelCount;
        }

        size_t get_size() const {
            return m_Width * m_Height * m_ChannelCount;
        }

        const std::vector<PixelType> get_pixel_buffer() const {
            return m_PixelBuffer;
        }

        const PixelType* get_pixel_data() const {
            return m_PixelBuffer.data();
        }

    };

} // app

#endif

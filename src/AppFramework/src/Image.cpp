//
// Header File: Image.cpp
// Date       : 18/03/2023
// Project    : MazeVisualisation
// Author     : -Ry
//

#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Logging.h"

namespace app {

    //############################################################################//
    // | CONSTRUCTORS |
    //############################################################################//

    Image::Image(
            const std::string& file
    ) : m_Width(),
        m_Height(),
        m_ChannelCount(),
        m_PixelBuffer() {

        // Load the Image
        PixelType* buffer = stbi_load(file.c_str(), &m_Width, &m_Height, &m_ChannelCount, 0);
        ASSERT(
                m_Width != 0 && m_Height != 0 && m_ChannelCount != 0,
                "Width, Height, or Channel Count should not be Zero..."
        );

        // Buffer is nullptr
        if (!buffer) {
            HERR("[IMAGE]", " # STBI_LOAD returned a nullptr for input file: '{}'", file);
            throw std::exception();
        }

        // Allocate into Vector
        m_PixelBuffer.resize(m_Width * m_Height * m_ChannelCount);
        for (size_t i = 0; i < m_PixelBuffer.size(); ++i) {
            m_PixelBuffer[i] = buffer[i];
        }

        // Just calls free(buffer)
        stbi_image_free(buffer);
    }

    Image::Image(
            const Image& other
    ) : m_Width(other.m_Width),
        m_Height(other.m_Height),
        m_ChannelCount(other.m_ChannelCount),
        m_PixelBuffer(other.m_PixelBuffer) {
    }

    Image::Image(
            Image&& other
    ) : m_Width(other.m_Width),
        m_Height(other.m_Height),
        m_ChannelCount(other.m_ChannelCount),
        m_PixelBuffer(std::move(other.m_PixelBuffer)) {
    }

    //############################################################################//
    // | ASSIGNMENT OPERATORS |
    //############################################################################//

    Image& Image::operator =(const Image& other) {
        m_Width = other.m_Width;
        m_Height = other.m_Height;
        m_ChannelCount = other.m_ChannelCount;
        m_PixelBuffer = other.m_PixelBuffer;
        return *this;
    }

    Image& Image::operator =(Image&& other) {
        m_Width = other.m_Width;
        m_Height = other.m_Height;
        m_ChannelCount = other.m_ChannelCount;
        m_PixelBuffer = std::move(other.m_PixelBuffer);
        return *this;
    }

} // app
//
// Created by -Ry on 08/02/2023.
//

#ifndef MAZEVISUALISATION_MAZECONTROLLER_H
#define MAZEVISUALISATION_MAZECONTROLLER_H

#include "Logging.h"

#include <glm/glm.hpp>

#include <vector>
#include <cstdint>
#include <random>

namespace maze {

    // TODO: Implement this.

    //############################################################################//
    // | GLOBALLY USED ALIAS |
    //############################################################################//

    using MazeCell = uint32_t;
    using MazeSizeType = unsigned short;
    using Distribution = std::uniform_int_distribution<MazeCell>;

    //############################################################################//
    // | MAZE CELL FLAGS |
    //############################################################################//

    enum class MazeCellFlags : MazeCell {
        // Cell Type
        EMPTY          = 0x1 << 0,
        WALL           = 0x1 << 1,
        DOOR           = 0x1 << 2,
        // Solid Colours
        RED            = 0x1 << 3,
        GREEN          = 0x1 << 4,
        BLUE           = 0x1 << 5,
        // Transition Effects
        FADE_NEAR      = 0x1 << 6,
        TRANSLATE_UP   = 0x1 << 7,
        TRANSLATE_DOWN = 0x1 << 8,
        SCALE_OUT      = 0x1 << 9,
        ROTATE         = 0x1 << 10,
        // Texture Type
        BRICK          = 0x1 << 11,
        STONE          = 0x1 << 12,
        CHECKERED      = 0x1 << 13,
        // State Flags
        MODIFIED       = 0x1 << 14
    };

    static inline constexpr size_t s_CellFlagCount = 15;

    using CellFlag = MazeCellFlags;

    template<CellFlag Flag>
    static inline constexpr MazeCell cellof() {
        return static_cast<MazeCell>(Flag);
    }

    template<CellFlag Flag>
    static inline constexpr bool check_flag(MazeCell cell) {
        return (cell & static_cast<MazeCell>(Flag)) != 0;
    }

    template<CellFlag... Flags>
    static inline constexpr bool check_flags(MazeCell cell) {
        static_assert(sizeof...(Flags) != 0, "Atleast one flag is required...");
        return (check_flag<Flags>(cell) && ...);
    }

    template<CellFlag... Flags>
    static inline constexpr MazeCell combine_flags() {
        if (sizeof...(Flags) == 0U) return 0U;
        return (static_cast<MazeCell>(Flags) | ... | 0U);
    }

    static inline CellFlag get_flag(MazeCell index) {
        if (index > s_CellFlagCount) {
            HERR("[GET_FLAG]", " # Index {} > {}...", index, s_CellFlagCount);
            throw std::exception();
        }
        return static_cast<CellFlag>(0x1 << index);
    }

    //############################################################################//
    // | MUTABLE MAZE TYPE |
    //############################################################################//

    class MutableMaze {

    private:
        size_t                m_Width;
        size_t                m_Height;
        Distribution          m_WidthDistribution;
        Distribution          m_HeightDistribution;
        std::vector<MazeCell> m_MazeBuffer;

        //############################################################################//
        // | CONSTRUCTION |
        //############################################################################//

    public:
        MutableMaze(
                MazeSizeType width,
                MazeSizeType height
        ) : m_Width(width),
            m_WidthDistribution(Distribution{ 0, width }),
            m_Height(height),
            m_HeightDistribution(Distribution{ 0, height }),
            m_MazeBuffer(std::vector<MazeCell>(width * height, cellof<MazeCellFlags::WALL>())) {

            if (m_MazeBuffer.size() == 0) {
                HERR("[MUTABLE_MAZE]", " # Maze Size cannot be 0. ( {}*{} (0) )", width, height);
                throw std::exception();
            }

            HINFO(
                    "[MUTABLE_MAZE]", " # CREATE: {}x{} ({})",
                    m_Width, m_Height, m_MazeBuffer.size()
            );
        }

        MutableMaze(
                const MutableMaze& maze
        ) : m_MazeBuffer(maze.m_MazeBuffer),
            m_Width(maze.m_Width),
            m_Height(maze.m_Height) {
            HINFO(
                    "[MUTABLE_MAZE]", " # COPY: {}x{} ({})",
                    m_Width, m_Height, m_MazeBuffer.size()
            );
        };

        MutableMaze(
                MutableMaze&& maze
        ) : m_MazeBuffer(std::move(maze.m_MazeBuffer)),
            m_Width(maze.m_Width),
            m_Height(maze.m_Height) {
            HINFO(
                    "[MUTABLE_MAZE]", " # MOVE: {}x{} ({})",
                    m_Width, m_Height, m_MazeBuffer.size()
            );
        };

        //############################################################################//
        // | GET DATA |
        //############################################################################//

    public:
        const MazeCell* get_data() const {
            return m_MazeBuffer.data();
        }

        short get_width() const {
            return m_Width;
        }

        const Distribution& get_width_distribution() const {
            return m_WidthDistribution;
        }

        short get_height() const {
            return m_Height;
        }

        const Distribution& get_height_distribution() const {
            return m_HeightDistribution;
        }

        size_t get_cell_count() const {
            return m_MazeBuffer.size();
        }

        MazeCell& get_cell(size_t row, size_t col) {
            check_index(row, col);
            return m_MazeBuffer[(row * m_Height) + col];
        }

        MazeCell get_cell(size_t row, size_t col) const {
            check_index(row, col);
            return m_MazeBuffer[(row * m_Height) + col];
        }

        template<CellFlag Flag>
        void set_flag(size_t row, size_t col) {
            get_cell(row, col) |= static_cast<MazeCell>(Flag);
        }

        template<CellFlag... Flags>
        void set_flags(size_t row, size_t col) {
            (set_flag<Flags>(row, col), ...);
        }

        template<CellFlag Flag>
        void unset_flag(size_t row, size_t col) {
            get_cell(row, col) |= ~static_cast<MazeCell>(Flag);
        }

        template<CellFlag... Flags>
        void unset_flags(size_t row, size_t col) {
            (unset_flag<Flags>(row, col), ...);
        }

        template<CellFlag Flag>
        bool check_flag(size_t row, size_t col) const {
            return (get_cell(row, col) & static_cast<MazeCell>(Flag)) != 0;
        }

        template<CellFlag... Flags>
        bool check_flags(size_t row, size_t col) const {
            return (check_flag<Flags>(row, col), ...);
        }

        //############################################################################//
        // | UTILITY |
        //############################################################################//

    public:

        bool is_inbounds(size_t row, size_t col) const {
            return row < m_Height && col < m_Width;
        }

        void check_index(size_t row, size_t col) const {
            if (!is_inbounds(row, col)) {
                HERR(
                        "[MUTABLE_MAZE]",
                        " # Index: [{}][{}] is out of bounds.",
                        row,
                        col,
                        m_MazeBuffer.size()
                );
                throw std::exception();
            }
        }

        auto begin() {
            return m_MazeBuffer.begin();
        }

        auto end() {
            return m_MazeBuffer.end();
        }

        template<class Function>
        void for_each(Function fn) {
            for (size_t r = 0; r < m_Height; ++r) {
                for (size_t c = 0; c < m_Width; ++c) {
                    fn(r, c, get_cell(r, c));
                }
            }
        }
    };

    using MazePtr = std::unique_ptr<MutableMaze>;

    //############################################################################//
    // | UTILITY 2D ARRAY INDEX |
    //############################################################################//

    class Index2D {
    private:
        size_t m_Row;
        size_t m_Col;

    public:
        constexpr Index2D() : m_Row(0), m_Col(0) {};
        constexpr Index2D(size_t row, size_t col) : m_Row(row), m_Col(col) {};

    public:
        template<class T>
        constexpr operator T() const {
            return T(m_Row, m_Col);
        }

    public:
        inline size_t get_row() const {
            return m_Row;
        }

        inline size_t row() const {
            return m_Row;
        }

        inline size_t& get_row() {
            return m_Row;
        }

        inline size_t get_col() const {
            return m_Col;
        }

        inline size_t col() const {
            return m_Col;
        }

        inline size_t& get_col() {
            return m_Col;
        }

    public:
        bool advance(size_t row_max, size_t col_max) {
            if (m_Col < col_max - 1) {
                ++m_Col;
            } else if (m_Row < row_max - 1) {
                ++m_Row;
                m_Col = 0;
            } else {
                return false;
            }
            return true;
        }

    public:
        void operator +=(const Index2D& offset) {
            m_Row += offset.m_Row;
            m_Col += offset.m_Col;
        }

        constexpr Index2D operator +(const Index2D& offset) {
            return Index2D(m_Row + offset.m_Row, m_Col + offset.m_Col);
        }

        void operator -=(const Index2D& offset) {
            m_Row -= offset.m_Row;
            m_Col -= offset.m_Col;
        }

        constexpr Index2D operator -(const Index2D& offset) {
            return Index2D(m_Row - offset.m_Row, m_Col - offset.m_Col);
        }

        constexpr Index2D operator -() const {
            return Index2D(-m_Row, -m_Col);
        }

        constexpr Index2D negate() const {
            return Index2D(-m_Row, -m_Col);
        }

        constexpr bool operator ==(const Index2D& rhs) const {
            return m_Row == rhs.m_Row && m_Col == rhs.m_Col;
        }
    };

    //############################################################################//
    // | UTILITY CARDINAL WRAPPER |
    //############################################################################//

    enum class Cardinal : char {
        NORTH = 'N',
        EAST  = 'E',
        SOUTH = 'S',
        WEST  = 'W'
    };

    template<Cardinal Dir>
    static constexpr Index2D cardinal_offset() {
        switch (Dir) {
            case Cardinal::NORTH:
                return Index2D(0, -1);
            case Cardinal::SOUTH:
                return Index2D(0, 1);
            case Cardinal::EAST:
                return Index2D(1, 0);
            case Cardinal::WEST:
                return Index2D(-1, 0);
        }
    }

    template<Cardinal Dir>
    static constexpr Index2D reverse() {
        return cardinal_offset<Dir>().negate();
    }

    //############################################################################//
    // | UTILITY GRID NAVIGATION CLASS |
    //############################################################################//

    class GridNavigator {

    private:
        Index2D m_Index;
        MutableMaze& m_Maze;

    public:
        GridNavigator(
                size_t row,
                size_t col,
                MutableMaze& maze
        ) : m_Index(row, col),
            m_Maze(maze) {
            HINFO("[GRID_NAVIGATOR]", " # Create: ( {}, {} )", row, col);
        }

        GridNavigator(const GridNavigator&) = delete;
        GridNavigator(GridNavigator&&) = delete;

    public:
        size_t get_row() const {
            return m_Index.get_row();
        }

        size_t get_col() const {
            return m_Index.get_col();
        }

        MutableMaze& get_maze() {
            return m_Maze;
        }

        //############################################################################//
        // | VALIDATION & NAVIGATION |
        //############################################################################//

    public:

        template<Cardinal Dir>
        bool can_travel() {
            Index2D offset = m_Index + cardinal_offset<Dir>();
            return m_Maze.is_inbounds(offset.get_row(), offset.get_col());
        }

        template<Cardinal Dir>
        void travel() {
            m_Index += cardinal_offset<Dir>();
        }

        //############################################################################//
        // | APPLY DATA |
        //############################################################################//

    public:

        MazeCell& get_cell() {
            return m_Maze.get_cell(get_row(), get_col());
        }

        MazeCell get_cell() const {
            return m_Maze.get_cell(get_row(), get_col());
        }

        template<CellFlag Flag>
        void set_flag() {
            m_Maze.set_flag<Flag>();
        }

        template<CellFlag... Flags>
        void set_flags() {
            m_Maze.set_flags<Flags...>();
        }

        template<CellFlag Flag>
        void unset_flag() {
            m_Maze.unset_flag<Flag>();
        }

        template<CellFlag... Flags>
        void unset_flags() {
            m_Maze.unset_flags<Flags...>();
        }

        template<CellFlag Flag>
        bool check_flag() const {
            return m_Maze.check_flag<Flag>();
        }

        template<CellFlag... Flags>
        bool check_flags() const {
            return m_Maze.check_flags<Flags...>();
        }

    };

    //############################################################################//
    // | FACTORY FOR GENERATING MAZES |
    //############################################################################//

    class MazeBuilderFactory {
    private:
        static inline std::mt19937 s_RandomGenerator = {};

    protected:
        bool m_IsFinished = false;

    public:
        static std::mt19937& get_rng() {
            return s_RandomGenerator;
        }

        virtual ~MazeBuilderFactory() = default;

    public:
        virtual void init(MutableMaze& mutable_maze) = 0;
        virtual Index2D step(MutableMaze& mutable_maze) = 0;
        virtual bool is_complete() {
            return m_IsFinished;
        };
    };

    using MazeFactoryPtr = std::unique_ptr<MazeBuilderFactory>;

    //############################################################################//
    // | PRIMITIVE RNG 'MAZE' |
    //############################################################################//

    class TrulyRandomMazeImpl : public MazeBuilderFactory {
    private:
        Index2D      m_Index                = Index2D(0, 0);
        Distribution m_RandomCellTypeDist   = Distribution(0, 2);
        Distribution m_RandomCellColourDist = Distribution(3, 5);

    public:
        virtual void init(MutableMaze& mutable_maze) override {
            HINFO("[RAND_MAZE]", " # Initialise...");
        }

        virtual Index2D step(MutableMaze& maze) override {
            if (m_IsFinished) return m_Index;
            MazeCell& cell = maze.get_cell(m_Index.get_row(), m_Index.get_col());
            cell &= ~cellof<MazeCellFlags::WALL>();

            cell |= static_cast<MazeCell>(get_flag(m_RandomCellTypeDist(get_rng())))
                    | static_cast<MazeCell>(get_flag(m_RandomCellColourDist(get_rng())))
                    | cellof<MazeCellFlags::MODIFIED>();

            if (!m_Index.advance(maze.get_height(), maze.get_width())) {
                m_IsFinished = true;
            }

            return m_Index;
        }
    };

    //############################################################################//
    // | RECURSIVE BACKTRACKER |
    //############################################################################//

    class RecursiveBacktrackImpl : public MazeBuilderFactory {

    private:
        Distribution m_Dist;

    public:
        virtual void init(MutableMaze& mutable_maze) override {

        }

        virtual Index2D step(MutableMaze& mutable_maze) override {

        }
    };
}

#endif //MAZEVISUALISATION_MAZECONTROLLER_H

//
// Created by -Ry on 08/02/2023.
//

#ifndef MAZEVISUALISATION_MAZECONSTRUCTS_H
#define MAZEVISUALISATION_MAZECONSTRUCTS_H

#include "Logging.h"
#include "Renderer/RendererHandlers.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <vector>
#include <cstdint>
#include <random>
#include <stack>

namespace maze {

    //############################################################################//
    // | GLOBAL ALIAS |
    //############################################################################//

    using Cell = uint32_t;
    using Index = int;
    using Distribution = std::uniform_int_distribution<Index>;

    #ifdef DETERMINISTIC
    using Random = std::mt19937;
    #else
    using Random = std::random_device;
    #endif


    struct Index2D {
        mutable Index row, col;

        constexpr Index flat(Index col_max) const {
            return row * col_max + col;
        }

        constexpr Index flat(const Index2D bounds) const {
            return row * bounds.col + col;
        }

        constexpr size_t size() const {
            return row * col;
        }

        constexpr bool operator >(const Index2D i) const { return i.row < row && i.col < col; }
        constexpr bool operator >=(const Index2D i) const { return i.row <= row && i.col <= col; }
        constexpr bool operator <(const Index2D i) const { return i.row > row && i.col > col; }
        constexpr bool operator <=(const Index2D i) const { return i.row >= row && i.col >= col; }

        constexpr bool operator ==(const Index2D i) const {
            return i.row == row && i.col == col;
        }

        constexpr Index2D operator -() const { return Index2D{ -row, -col }; }

        constexpr Index2D operator -(const Index2D o) const {
            return Index2D{ row - o.row, col - o.col };
        }

        constexpr Index2D operator +(const Index2D o) const {
            return Index2D{ row + o.row, col + o.col };
        }

        constexpr void operator =(const Index2D o) const {
            row = o.row;
            col = o.col;
        }

        constexpr bool inbounds(const Index row_max, const Index col_max) const {
            return row >= 0
                   && row < row_max
                   && col >= 0
                   && col < col_max;
        }

        constexpr bool inbounds(const Index2D bounds) const {
            return row >= 0
                   && row < bounds.row
                   && col >= 0
                   && col < bounds.col;
        }

        constexpr bool has_next(const Index row_max, const Index col_max) {
            return next(row_max, col_max) != *this;
        }

        constexpr Index2D next(const Index row_max, const Index col_max) const {
            if (col < col_max - 1) {
                return Index2D{ row, col + 1 };
            } else if (row < row_max - 1) {
                return Index2D{ row + 1, 0 };
            } else {
                return *this;
            }
        }

        Index2D abs() const {
            return Index2D{ std::abs(row), std::abs(col) };
        }

        std::string to_string() const {
            return std::format("( {},{} )", row, col);
        }

        struct Hasher {
            size_t operator ()(const Index2D& i) const noexcept {
                std::hash<Index> hasher{};
                return hasher(i.row) ^ (hasher(i.col) << 1);
            }
        };

        glm::vec3 to_vec() {
            return glm::vec3{ row, 0, col };
        }
    };

    //############################################################################//
    // | MAZE FLAGS |
    //############################################################################//

    enum class Flag : Cell {

        // Cell Type
        EMPTY_PATH = 1 << 0,
        PATH_NORTH = 1 << 1,
        PATH_EAST  = 1 << 2,
        PATH_SOUTH = 1 << 3,
        PATH_WEST  = 1 << 4,

        // Cell Visual State (Applies to all walls)
        RED   = 1 << 5,
        GREEN = 1 << 6,
        BLUE  = 1 << 7,

        // Cell State
        VISITED   = 1 << 11,
        INVALID   = 1 << 12,
        PROCESSED = 1 << 13,
        MODIFIED  = 1 << 14,
        FINISHED  = 1 << 15
    };

    inline static constexpr Cell s_FlagCount = 13;
    inline static Distribution   s_CellTypeDist{ 0, 4 };
    inline static Distribution   s_CellColourDist{ 5, 7 };
    inline static Distribution   s_CellTextureDist{ 8, 10 };

    static constexpr Flag flagof(const unsigned int index) {
        if (index >= s_FlagCount) {
            HERR("[FLAG_INDEX]", " # Flag index '{}' out of bounds...", index);
            throw std::exception();
        } else {
            return static_cast<Flag>(1 << index);
        }
    }

    template<Flag V>
    static constexpr Cell cellof() {
        return static_cast<Cell>(V);
    }

    static constexpr Cell cellof(const Flag flag) {
        return static_cast<Cell>(flag);
    }

    template<Flag F>
    static constexpr bool is_set(const Cell cell) {
        return (cell & cellof<F>()) != 0;
    }

    template<Flag F>
    static constexpr bool is_unset(const Cell cell) {
        return (cell & cellof<F>()) == 0;
    }

    template<Flag... Flags>
    static constexpr bool is_all_unset(const Cell cell) {
        return (... && ((cell & cellof<Flags>()) == 0));
    }

    static constexpr bool is_set(const Flag flag, const Cell cell) {
        return (cell & cellof(flag)) != 0;
    }

    static constexpr bool is_set(const std::initializer_list<Flag> flags, const Cell cell) {
        for (const Flag flag : flags) {
            if ((cell & cellof(flag)) == 0) return false;
        }
        return true;
    }


    static std::string eval_flags(const Cell cell) {
        std::string s{};
        if (is_set<Flag::PATH_NORTH>(cell)) s.append(" NORTH ");
        if (is_set<Flag::PATH_EAST>(cell)) s.append(" EAST  ");
        if (is_set<Flag::PATH_SOUTH>(cell)) s.append(" SOUTH ");
        if (is_set<Flag::PATH_WEST>(cell)) s.append(" WEST  ");
        return s;
    }

    //############################################################################//
    // | CARDINAL DIRECTION ENUMERATION |
    //############################################################################//

    enum class Cardinal : char {
        NORTH = 0,
        EAST  = 1,
        SOUTH = 2,
        WEST  = 3
    };

    static inline constexpr char                    s_CardinalCount = 4;
    static inline const Distribution                s_CardinalDist  = Distribution{ 0, 3 };
    static inline constexpr std::array<Cardinal, 4> s_AllCardinals{
            Cardinal::NORTH, Cardinal::EAST, Cardinal::SOUTH, Cardinal::WEST
    };

    static constexpr Cardinal get_cardinal(const char idx) {
        if (idx < 0 || idx >= s_CardinalCount) {
            HERR("[GET_CARDINAL]", " # Cardinal index '{}' is invalid...", idx);
            throw std::exception();
        }
        return static_cast<Cardinal>(idx);
    }

    static constexpr Index2D cardinal_offset(const Cardinal dir) {
        switch (dir) {
            case Cardinal::NORTH:
                return Index2D{ -1, 0 };
            case Cardinal::SOUTH:
                return Index2D{ 1, 0 };
            case Cardinal::EAST:
                return Index2D{ 0, 1 };
            case Cardinal::WEST:
                return Index2D{ 0, -1 };
            default:
                throw std::exception();
        }
    }

    static constexpr Index2D reverse(const Cardinal dir) {
        return -cardinal_offset(dir);
    }

    static std::string cardinal_to_string(const Cardinal dir) {
        switch (dir) {
            case Cardinal::NORTH:
                return "North";
            case Cardinal::EAST:
                return "East";
            case Cardinal::SOUTH:
                return "South";
            case Cardinal::WEST:
                return "West";
        }
    }

    static constexpr Flag path_flag_for_dir(const Cardinal dir) {
        switch (dir) {
            case Cardinal::NORTH:
                return Flag::PATH_NORTH;
            case Cardinal::SOUTH:
                return Flag::PATH_SOUTH;
            case Cardinal::EAST:
                return Flag::PATH_EAST;
            case Cardinal::WEST:
                return Flag::PATH_WEST;
            default:
                throw std::exception();
        }
    }

    static constexpr bool is_wall(const Cardinal dir, const Cell cell) {
        switch (dir) {
            case Cardinal::NORTH:
                return !is_set<Flag::PATH_NORTH>(cell);
            case Cardinal::EAST:
                return !is_set<Flag::PATH_EAST>(cell);
            case Cardinal::SOUTH:
                return !is_set<Flag::PATH_SOUTH>(cell);
            case Cardinal::WEST:
                return !is_set<Flag::PATH_WEST>(cell);
        }
        throw std::exception();
    }

    //############################################################################//
    // | ADJACENT CELLS UTILITY STRUCT |
    //############################################################################//

    struct AdjacentCells {
        inline static constexpr Cell s_Invalid = cellof<Flag::INVALID>();

        Cell cells[4]{ s_Invalid, s_Invalid, s_Invalid, s_Invalid };

        int valid_count() const {
            int count = 0;

            for (int j = 0; j < s_CardinalCount; ++j) {
                if (!is_set<Flag::INVALID>(cells[j])) ++count;
            }
            return count;
        }

        template<class Function>
        int count_where(Function fn) const {
            int count = 0;

            for (int j = 0; j < s_CardinalCount; ++j) {
                if (fn(cells[j])) ++count;
            }
            return count;
        }

        template<class Function>
        int count_where_alt(Function fn) const {
            int      count = 0;
            for (int i     = 0; i < s_CardinalCount; ++i) {
                if (fn(get_cardinal(i), cells[i])) ++count;
            }
            return count;
        }

        void set(const Cardinal dir, const Cell cell) {
            cells[static_cast<char>(dir)] = cell;
        }

        template<Cardinal Dir>
        void set(const Cell cell) {
            cells[static_cast<char>(Dir)] = cell;
        }

        Cell get(const Cardinal dir) {
            return cells[static_cast<char>(dir)];
        }

        template<Cardinal Dir>
        Cell get() {
            return cells[static_cast<char>(Dir)];
        }

        std::pair<Cardinal, Cell> get_random_valid(Random& rng) {
            return get_random_where(rng, [](const Cell cell) {
                return !is_set<Flag::INVALID>(cell);
            });
        }

        template<class Function>
        std::pair<Cardinal, Cell> get_random_where(Random& rng, Function predicate) {
            static_assert(std::is_invocable<Function, Cell>(), "Provided function is invalid...");
            if (count_where(predicate) == 0) {
                HERR("[ADJ_CELL]", " # No adjacent cells are valid...");
                throw std::exception();
            }

            // Shuffle directions and pick first valid
            Cardinal all_dirs[]{
                    Cardinal::NORTH, Cardinal::EAST,
                    Cardinal::SOUTH, Cardinal::WEST
            };
            std::shuffle(all_dirs, all_dirs + s_CardinalCount, rng);

            for (const auto dir : all_dirs) {
                const char index = static_cast<char>(dir);
                const Cell cell  = cells[index];
                if (predicate(cell)) return { dir, cell };
            }

            throw std::exception();
        }

        template<class Function>
        std::pair<Cardinal, Cell> get_random_where_alt(Random& rng, Function fn) {
            if (count_where_alt(fn) == 0) {
                HERR("[ADJ_CELL]", " # No adjacent cells are valid...");
                throw std::exception();
            }

            // Shuffle directions and pick first valid
            Cardinal all_dirs[]{
                    Cardinal::NORTH, Cardinal::EAST,
                    Cardinal::SOUTH, Cardinal::WEST
            };
            std::shuffle(all_dirs, all_dirs + s_CardinalCount, rng);

            for (const auto dir : all_dirs) {
                const char index = static_cast<char>(dir);
                const Cell cell  = cells[index];
                if (fn(dir, cell)) return { dir, cell };
            }

            throw std::exception();
        }

        template<class Function>
        void for_each_valid(Function fn) {
            for (int i = 0; i < s_CardinalCount; ++i) {
                auto cell = cells[i];
                if (!is_set<Flag::INVALID>(cell)) {
                    fn(get_cardinal(i), cell);
                }
            }
        }
    };

    //############################################################################//
    // | COMPONENTS |
    //############################################################################//

    class WallBase {

    private:
        Cell     m_Cell;
        Index2D  m_Pos;
        Cardinal m_Dir;

    public:
        WallBase(
                const Cell cell, const Index2D& pos, const Cardinal dir
        ) : m_Cell(cell), m_Pos(pos), m_Dir(dir) {};

        WallBase(const WallBase&) = default;
        WallBase(WallBase&&) = default;
        WallBase& operator =(const WallBase&) = default;
        WallBase& operator =(WallBase&&) = default;

    public:

        const Cell get_cell() const {
            return m_Cell;
        }

        void set_cell(Cell cell) {
            m_Cell = cell;
        }

        const Index2D& get_pos() const {
            return m_Pos;
        }

        const Cardinal get_wall_dir() const {
            return m_Dir;
        }

    public:

        glm::vec3 get_pos_vec(const float offset = 1.0F) const {
            float x  = m_Pos.row, y = 0.0F, z = m_Pos.col;
            float hf = offset * 0.5F;
            switch (m_Dir) {
                case Cardinal::EAST:
                    return glm::vec3{ x, y, z + hf };
                case Cardinal::SOUTH:
                    return glm::vec3{ x + hf, y, z };
                case Cardinal::NORTH:
                    return glm::vec3{ x - hf, y, z };
                case Cardinal::WEST:
                    return glm::vec3{ x, y, z - hf };
            }

            throw std::exception();
        }

        glm::vec3 get_scale_vec() const {
            glm::vec3 path_scale{ 0.1, 0.1, 0.1 };
            switch (m_Dir) {
                case Cardinal::EAST: {
                    if (is_set<Flag::PATH_EAST>(m_Cell)) return path_scale;
                    return glm::vec3{ 0.5, 0.5, 0.1 };
                }
                case Cardinal::WEST: {
                    if (is_set<Flag::PATH_WEST>(m_Cell)) return path_scale;
                    return glm::vec3{ 0.5, 0.5, 0.1 };
                }
                case Cardinal::SOUTH: {
                    if (is_set<Flag::PATH_SOUTH>(m_Cell)) return path_scale;
                    return glm::vec3{ 0.1, 0.5, 0.5 };
                }
                case Cardinal::NORTH: {
                    if (is_set<Flag::PATH_NORTH>(m_Cell)) return path_scale;
                    return glm::vec3{ 0.1, 0.5, 0.5 };
                }
            }
            throw std::exception();
        }

        glm::vec3 get_colour() const {
            glm::vec3 colour{ 0.0 };
            if (is_set<Flag::RED>(m_Cell)) colour.r   = 1.0F;
            if (is_set<Flag::GREEN>(m_Cell)) colour.g = 1.0F;
            if (is_set<Flag::BLUE>(m_Cell)) colour.b  = 1.0F;
            return colour;
        }

    };

    //############################################################################//
    // | MAZE DATA STRUCTURE |
    //############################################################################//

    class Maze2D {

        //############################################################################//
        // | ALIAS & MEMBERS |
        //############################################################################//

    public:
        using CellVec = std::vector<Cell>;

    private:
        Index2D m_GridSize;
        CellVec m_Cells;

        //############################################################################//
        // | CONSTRUCTORS |
        //############################################################################//

    public:

        explicit Maze2D(
                Index rows,
                Index cols
        ) : m_GridSize(Index2D{ rows, cols }),
            m_Cells(CellVec(static_cast<unsigned int>(m_GridSize.size()),
                            cellof<Flag::EMPTY_PATH>())) {

            if (m_GridSize.size() <= 0) {
                HERR("[MAZE2D]", " # Invalid size '{}'...", m_GridSize.size());
                throw std::exception();
            }
        }

        Maze2D(
                const Maze2D& maze
        ) : m_GridSize(maze.m_GridSize),
            m_Cells(maze.m_Cells) {
            HINFO("[MAZE2D_CPY]", " # Copy: '{}'", maze.to_string());
        }

        Maze2D(
                Maze2D&& maze
        ) : m_GridSize(maze.m_GridSize),
            m_Cells(std::move(maze.m_Cells)) {
            HINFO("[MAZE2D_MOVE]", " # Move: '{}'", maze.to_string());
        }

    public:

        Maze2D& operator =(Maze2D&& o) {
            m_GridSize = o.m_GridSize;
            m_Cells    = std::move(o.m_Cells);
            return *this;
        }

        //############################################################################//
        // | GETTERS |
        //############################################################################//

    public:

        Index get_row_count() const {
            return m_GridSize.row;
        }

        Index get_col_count() const {
            return m_GridSize.col;
        }

        Index2D get_bounds() const {
            return m_GridSize;
        }

        size_t get_size() const {
            return m_GridSize.size();
        }

        size_t get_total_wall_count() const {
            // Haskell: fn r c = r * c * 2 + r + c
            return get_size() * 2 + get_row_count() + get_col_count();
        }

        const Cell* get_cell_data() const {
            return m_Cells.data();
        }

        auto begin() {
            return m_Cells.begin();
        }

        auto end() {
            return m_Cells.end();
        }

        void set_flags_adjacent(const Index2D pos, std::initializer_list<Flag> flags) {
            set_flags(pos, flags);
            for (const Cardinal dir : s_AllCardinals) {
                if (inbounds(pos, dir)) set_flags(pos + cardinal_offset(dir), flags);
            }
        }

        void unset_flags_adjacent(const Index2D pos, std::initializer_list<Flag> flags) {
            unset_flags(pos, flags);
            for (const Cardinal dir : s_AllCardinals) {
                if (inbounds(pos, dir)) unset_flags(pos + cardinal_offset(dir), flags);
            }
        }

        AdjacentCells get_adjacent(const Index2D pos) const {
            AdjacentCells       cells{};
            for (const Cardinal dir : s_AllCardinals) {
                if (inbounds(pos, dir)) {
                    cells.set(dir, get_cell(pos + cardinal_offset(dir)));
                }
            }
            return cells;
        }

        template<class Function>
        void for_each_cell(Function fn) const {
            for (Index row = 0; row < m_GridSize.row; ++row) {
                for (Index col = 0; col < m_GridSize.col; ++col) {
                    Index2D i{ row, col };
                    fn(i, get_cell(i));
                }
            }
        }

        template<class Function>
        void for_each_wall(Function fn) const {
            return for_each_cell([&](const Index2D& pos, Cell cell) {
                if (!is_set<Flag::PATH_NORTH>(cell)) fn(Cardinal::NORTH, pos, cell);
                if (!is_set<Flag::PATH_EAST>(cell)) fn(Cardinal::EAST, pos, cell);
                if (!is_set<Flag::PATH_SOUTH>(cell)) fn(Cardinal::SOUTH, pos, cell);
                if (!is_set<Flag::PATH_WEST>(cell)) fn(Cardinal::WEST, pos, cell);
            });
        }

        template<class Function>
        void for_each_wall_unique(Function fn) const {
            return for_each_cell([&](const Index2D& pos, Cell cell) {

                // Evaluate North if in Top Row
                if (pos.row == 0 && !is_set<Flag::PATH_NORTH>(cell)) {
                    fn(Cardinal::NORTH, pos, cell);
                }

                // Evaluate West if in First Column (Leftmost column)
                if (pos.col == 0 && !is_set<Flag::PATH_WEST>(cell)) {
                    fn(Cardinal::WEST, pos, cell);
                }

                // Always evaluate East & South
                if (!is_set<Flag::PATH_EAST>(cell)) fn(Cardinal::EAST, pos, cell);
                if (!is_set<Flag::PATH_SOUTH>(cell)) fn(Cardinal::SOUTH, pos, cell);
            });
        }

        //############################################################################//
        // | CELL METHODS |
        //############################################################################//

    public:

        bool inbounds(const Index2D pos) const {
            return pos.inbounds(m_GridSize);
        }

        bool inbounds(const Index2D pos, const Cardinal dir) const {
            return (pos + cardinal_offset(dir)).inbounds(m_GridSize);
        }

        Cell& get_cell(const Index2D pos) {
            check_index(pos);
            return m_Cells[pos.flat(m_GridSize)];
        }

        const Cell get_cell(const Index2D pos) const {
            check_index(pos);
            return m_Cells[pos.flat(m_GridSize)];
        }

        void set_flags(const Index2D pos, std::initializer_list<Flag> flags) {
            Cell& cell = get_cell(pos);
            for (const Flag flag : flags) cell |= cellof(flag);
        }

        void unset_flags(const Index2D pos, std::initializer_list<Flag> flags) {
            Cell& cell = get_cell(pos);
            for (const Flag flag : flags) cell &= ~cellof(flag);
        }

        bool check_flags(const Index2D pos, std::initializer_list<Flag> flags) const {
            return is_set(flags, get_cell(pos));
        }

        void make_path(const Index2D pos, const Cardinal dir) {
            Cell& from = get_cell(pos);
            Cell& to   = get_cell(pos + cardinal_offset(dir));

            // Unset Empty Path Flag
            from &= ~cellof<Flag::EMPTY_PATH>();
            to &= ~cellof<Flag::EMPTY_PATH>();

            switch (dir) {
                case Cardinal::NORTH: {
                    from |= cellof(Flag::PATH_NORTH);
                    to |= cellof(Flag::PATH_SOUTH);
                    break;
                }
                case Cardinal::SOUTH: {
                    from |= cellof(Flag::PATH_SOUTH);
                    to |= cellof(Flag::PATH_NORTH);
                    break;
                }
                case Cardinal::EAST: {
                    from |= cellof(Flag::PATH_EAST);
                    to |= cellof(Flag::PATH_WEST);
                    break;
                }
                case Cardinal::WEST: {
                    from |= cellof(Flag::PATH_WEST);
                    to |= cellof(Flag::PATH_EAST);
                    break;
                }
                default:
                    throw std::exception();
            }
        }

        void make_path(Index2D a, Index2D b) {
            Index2D pos = (a - b);

            // Validate
            if (pos.row > 1 || pos.col > 1 || pos.row < -1 || pos.col < -1) {
                HERR(
                        "[MAZE2D]",
                        " # Make Path {} to {} resulted in: {} which is invalid...",
                        a.to_string(),
                        b.to_string(),
                        pos.to_string()
                );
                throw std::exception();
            }

            for (Cardinal dir : s_AllCardinals) {
                if (cardinal_offset(dir) == pos) {
                    make_path(a, dir);
                    return;
                }
            }

            throw std::exception();
        }

        //############################################################################//
        // | UTILITY |
        //############################################################################//

    public:

        std::string to_string() const {
            return m_GridSize.to_string();
        }

        void check_index(const Index2D pos) const {
            if (!inbounds(pos)) {
                HERR(
                        "[MAZE2D]",
                        " # Index '{}' is out of bounds for range '{}'...",
                        pos.to_string(),
                        m_GridSize.to_string()
                );
                throw std::exception();
            }
        }

        void print_maze() const {
            std::string s{};
            s.append("\n");
            for (Index i = 0; i < get_row_count(); ++i) {
                for (Index j = 0; j < get_col_count(); ++j) {
                    s.append(std::format("({:<2}, {:<2}) :: {:b} {}\n",
                                         i, j,
                                         get_cell({ i, j }),
                                         eval_flags(get_cell({ i, j }))
                    ));
                }
            }

            HINFO("[MAZE2D]", " # Current Maze State #\n{}", s);
        }

        void print_debug_str() const {
            std::string s{};

            for (Index i = 0; i < get_row_count(); ++i) {
                for (Index j = 0; j < get_col_count(); ++j) {
                    std::string cell_str{};

                    cell_str.append("P{");
                    Cell cell = get_cell(Index2D{ i, j });

                    if (is_set<Flag::PATH_NORTH>(cell)) {
                        cell_str.append("N");
                    } else {
                        cell_str.append("_");
                    }

                    if (is_set<Flag::PATH_EAST>(cell)) {
                        cell_str.append("E");
                    } else {
                        cell_str.append("_");
                    }

                    if (is_set<Flag::PATH_SOUTH>(cell)) {
                        cell_str.append("S");
                    } else {
                        cell_str.append("_");
                    }

                    if (is_set<Flag::PATH_WEST>(cell)) {
                        cell_str.append("W");
                    } else {
                        cell_str.append("_");
                    }

                    s.append(cell_str).append("}");

                    if (j != get_col_count() - 1) {
                        s.append(", ");
                    } else {
                        s.append("\n");
                    }
                }
            }

            HINFO("[PRINT_MAZE]", " #\n{}", s);
        }

        void reset() {
            std::for_each(m_Cells.begin(), m_Cells.end(), [&](auto& item) {
                item = cellof<Flag::EMPTY_PATH>();
            });
        }

        void resize(Index2D new_size) {
            if (new_size.row <= 0 || new_size.col <= 0) {
                HERR("[MAZE2D]", " # New Maze size '{}' is invalid...", new_size.to_string());
                throw std::exception();
            }
            m_GridSize = new_size;
            m_Cells.resize(new_size.size(), cellof<Flag::EMPTY_PATH>());
        }
    };

    //############################################################################//
    // | MAZE GENERATOR |
    //############################################################################//

    class AbstractMazeGenerator {

    private:
        inline static Random s_Random{};

    protected:
        bool m_IsComplete = false;
        bool m_IsInit     = false;

    public:
        AbstractMazeGenerator() = default;
        virtual ~AbstractMazeGenerator() = default;

    public:
        bool is_initialised() const {
            return m_IsInit;
        }

        bool is_complete() const {
            return m_IsComplete;
        }

        void init_once(Maze2D& maze) {
            if (!m_IsInit) {
                init(maze);
                m_IsInit = true;
                HINFO("[GENERATOR]", " # Initialised Maze Generator...");
            }
        }

        void step(Maze2D& maze, unsigned int count) {
            for (unsigned int i = 0; i < count; ++i) step(maze);
        }

        Random& get_random() {
            return s_Random;
        }

    public:
        virtual void init(Maze2D& maze) = 0;
        virtual void step(Maze2D& maze) = 0;
    };

    using MazeGenerator = std::unique_ptr<AbstractMazeGenerator>;

    //############################################################################//
    // | COMPLETELY RANDOM 'MAZE' |
    //############################################################################//

    class RandomMazeImpl : public AbstractMazeGenerator {
    private:
        Index2D m_CurrentPos{ 0, 0 };

    public:
        virtual void init(Maze2D& maze) override {

        }

        virtual void step(Maze2D& maze) override {
            if (is_complete()) return;

            maze.set_flags(m_CurrentPos, {
                    flagof(s_CellColourDist(get_random())),
                    flagof(s_CellColourDist(get_random()))
            });

            // Create a random number of paths
            int      path_range = s_CardinalDist(get_random());
            for (int i          = 0; i < path_range; ++i) {
                const Cardinal dir = get_cardinal(s_CardinalDist(get_random()));
                if (maze.inbounds(m_CurrentPos, dir)) {
                    maze.make_path(m_CurrentPos, dir);
                }
            }

            if (m_CurrentPos.has_next(maze.get_row_count(), maze.get_col_count())) {
                m_CurrentPos = m_CurrentPos.next(maze.get_row_count(), maze.get_col_count());
            } else {
                m_IsComplete = true;
            }
        }
    };

    class PathSingleDirection : public AbstractMazeGenerator {
    private:
        Index2D  m_Prev{ 0, 0 };
        Index2D  m_Pos{ 0, 0 };
        Cardinal m_Direction;

    public:
        PathSingleDirection(Cardinal dir = Cardinal::WEST) : m_Direction(dir) {}

    public:
        virtual void init(Maze2D& maze) override {

        }

        virtual void step(Maze2D& maze) override {
            if (is_complete()) return;

            // Step Forward
            if (maze.inbounds(m_Pos, m_Direction)) {
                maze.make_path(m_Pos, m_Direction);
                maze.set_flags(m_Pos, { Flag::VISITED, Flag::RED });
            }

            // Update Previous
            maze.unset_flags(m_Prev, { Flag::RED });
            maze.set_flags(m_Prev, { Flag::GREEN });

            // Update State
            if (m_Pos.has_next(maze.get_row_count(), maze.get_col_count())) {
                m_Prev = m_Pos;
                m_Pos  = m_Pos.next(maze.get_row_count(), maze.get_col_count());
            } else {
                m_IsComplete = true;
            }
        }
    };

    //############################################################################//
    // | RECURSIVE BACKTRACKER |
    //############################################################################//

    class RecursiveBacktrackImpl : public AbstractMazeGenerator {

    private:
        std::stack<Index2D> m_Stack{};

    public:
        virtual void init(Maze2D& maze) override {
            m_Stack.emplace(
                    Distribution(0, maze.get_row_count() - 1)(get_random()),
                    Distribution(0, maze.get_col_count() - 1)(get_random())
            );
        }

        virtual void step(Maze2D& maze) override {
            if (is_complete()) return;

            if (m_Stack.empty()) {
                m_IsComplete = true;
                std::for_each(maze.begin(), maze.end(), [&](Cell& item) {
                    item |= cellof<Flag::RED>()
                            | cellof<Flag::GREEN>()
                            | cellof<Flag::BLUE>()
                            | cellof<Flag::FINISHED>();
                });
                HINFO("[BACKTRACK]", " # Recursive backtracker has finished...");
                return;
            }

            Index2D       pos       = m_Stack.top();
            AdjacentCells adj_cells = maze.get_adjacent(pos);

            // Can't go anywhere so unwind.
            if (adj_cells.count_where(is_valid) == 0) {
                m_Stack.pop();
                if (!m_Stack.empty()) {
                    const Index2D top = m_Stack.top();
                    maze.set_flags(top, { Flag::RED });
                    maze.unset_flags(top, { Flag::GREEN });
                    maze.unset_flags(pos, { Flag::GREEN });
                }

                // At-least one neighbouring cell is accessible
            } else {
                const auto [dir, cell] = adj_cells.get_random_where(get_random(), is_valid);
                maze.unset_flags(pos, { Flag::EMPTY_PATH, Flag::RED });
                maze.set_flags(pos, { Flag::VISITED, Flag::GREEN });
                maze.set_flags(pos + cardinal_offset(dir), { Flag::VISITED, Flag::GREEN });
                maze.make_path(pos, dir);
                m_Stack.emplace(pos + cardinal_offset(dir));
            }

        }

    private:
        static bool is_valid(const Cell cell) {
            return !is_set<Flag::INVALID>(cell) && !is_set<Flag::VISITED>(cell);
        }
    };

    //############################################################################//
    // | HUNT & KILL ALGORITHM |
    //############################################################################//

    class HuntAndKillBase : public AbstractMazeGenerator {

    protected:
        Index2D             m_CurrentPosition{};
        bool                m_IsRandomWalk = true;
        std::deque<Index2D> m_UnvisitedCells{};
        size_t              m_VisitedCount = 0;

    private:
        virtual void populate_cells(Maze2D& maze, std::deque<Index2D>& cells) {
            m_UnvisitedCells.clear();
            maze.for_each_cell([&](Index2D pos, Cell cell) {
                m_UnvisitedCells.push_front(pos);
            });
        }

        virtual Index2D get_starting_cell() {
            return m_UnvisitedCells.front();
        }

    public:
        virtual void init(Maze2D& maze) override {
            populate_cells(maze, m_UnvisitedCells);

            m_CurrentPosition = get_starting_cell();
            maze.set_flags(m_CurrentPosition, { Flag::VISITED });
        }

        virtual void step(Maze2D& maze) override {

            if (m_IsComplete) return;

            if (m_VisitedCount >= maze.get_size()) {
                HINFO("[RH&K]", " # Hunt & Kill Finished...");
                m_IsComplete = true;
                std::for_each(maze.begin(), maze.end(), [&](Cell& item) {
                    item |= cellof<Flag::RED>()
                            | cellof<Flag::GREEN>()
                            | cellof<Flag::BLUE>()
                            | cellof<Flag::FINISHED>();
                });
                return;
            }

            // Random Walk
            if (m_IsRandomWalk) {
                AdjacentCells adj = maze.get_adjacent(m_CurrentPosition);

                constexpr auto neighbour_validator = [&](Cardinal dir, Cell cell) {
                    return is_all_unset<Flag::INVALID, Flag::VISITED>(cell);
                };

                // Early Return if no valid adjacent Cells
                if (adj.count_where_alt(neighbour_validator) == 0) {
                    m_IsRandomWalk = false;
                    return;
                }

                auto [dir, cell] = adj.get_random_where_alt(get_random(), neighbour_validator);

                // Set flags for current position
                const auto unset_group = { Flag::RED, Flag::GREEN, Flag::BLUE };
                const auto set_group   = { Flag::GREEN, Flag::BLUE, Flag::VISITED };

                unset_then_set_flags(m_CurrentPosition, maze, unset_group, set_group);
                maze.make_path(m_CurrentPosition, dir);
                m_CurrentPosition = m_CurrentPosition + cardinal_offset(dir);
                unset_then_set_flags(m_CurrentPosition, maze, unset_group, set_group);

                // Find a New Cell
            } else {
                Index2D prev_pos = m_CurrentPosition;
                m_CurrentPosition = m_UnvisitedCells.back();
                m_UnvisitedCells.pop_back();
                m_UnvisitedCells.push_front(m_CurrentPosition);

                if (is_valid_cell(maze, m_CurrentPosition)) {

                    unset_then_set_flags(
                            m_CurrentPosition, maze,
                            { Flag::RED, Flag::GREEN, Flag::BLUE },
                            { Flag::GREEN, Flag::VISITED }
                    );

                    m_IsRandomWalk = true;
                    m_VisitedCount = 0;
                } else {
                    unset_then_set_flags(
                            prev_pos, maze,
                            { Flag::GREEN },
                            { Flag::RED, Flag::BLUE }
                    );

                    unset_then_set_flags(
                            m_CurrentPosition, maze,
                            { Flag::RED, Flag::GREEN, Flag::BLUE },
                            { Flag::RED }
                    );
                    ++m_VisitedCount;
                }
            }
        }

    private:
        bool is_valid_cell(Maze2D& maze, Index2D pos) {
            // Inbounds and Unvisited
            Cell           cell      = maze.get_cell(pos);
            constexpr auto validator = [](Cell c) {
                return is_unset<Flag::INVALID>(c) && is_set<Flag::VISITED>(c);
            };

            // If visited skip
            if (is_set<Flag::VISITED>(cell)) {
                return false;
            }

            // If unvisited then if one of the adjacent cells are visited then this cell is ok
            auto adj   = maze.get_adjacent(pos);
            int  count = adj.count_where(validator);

            if (count > 0) {
                auto [dir, _] = adj.get_random_where(get_random(), validator);
                maze.make_path(pos, dir);
                unset_then_set_flags(
                        pos, maze,
                        { Flag::RED, Flag::GREEN, Flag::BLUE },
                        { Flag::GREEN }
                );
                return true;
            } else {
                return false;
            }
        }

        void unset_then_set_flags(
                Index2D pos,
                Maze2D& maze,
                std::initializer_list<Flag> to_unset = {},
                std::initializer_list<Flag> to_set = {}
        ) {
            maze.unset_flags(pos, to_unset);
            maze.set_flags(pos, to_set);
        }

    };

    class RandomHuntAndKillImpl : public HuntAndKillBase {

        virtual void populate_cells(Maze2D& maze, std::deque<Index2D>& cells) override {
            maze.for_each_cell([&](Index2D pos, auto) {
                cells.push_back(pos);
            });
            std::shuffle(cells.begin(), cells.end(), get_random());
        }

        virtual Index2D get_starting_cell() override {
            std::uniform_int_distribution<size_t> index_dist{ 0, m_UnvisitedCells.size() };
            return m_UnvisitedCells.at(index_dist(get_random()));
        }
    };

    class StandardHuntAndKill : public HuntAndKillBase {

        virtual void populate_cells(Maze2D& maze, std::deque<Index2D>& cells) override {
            Index row_max = maze.get_row_count();
            Index col_max = maze.get_col_count();

            bool is_reverse = false;

            // Iterates in a Serpentine Pattern
            for (Index i = 0; i < row_max; ++i) {
                for (Index j = is_reverse ? col_max - 1 : 0;
                     is_reverse ? j >= 0 : j < col_max;
                     is_reverse ? --j : ++j) {
                    cells.emplace_back(i, j);
                }
                is_reverse = !is_reverse;
            }

        }

        virtual Index2D get_starting_cell() override {
            return m_UnvisitedCells.front();
        }

    };

}

#endif //MAZEVISUALISATION_MAZECONSTRUCTS_H

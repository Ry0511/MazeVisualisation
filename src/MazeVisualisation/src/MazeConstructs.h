//
// Created by -Ry on 08/02/2023.
//

#ifndef MAZEVISUALISATION_MAZECONSTRUCTS_H
#define MAZEVISUALISATION_MAZECONSTRUCTS_H

#include "Logging.h"

#include <glm/glm.hpp>

#include <vector>
#include <cstdint>
#include <random>
#include <stack>
#include <glm/gtc/type_ptr.hpp>

namespace maze {

    //############################################################################//
    // | GLOBAL ALIAS |
    //############################################################################//

    using Cell = uint32_t;
    using Index = int;
    using Distribution = std::uniform_int_distribution<Index>;
    using Random = std::mt19937;

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

        constexpr Index2D next(const Index row_max, const Index col_max) const {
            if (col < col_max - 1) {
                return Index2D{ row, col + 1 };
            } else if (row < row_max - 1) {
                return Index2D{ row + 1, 0 };
            } else {
                return *this;
            }
        }

        std::string to_string() const {
            return std::format("( {},{} )", row, col);
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
        RED       = 1 << 5,
        GREEN     = 1 << 6,
        BLUE      = 1 << 7,
        TEX_BRICK = 1 << 8,
        TEX_STONE = 1 << 9,
        TEX_DOOR  = 1 << 10,

        // Cell State
        VISITED = 1 << 11,
        INVALID = 1 << 12
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

    static inline constexpr char          s_CardinalCount = 4;
    static inline const Distribution      s_CardinalDist  = Distribution{ 0, 3 };
    static inline std::array<Cardinal, 4> s_AllCardinals{
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

        size_t get_size() const {
            return m_GridSize.size();
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

        AdjacentCells get_adjacent(const Index2D pos) const {
            AdjacentCells       cells{};
            for (const Cardinal dir : s_AllCardinals) {
                if (inbounds(pos, dir)) {
                    cells.set(dir, get_cell(pos + cardinal_offset(dir)));
                }
            }
            return cells;
        }

        void fill_path_vec(std::vector<glm::vec3>& vec, std::vector<glm::mat4>& scale_vec, float offset = 1.0F) const {

            auto get_colour = [](const Cell cell) {
                glm::vec3 c{};
                if (is_set<Flag::RED>(cell)) c.r   = 1.0;
                if (is_set<Flag::GREEN>(cell)) c.g = 1.0;
                if (is_set<Flag::BLUE>(cell)) c.b  = 1.0;
                return c;
            };

            for (Index row = 0; row < m_GridSize.row; ++row) {
                for (Index col = 0; col < m_GridSize.col; ++col) {
                    const Cell cell = get_cell({ row, col });

                    float     x      = row, y = 0.0F, z = col;
                    glm::vec3 colour = get_colour(cell);

                    // Add Floor
                    vec.emplace_back(x, y - offset, z);
                    vec.emplace_back(1.0, 1.0, 1.0);
                    scale_vec.push_back(glm::scale(glm::mat4{1}, glm::vec3{0.5}));

                    // Evaluate West Path if in far left column
                    if (col == 0 && !is_set<Flag::PATH_WEST>(cell)) {
                        vec.emplace_back(x, y, z - offset);
                        vec.push_back(glm::vec3{1.0, 0.0, 0.0});
                        scale_vec.push_back(glm::scale(glm::mat4{1}, glm::vec3{0.5, 0.5, 0.5}));
                    }

                    // Evaluate North Wall if in Top Row
                    if (row == 0 && !is_set<Flag::PATH_NORTH>(cell)) {
                        vec.emplace_back(x - offset, y, z);
                        vec.push_back(glm::vec3{0.0, 1.0, 0.0});
                        scale_vec.push_back(glm::scale(glm::mat4{1}, glm::vec3{0.5, 0.5, 0.5}));
                    }

                    // Always Evaluate East & South
                    if (!is_set<Flag::PATH_EAST>(cell)) {
                        vec.emplace_back(x, y, z + offset);
                        vec.push_back(glm::vec3{0.0, 0.0, 1.0});
                        scale_vec.push_back(glm::scale(glm::mat4{1}, glm::vec3{0.5, 0.5, 0.5}));
                    }

                    if (!is_set<Flag::PATH_SOUTH>(cell)) {
                        vec.emplace_back(x + (offset * 0.5), y, z);
                        vec.push_back(glm::vec3{1.0, 1.0, 0.0});
                        scale_vec.push_back(glm::scale(glm::mat4{1}, glm::vec3{0.2, 0.5, 0.5}));
                    }
                }
            }
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

            // TODO: This isn't enough, we also need to set all adjacent cells accessible.

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
    };

    //############################################################################//
    // | MAZE GENERATOR |
    //############################################################################//

    class AbstractMazeGenerator {

    protected:
        bool   m_IsComplete = false;
        Random m_Random     = {};

    public:
        AbstractMazeGenerator() = default;
        virtual ~AbstractMazeGenerator() = default;

    public:
        bool is_complete() const {
            return m_IsComplete;
        }

        void step(Maze2D& maze, unsigned int count) {
            for (unsigned int i = 0; i < count; ++i) {
                step(maze);
            }
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
                    flagof(s_CellColourDist(m_Random)),
                    flagof(s_CellColourDist(m_Random))
            });

            // Create a random number of paths
            int      path_range = s_CardinalDist(m_Random);
            for (int i          = 0; i < path_range; ++i) {
                const Cardinal dir = get_cardinal(s_CardinalDist(m_Random));
                if (maze.inbounds(m_CurrentPos, dir)) {
                    maze.make_path(m_CurrentPos, dir);
                }
            }

            Index2D next = m_CurrentPos.next(maze.get_row_count(), maze.get_col_count());
            if (next == m_CurrentPos) {
                m_IsComplete = true;
            } else {
                m_CurrentPos = next;
            }
        }
    };

    class WallWestToEastImpl : public AbstractMazeGenerator {
    private:
        Index2D m_Pos{ 0, 0 };

    public:
        virtual void init(Maze2D& maze) override {

        }

        virtual void step(Maze2D& maze) override {
            if (is_complete()) return;

            if (maze.inbounds(m_Pos, Cardinal::EAST)) {
                maze.make_path(m_Pos, Cardinal::EAST);
            }

            Index2D next = m_Pos.next(maze.get_row_count(), maze.get_col_count());
            m_IsComplete = next == m_Pos;
            m_Pos        = next;

            if (is_complete()) {
                HINFO("[W2E]", " # West to East path generator finished...");
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
                    Distribution(0, maze.get_row_count() - 1)(m_Random),
                    Distribution(0, maze.get_col_count() - 1)(m_Random)
            );
        }

        virtual void step(Maze2D& maze) override {
            if (is_complete()) return;

            if (m_Stack.empty()) {
                m_IsComplete = true;
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
                const auto [dir, cell] = adj_cells.get_random_where(m_Random, is_valid);
                maze.unset_flags(pos, { Flag::EMPTY_PATH, Flag::RED });
                maze.set_flags(pos, { Flag::VISITED, Flag::GREEN });
                maze.make_path(pos, dir);
                m_Stack.emplace(pos + cardinal_offset(dir));
            }

        }

    private:
        static bool is_valid(const Cell cell) {
            return !is_set<Flag::INVALID>(cell) && !is_set<Flag::VISITED>(cell);
        }
    };

}

#endif //MAZEVISUALISATION_MAZECONSTRUCTS_H
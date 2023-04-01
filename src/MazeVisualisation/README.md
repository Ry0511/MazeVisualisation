# Maze Visualisation

This is the main application executable project.

## Controls

The program is broken into various states and depending on the current state you can use different
keybinds.

|  Key/s  | Description                                  | Required Game State |
|:-------:|:---------------------------------------------|:-------------------:|
| W,A,S,D | Moves the Forward, Left, Back, and Right     |         Any         |
|    F    | Cycles the current Wall and Floor texture    |         Any         |
|    T    | Cycles the Skybox texture                    |         Any         |
|    1    | Enter Maze Generation Mode                   |         Any         |
|    2    | Enter Algorithm Solving Mode                 |         Any         |
|    3    | Enter Player Solving Mode                    |         Any         |
|  SPACE  | Pauses/Unpauses the current algorithm        |   Maze Generation   |
|    Q    | Slows down up the current algorithm by *2    |   Maze Generation   |
|    E    | Speeds up the current algorithm by *2        |   Maze Generation   |
|    R    | Resets the maze to its default state         |   Maze Generation   |
|    +    | Cycles the current Maze Generation Algorithm |   Maze Generation   |

# Development

Development was done using CLion and CMake complied and tested using MSVC C++20.

# Dependencies

The executable relies on AppFramework and all its dependencies as well.

## Library Usages

1. AppFramework - Application Render & Update loop
2. All libraries relied upon by AppFramework

# Notes

The 'Res' folder is the Resources folder and from the executable said folder should be accessible,
that is, file-paths internally use 'Res/...' so if building and for somereason CMake doesn't copy
the files, or you use your own custom output directory then copy the 'Res' folder to the folder
containing MazeVisualisation.exe
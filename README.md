 # Pac-Man 3D in C (OpenGL/GLUT)

A Pac-Man game written in C with OpenGL and GLUT/freeglut. The board is rendered in 3D, maps are loaded from text files, and the camera can be panned, zoomed and rotated during play.

## Features

- Maps loaded from `.txt` files, with support for multiple maps in a single file
- 3D Pac-Man with an animated mouth and smooth movement between cells
- Ghosts with random colours and speeds that chase Pac-Man and pick a random direction when blocked
- Win by visiting every path cell; lose when a ghost catches Pac-Man
- Wall mode: Pac-Man leaves walls behind him that block the ghosts
- Pause, restart and switch maps without leaving the game
- Free camera (pan, zoom and rotation)

## Controls

| Key | Action |
|---|---|
| Arrow keys | Move Pac-Man |
| `P` | Pause / resume |
| `R` | Restart the current map |
| `N` | Go to the next map |
| `W` | Toggle wall mode |
| `x` / `X` | Pan the camera along the X axis |
| `y` / `Y` | Pan the camera along the Y axis |
| `z` / `Z` | Zoom out / in |
| `k` / `K` | Rotate the camera vertically (pitch) |
| `j` / `J` | Rotate the camera horizontally (yaw) |
| `Esc` | Quit |

## Project structure

| File | Description |
|---|---|
| `main.c` | Initialises GLUT and the window, and registers the callbacks |
| `board.c` / `board.h` | Game state, camera, board rendering, keyboard input and end-of-game messages |
| `character.c` / `character.h` | Pac-Man, ghosts, movement, collisions and win condition |
| `pm-maps.c` / `pm-maps.h` | Map file parsing and cell management |
| `cube.c` / `cube.h` | Cube drawing used for walls and cells |
| `pm-simple.txt` | Two small maps (loaded by default) |
| `pm-Pacman.txt` | 28×31 map inspired by the classic maze |
| `pacman.sln`, `Figura2D.vcxproj` | Visual Studio solution and project |

## Map format

```
<number of maps>

<ghosts> <smart ghosts>
<columns> <rows>
<row 1>
<row 2>
...
```

In each map row, `1` is a path cell and `0` is an empty cell. Pac-Man and the ghosts start on randomly chosen path cells. The two ghost counts are currently added together and all ghosts behave the same way.

Example (`pm-simple.txt`, first map):

```
2

1 1
6 8
110111
110111
100111
101111
100011
110001
110111
111111
```

## Building and running

### Windows (Visual Studio)

1. Install [freeglut](https://freeglut.sourceforge.net/) to `C:\freeglut` (the project looks for headers in `C:\freeglut\include`).
2. Open `pacman.sln` in Visual Studio.
3. Make sure `freeglut.lib` is in the linker dependencies and `freeglut.dll` is next to the executable.
4. Build and run (`Ctrl+F5`).

### Linux

```bash
sudo apt install build-essential freeglut3-dev
gcc main.c board.c character.c cube.c pm-maps.c -o pacman -lglut -lGLU -lGL -lm
./pacman
```

### macOS

```bash
clang main.c board.c character.c cube.c pm-maps.c -o pacman -framework OpenGL -framework GLUT
./pacman
```

### Using a different map file

With no arguments, the game loads `pm-simple.txt`. To use another file:

```bash
./pacman pm-Pacman.txt
```

## Author

António Vélez — [@agvelez07](https://github.com/agvelez07)

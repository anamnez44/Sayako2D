# Sayako2D

Sayako2D is a 2D game written in C++ using SFML and TinyXML.

## About the Project

I started this game in 2020 when I wanted to get into game development. At that time, I was learning C++ and trying to create something of my own. The current project is presented as a small demo game built with Visual Studio.

## Project Structure

- `Game.sln` — Visual Studio solution.
- `Game/Game.vcxproj` — main project file.
- `Game/Source.cpp` — main source file.
- `Game/Animation.h`, `Game/healthBar.h`, `Game/level.h`, `Game/view.h` — header files.
- `Game/TinyXML/` — TinyXML library for XML processing.
- `Game/map.tmx`, `Game/map2.tmx`, `Game/animation.xml` — level and animation data.
- `Game/images/` — game image assets.

## How to Run

1. Open `Game.sln` in Visual Studio.
2. Make sure the configuration is set to `Debug` and the platform is `x64`.
3. Build the solution.
4. Run the project.

## Notes

- Build folders, temporary files, and binary files are excluded from the repository.
- The project uses external libraries SFML and TinyXML.

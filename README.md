# The Legend of Teto

## Description

The Legend of Teto (this game will not be about Kasane Teto, I just saw a Zelda game on my shelf and a Teto wallpaper on my laptop. I named this project like that because I lack imagination) is a game about idk, I just wanted to make something, I didn't think this far.

## Building

Building for windows: ```.\build_windows.bat``` (I just gave up on making a makefile for both linux and windows)

~~Building for linux: ```make```~~ (Too bad ! The make file is empty right now)

# TODO

- Add a Scene class that contains an array of Batches
- That way we can almost test the real performance of Creating and Destroying meshes
- Mesh class should know about its parent batch
- Texture class (which is in reality a container for a TextureArray2D) to be done
- Same for shader class
- Restore engine singleton functions (EventManager, etc...)
- EventManager should have both lua scripting and compile time events
- build assimp and all that to load models
- create a tool to separate all data of a model (geometry, animations, textures, materials)
- add an animation feature
- add a material feature -> should the shader be part of the material ?
- make a makefile
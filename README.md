# TLOT

## Description

TLOT is a game engine I'm developping for my final uni assignement "Scopalatro" (a mix between the italian card game scopa and the  roguelike balatro), it's very barebone and while I try to optimize, i'm too dumb to do make my optimizations work :(

## Building

Building : ```make``` at project root directory. Modify the makefile to fit your needs. (i'm not even sure it works on linux)

## TODO

- Restore EventManager and allow script + compile time events (compile time has more priority)
- Restore frameBuffers
- Add Shader parameter to models shader (it probably imply the creation of a Instance<Shader> but whatever)

___

- Allow reisizing of buffers when trying to add stuff (and decrease original buffer size) (High Prio)

___

- Try to optimize more the models insertion (medium prio)
- Add Static/Dynamic geometry (Medium Prio)
- Create that bucket system for the CompoundElementArray class to spread a little more bundled data and not have an infinite growing array of contiguous elements (I would asume that allocating contiguous memory might become difficult the more vertices we try to add) (medium prio)

___

- Add logging (low prio)
- Rethink some stuff to allow multithreadead applications (low prio)
- Add more than 1 texture to a mesh (low prio)

___

- Add an animation feature (very low prio)

## Notes
- the "bottleneck" is adding/removing meshes from the renderer: we're recalculating and resending instance data every frames, not good, it starts getting really bad at ~40000 meshes (Chiori $+$ Columbina model $\times$  2000) (its still not good before but eh)

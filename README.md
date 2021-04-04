# maze4d
First person four-dimensional maze game, visualized by 3D cross-sections.

[![CI Linux](https://github.com/IronLynx/maze4d/actions/workflows/ci-linux.yml/badge.svg)](https://github.com/IronLynx/maze4d/actions/workflows/ci-linux.yml)

## Build
### Windows
Just clone the repository, and build `maze4d.sln` in Visual Studio 2015.

Project includes following dependencies:
- glfw-3.3.2
- glm-0.9.9.8
- freetype-2.10.4

## Controls
```
 ------------ Movement ------------
 Forward (+x): W,   Back (-x): S
 Up      (+y): R,   Down (-y): F
 Right   (+z): D,   Left (-z): A
 Ana     (+w): T,   Kata (-w): G

 ------------ Rotation ------------
 XY (pitch): I,    K  (or mouseY)
 XZ (yaw):   L,    J  (or mouseX)
 XW:         O,    U  (or mouseY + LSHIFT)
 YZ (roll):  E,    Q
 YW:         Z,    C
 ZW:         M,    N  (or mouseX + LSHIFT)
 Align W-angle:    TAB
 Reset W-angle:    B

 ---------- Editor tools ----------
 New cube:              Left Mouse Click
 New cube near:         Scroll/Mouse4 Click
 Delete cube:           Right Mouse Click
 Solid cube:            1
 Light cube:            2
 Clear all:             Delete
 Restore map border:    Insert
 Decrease transparency: Scroll/Page Down
 Increase transparency: Scroll/Page Up

 ------------- Other --------------
 Reset player:      P
 Noclip:            F8
 Fullscreen:        F11
 Lock/unlock mouse: SPACE
 Menu:              ESC
```

<details><summary>Screenshots</summary>
  
![](/screenshots/scr1.jpg)

![](/screenshots/scr2.jpg)

![](/screenshots/scr3.jpg)
</details>

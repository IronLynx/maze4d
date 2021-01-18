# maze4d
First person four-dimensional maze game, visualized by 3D cross-sections. The game uses software rendering (CPU-based raycasting), so it has poor performance and can be very slow at high resolutions.

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

 ------------ Other ------------
 Reset player:      P
 Noclip:            F8
 Fullscreen:        F11
 Lock/unlock mouse: SPACE or mouse button
 Menu:              ESC
```

<details><summary>Screenshots</summary>
  
![](/screenshots/scr1.jpg)

![](/screenshots/scr2.jpg)

![](/screenshots/scr3.jpg)
</details>

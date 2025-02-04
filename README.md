## brocgl

### Intro
brocgl is a basic setup code needed to start using OpenGL on Windows.

It includes creating a window and a proper modern OpenGL context.

User api boils down to creating 2 functions: setup() and draw().

* void setup();
Called once during the initialization.
* void draw();
Called in a while() loop to update the screen.

### Usage

In your project directory:

1. `git submodule add https://github.com/brocbyte/brocgl`
2. `cd brocgl`
3. `setup_dev.cmd`
4. in your source cpp file, do `#include "win_gl.cpp"` and implement `void setup()` and `void draw()` functions
5. in your build process, set language version to std:c++20, add include directory brocgl\, link with user32.lib, gdi32.lib, opengl32.lib

5th step in a single `cl` call:

```
cl game.cpp /std:c++20 /I brocgl\ user32.lib gdi32.lib opengl32.lib
```


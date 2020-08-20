# Strafe Helper

Code for drawing a [gazhud](https://www.q3df.org/wiki?p=133)-like HUD overlay
for id Tech 3 engine games, that shows optimal strafing angles. This was
initially written for the three **id Tech 3** games by Raven Software,
*Star Trek: Voyager - Elite Force*, *Star Wars Jedi Knight II: Jedi Outcast* and
*Star Wars Jedi Knight: Jedi Academy*, but it can work fine for other
**id Tech 3** games or even other **id Tech** engines too.

## Usage

Include this repository as
[submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules) in your project
and add `strafe_helper.c` to your build system. Then a header file called
`strafe_helper_includes.h` has to be created directly outside the submodule.
It needs to declare the constants

```c
/*color_type*/ shi_color_accelerating;
/*color_type*/ shi_color_optimal;
/*color_type*/ shi_color_center_marker;
/*color_type*/ shi_color_speed;
```

the functions

```c
void shi_drawFilledRectangle(float x, float y, float w, float h,
                             /*color_type*/ color);
void shi_drawString(float x, float y, const char* string, float scale,
                    /*color_type*/ color);
```

and the functions `acosf`, `snprintf`, `sqrtf` and `truncf` from the C Standard
Library. If the standard library can be used, including `math.h` and `stdio.h`
is enough for the latter. If not (for example for Quake Virtual Machine code),
equivalent implementations have to be given manually.

Then, call the functions `StrafeHelper_SetAccelerationValues` and
`StrafeHelper_Draw` where appropriate.

If drawing the current speed is not wanted, the declaration of
`shi_color_speed`, `shi_drawString` and `snprintf` can be omitted. Instead use
the preprocessor definition

```c
#define STRAFE_HELPER_INTERFACE_DISABLE_DRAW_SPEED
```

to disable that part of the strafe helper.

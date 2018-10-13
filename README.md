Strafe Helper
=============

Code for drawing a [gazhud](https://www.q3df.org/wiki?p=133)-like HUD overlay
for id Tech 3 engine games, that shows optimal strafing angles. This was
initially written for the three **id Tech 3** games by Raven Software,
*Star Trek: Voyager - Elite Force*, *Star Wars Jedi Knight II: Jedi Outcast* and
*Star Wars Jedi Knight: Jedi Academy*, but it can work fine for other
**id Tech 3** games or even other **id Tech** engines too.

# Usage

Include this repository as
[submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules) in your project
and add `strafe_helper.c` to your build system. Declare the constants
```lang=c
/*color_type*/ shi_color_accelerating;
/*color_type*/ shi_color_optimal;
/*color_type*/ shi_color_center_marker;
```
and the function
```lang=c
void shi_drawFilledRectangle(float x, float y, float w, float h,
                             /*color_type*/ color);
```
in a file called `strafe_helper_includes.h` that is located one directory above
the file `strafe_helper.c` and implement them accordingly. Implementation can be
in the header as well.

Then, call the functions `StrafeHelper_SetAccelerationValues` and
`StrafeHelper_Draw` where appropriate.

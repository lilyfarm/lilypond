#ifndef RENDER_H
#define RENDER_H

#include "base.h"
#include <stdbool.h>

// contains all the rendering functionality

// render_create initializes all the subsystems necessary for rendering. It
// returns 0 on success and -1 on failure. It also logs errors.
int render_create(void);

// render_destroy cleans up all the state that was created for rendering.
void render_destroy(void);

// render_fullscreen toggles full screen. It sets full screen if fullscreen
// is true, and unsets it otherwise. Returns 0 on success, -1 on failure.
int render_fullscreen(bool fullscreen);

// render_iterate completes one iteration of rendering all objects that have not
// been removed. This should be called once per frame so e.g if your frames per
// second are ~30, this should be called ~30 times in one second. Returns 0 on
// success, -1 on failure.
int render_iterate(void);
#endif // RENDER_H

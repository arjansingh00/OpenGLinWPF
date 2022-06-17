#pragma once

// gl.h
#include <gl/gl.h>

// glu.h for gluViewport
#include <gl/glu.h>

// Statically link the OpenGL and GLU libs
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
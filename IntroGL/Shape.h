#ifndef _Shape_h_
#define _Shape_h_

#include <GL/glew.h>
class Shape
{

public:
	Shape(float in_x, float in_y, float in_width, float in_height);
	Shape();
	~Shape();

	void syncVbo();

	float x;
	float y;
	float width;
	float height;
	GLuint vbo;
};

#endif


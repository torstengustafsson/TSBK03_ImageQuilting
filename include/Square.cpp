#include "Square.h"
/*
Square::Square(const float& x, const float& y, const float& w, const float& h) :
	square {
			-1,-1, 0,
			-1, 1, 0,
			 1, 1, 0,
			 1,-1, 0
		   },
	squareTexCoord {
					x  , y,
					x  , y+h,
					x+w, y+h,
					x+w, y
				   },
	squareIndices {0, 1, 2, 0, 2, 3}
{
	squareModel = LoadDataToModel(
			square, NULL, squareTexCoord, NULL,
			squareIndices, 4, 6);
}
*/
Square::Square(const float _x, const float _y, const float w, const float h) :
	square {
		-1 + 2*_x    , -1 + 2*_y    , 0, // 0
		-1 + 2*_x    , -1 + 2*(_y+h), 0, // 1
		-1 + 2*(_x+w), -1 + 2*(_y+h), 0, // 2
		-1 + 2*(_x+w), -1 + 2*_y    , 0  // 3
	   },
	squareTexCoord {
		_x  , _y,   // 0
		_x  , _y+h, // 1
		_x+w, _y+h, // 2
		_x+w, _y    // 3
	   },
	squareIndices { 0, 1, 2, 0, 2, 3 }
{
	squareModel = LoadDataToModel(
			square, NULL, squareTexCoord, NULL,
			squareIndices, 4, 6);
	x = _x;
	y = _y;
	width = w;
	height = h;
}

Square::Square(const Square& s) : Square(s.x, s.y, s.width, s.height) {};

void Square::set_position(const float _x, const float _y, const float w, const float h)
{
	width = w != 0 ? w : width;
	height = h != 0 ? h : height;
	x = _x;
	y = _y;
	square[0] = -1 + 2*x;
	square[1] = -1 + 2*y;
	square[3] = -1 + 2*x;
	square[4] = -1 + 2*(y+height);
	square[6] = -1 + 2*(x+width);
	square[7] = -1 + 2*(y+height);
	square[9] = -1 + 2*(x+width);
	square[10] = -1 + 2*y;

	/*
	new positions and possible new width and height
		-1 + 2*x    , -1 + 2*y    , 0
		-1 + 2*x    , -1 + 2*(y+h), 0
		-1 + 2*(x+w), -1 + 2*(y+h), 0
		-1 + 2*(x+w), -1 + 2*y    , 0
	*/
	
	squareModel->vertexArray = square;	

	glBindVertexArray(squareModel->vao);	
	glBindBuffer(GL_ARRAY_BUFFER, squareModel->vb);
	glBufferData(GL_ARRAY_BUFFER, squareModel->numVertices*3*sizeof(GLfloat), squareModel->vertexArray, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, squareModel->tb);
	glBufferData(GL_ARRAY_BUFFER, squareModel->numVertices*2*sizeof(GLfloat), squareModel->texCoordArray, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, squareModel->ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, squareModel->numIndices*sizeof(GLuint), squareModel->indexArray, GL_STATIC_DRAW);
}

void Square::move_texture(const float _x, const float _y, const float w, const float h)
{
	width = w != 0 ? w : width;
	height = h != 0 ? h : height;
	x = _x;
	y = _y;
	squareTexCoord[0] = x;
	squareTexCoord[1] = y;
	squareTexCoord[2] = x;
	squareTexCoord[3] = y+height;
	squareTexCoord[4] = x+width;
	squareTexCoord[5] = y+height;
	squareTexCoord[6] = x+width;
	squareTexCoord[7] = y;
	set_position(x, y, width, height);
}

Model* Square::get()
{
	return squareModel;
}
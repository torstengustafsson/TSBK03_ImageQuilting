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
Square::Square(const float x, const float y, const float w, const float h) :
	square {
			-1 + 2*x    , -1 + 2*y    , 0, // 0
			-1 + 2*x    , -1 + 2*(y+h), 0, // 1
			-1 + 2*(x+w), -1 + 2*(y+h), 0, // 2
			-1 + 2*(x+w), -1 + 2*y    , 0  // 3
		   },
	squareTexCoord {
					x  , y,   // 0
					x  , y+h, // 1
					x+w, y+h, // 2
					x+w, y    // 3
				   },
	squareIndices {0, 1, 2, 0, 2, 3}
{
	squareModel = LoadDataToModel(
			square, NULL, squareTexCoord, NULL,
			squareIndices, 4, 6);
	width = w;
	height = h;
}

void Square::set_position(const float x, const float y, const float w, const float h)
{
	width = w != 0 ? w : width;
	height = h != 0 ? h : height;
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

	squareModel = LoadDataToModel(
			square, NULL, squareTexCoord, NULL,
			squareIndices, 4, 6);
}

Model* Square::get()
{
	return squareModel;
}
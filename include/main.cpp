
#include "ImageQuilter.h"

ImageQuilter* iq;

void OnTimer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(5, &OnTimer, value);
}

//-------------------------------callback functions------------------------------------------
void display(void)
{
	iq->render();
}

void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
}

// This function is called whenever the computer is idle
// As soon as the machine is idle, ask GLUT to trigger rendering of a new
// frame
void idle()
{
  glutPostRedisplay();
}

//-----------------------------main-----------------------------------------------
int main(int argc, char *argv[])
{
	cout <<
	"\n##########################################################"
	"\nDemo application for an image quilting algorithm" << 
	"\ndeveloped at Linköping University for the course TSBK03." <<
	"\nMade by Torsten Gustafsson (2016)." <<
	"\n\nApplication may be run with command line arguments:" << 
	"\nImageQuilting 'imagename.tga' 'output size' 'patch size' 'method'" <<
	"\n\n'imagename.tga' = filename of the input image. Currently only handles tga." <<
	"\n'output size' = size in pixels of the final image." <<
	"\n'patch size' = size in pixels of the patches to be drawn from original image." <<
	"\n'method' have 3 options:" <<
	"\n0 = random placement of blocks." <<
	"\n1 = similar method, but with overlapping blocks." <<
	"\n2 = min-error bounds. Avoids choosing bad patches." <<
	"\n##########################################################\n\n";

	glutInit(&argc, argv);

	//load texture
	TextureData tex;

	bool ok =false;

	int size = 512;
	if(argc > 2) { // size argument need to load first
		size = atoi(argv[2]);
	}

	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(size, size);

	glutInitContextVersion(3, 2);
	glutCreateWindow ((char*)"Image Quilting");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);

	if(argc > 1 && !(ok = LoadTGATexture(argv[1], &tex))) {
		cout << "\nError loading file (does filename '" << argv[1] << "' exist?)\nLoading noise image instead..\n\n";
		LoadTGATexture((char*)"images/noise.tga", &tex);
	}

	int synthesis_size = 64;
	if(argc > 3) {
		synthesis_size = atoi(argv[3]);
	}

	iq = new ImageQuilter(size, size, synthesis_size, tex);
	if(argc > 4) {
		if(atoi(argv[4]) == 0)
			iq->create_image_random();
		if(atoi(argv[4]) == 1)
			iq->create_image_neighbor();
		if(atoi(argv[4]) == 2)
			iq->create_image_minerror();
	}
	else // default
		iq->create_image_random();

	iq->saveImage();

	glutTimerFunc(5, &OnTimer, 0);
	glutMainLoop();
	exit(0);
}


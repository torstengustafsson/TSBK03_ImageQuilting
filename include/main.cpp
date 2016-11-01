
#include "ImageQuilter.h"


// initial width and heights
#define W 512
#define H 512

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
	"\nImageQuilting 'imagename.tga' 'size' 'method'" <<
	"\n\n'imagename.tga' = filename of the input image. Currently only handles tga." <<
	"\n'size' = size in pixels of the patches to be drawn from original image." <<
	"\n'method' have 3 options:" <<
	"\n0 = random placement of blocks." <<
	"\n1 = similar method, but with overlapping blocks." <<
	"\n2 = min-error bounds. Avoids choosing bad patches." <<
	"\n##########################################################\n\n";

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(W, H);

	glutInitContextVersion(3, 2);
	glutCreateWindow ((char*)"Image Quilting");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);

	//load texture
	TextureData tex;

	char* file;
	bool ok =false;
	
	if(argc > 1) {
		file = argv[1];
		if(!(ok = LoadTGATexture(file, &tex)))
			cout << "\nError loading file (does filename '" << file << "' exist?)\nLoading noise image instead..\n\n";
	}
	
	if(!ok)
		LoadTGATexture((char*)"images/noise.tga", &tex);

	int synthesis_size = 64;
	if(argc > 2) {
		synthesis_size = atoi(argv[2]);
	}

	iq = new ImageQuilter(512, 512, synthesis_size, tex);

	if(argc > 3) {
		if(atoi(argv[3]) == 0)
			iq->create_image_random();
		if(atoi(argv[3]) == 1)
			iq->create_image_neighbor();
		if(atoi(argv[3]) == 2)
			iq->create_image_minerror();
	}
	else // default
		iq->create_image_random();

	iq->saveImage();

	glutTimerFunc(5, &OnTimer, 0);
	glutMainLoop();
	exit(0);
}


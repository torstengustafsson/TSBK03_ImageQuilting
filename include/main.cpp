
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
	GLfloat ratio = (GLfloat) w / (GLfloat) h;
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
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(W, H);

	glutInitContextVersion(3, 2);
	glutCreateWindow ("Image Quilting");
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
			cout << "\nError loading file (does filename '" << file << "'' exist?)\nLoading noise image instead..\n\n";
	}
	
	if(!ok)
		LoadTGATexture("images/noise.tga", &tex);

	int synthesis_size = 64;
	if(argc > 2) {
		synthesis_size = atoi(argv[2]);
	}

	iq = new ImageQuilter(512, 512, synthesis_size, tex);
	iq->saveImage();

	glutTimerFunc(5, &OnTimer, 0);
	glutMainLoop();
	exit(0);
}


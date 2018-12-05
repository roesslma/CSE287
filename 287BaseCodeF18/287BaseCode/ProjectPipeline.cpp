#include <ctime> 
#include <iostream> 
#include <algorithm>
#include <cmath>

#include "EShape.h"
#include "FrameBuffer.h"
#include "Raytracer.h"
#include "IScene.h"
#include "Light.h"
#include "Camera.h"
#include "Utilities.h"
#include "VertexOps.h"

PositionalLightPtr theLight = new PositionalLight(glm::vec3(2, 1, 3), pureWhiteLight);
std::vector<LightSourcePtr> lights = { theLight };

glm::vec3 position(0, 1, 4);
float angle = 0, hAngle = 0, vAngle = 0;
bool isMoving = true;
bool twoViewOn = false;
const float SPEED = 0.1;

FrameBuffer frameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);

//EShapeData plane = EShape::createECheckerBoard(copper, tin, 10, 10, 10);
EShapeData plane = EShape::createECheckerBoard(silver, blackPlastic, 10, 10, 10);

//EShapeData cone = EShape::createECone(gold, 2, 2, 10, 10);
//EShapeData cylinder = EShape::createECylinder(chrome, 3, 1, 10, 10);

void renderObjects() {
	//VertexOps::render(frameBuffer, cylinder, lights, glm::mat3(T(1, 2, 3)));
	//VertexOps::render(frameBuffer, cone, lights, glm::mat3());
	VertexOps::render(frameBuffer, plane, lights, glm::mat3());
}

static void render() {
	frameBuffer.clearColorAndDepthBuffers();
	int width = frameBuffer.getWindowWidth();
	int height = frameBuffer.getWindowHeight();
	VertexOps::viewingTransformation = glm::lookAt(position, ORIGIN3D, Y_AXIS);
	float AR = (float)width / height;
	VertexOps::projectionTransformation = glm::perspective(glm::radians(125.0), 2.0, 0.1, 5.0);
	VertexOps::setViewport(0, width - 1, 0, height - 1);
	renderObjects();
	frameBuffer.showColorBuffer();
}

void resize(int width, int height) {
	frameBuffer.setFrameBufferSize(width, height);
	float AR = (float)width / height;

	VertexOps::setViewport(0, width - 1, 0, height - 1);
	VertexOps::projectionTransformation = glm::perspective(M_PI_3, AR, 0.5f, 80.0f);

	glutPostRedisplay();
}
void keyboard(unsigned char key, int x, int y) {
	const float INC = 0.5;
	switch (key) {
	case 'X':
	case 'x': theLight->lightPosition.x += (isupper(key) ? INC : -INC);
				std::cout << theLight->lightPosition << std::endl;
				break;
	case 'Y':
	case 'y': theLight->lightPosition.y += (isupper(key) ? INC : -INC);
				std::cout << theLight->lightPosition << std::endl;
				break;
	case 'Z':
	case 'z':	theLight->lightPosition.z += (isupper(key) ? INC : -INC);
				std::cout << theLight->lightPosition << std::endl;
				break;
	case 'P':
	case 'p':	isMoving = !isMoving;
				break;
	case 'C':	// Do something here
	case 'c':	angle += hAngle;
				hAngle = 0;
				break;
	case '?':	twoViewOn = !twoViewOn;
				break;
	case ESCAPE:
		glutLeaveMainLoop();
		break;
	default:
		std::cout << (int)key << "unmapped key pressed." << std::endl;
	}

	glutPostRedisplay();
}

static void special(int key, int x, int y) {
	static const float rotateInc = glm::radians(10.0);
	static const double minEL = -glm::radians(80.0);
	static const double maxEL = glm::radians(80.0);
	static const double minAZ = -glm::radians(90.0);
	static const double maxAZ = glm::radians(90.0);
	std::cout << key << std::endl;
	switch (key) {
	case(GLUT_KEY_LEFT):	hAngle -= rotateInc;
		if (hAngle < minAZ) {
			hAngle = minAZ;
		}
		break;
	case(GLUT_KEY_RIGHT):	hAngle += rotateInc;
		if (hAngle > maxAZ) {
			hAngle = maxAZ;
		}
		break;
	case(GLUT_KEY_DOWN):	vAngle -= rotateInc;
		if (vAngle < minEL) {
			vAngle = minEL;
		}
		break;
	case(GLUT_KEY_UP):		vAngle += rotateInc;
		if (vAngle > maxEL) {
			vAngle = maxEL;
		}
		break;
	}
	glutPostRedisplay();
}

static void timer(int id) {
	// You should change this.
	while (isMoving) {
		angle += glm::radians(5.0);
	}
	glutTimerFunc(100, timer, 0);
	glutPostRedisplay();
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	GLuint world_Window = glutCreateWindow(extractBaseFilename(__FILE__).c_str());
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutDisplayFunc(render);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutTimerFunc(100, timer, 0);
	glutMouseFunc(mouseUtility);

	frameBuffer.setClearColor(lightGray);

	glutMainLoop();

	return 0;
}

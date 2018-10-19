#include <ctime>
#include <vector>
#include "defs.h"
#include "Utilities.h"
#include "FrameBuffer.h"
#include "ColorAndMaterials.h"
#include "Rasterization.h"

FrameBuffer frameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);

void closed5x5Square(int x, int y, color C) {
	drawLine(frameBuffer, x - 2, y - 2, x + 2, y - 2, C);
	drawLine(frameBuffer, x - 2, y - 1, x + 2, y - 1, C);
	drawLine(frameBuffer, x - 2,     y, x + 2,     y, C);
	drawLine(frameBuffer, x - 2, y + 1, x + 2, y + 1, C);
	drawLine(frameBuffer, x - 2, y + 2, x + 2, y + 2, C);
}

void closed5x5Square(const glm::vec2 &centerPt, color C) {
	closed5x5Square(centerPt.x, centerPt.y, C);
}

void open5x5Square(const glm::vec2 &centerPt, color C) {
	drawLine(frameBuffer, centerPt.x - 2, centerPt.y - 2, centerPt.x + 2, centerPt.y - 2, C);
	drawLine(frameBuffer, centerPt.x - 2, centerPt.y + 1, centerPt.x - 2, centerPt.y - 1, C);
	drawLine(frameBuffer, centerPt.x + 2, centerPt.y + 1, centerPt.x + 2, centerPt.y - 1, C);
	drawLine(frameBuffer, centerPt.x - 2, centerPt.y + 2, centerPt.x + 2, centerPt.y + 2, C);
}

void pieChart(const glm::vec2 &centerPt, float rad, float perc, const color &C1, const color &C2) {
	drawArc(frameBuffer, centerPt, rad, perc * M_PI, (1-perc) * M_2PI, C1);
	drawArc(frameBuffer, glm::vec2(centerPt.x + 20, centerPt.y), rad, (1-(perc/2)) * M_2PI, perc * M_2PI, C2);
	
	drawLine(frameBuffer, centerPt, pointOnCircle(centerPt, rad, (perc / 2) * M_2PI), C1);
	drawLine(frameBuffer, centerPt, pointOnCircle(centerPt, rad, (1 -(perc / 2)) * M_2PI), C1);

	drawLine(frameBuffer, glm::vec2(centerPt.x + 20, centerPt.y), pointOnCircle(glm::vec2(centerPt.x + 20, centerPt.y), rad, (perc / 2) * M_2PI), C2);
	drawLine(frameBuffer, glm::vec2(centerPt.x + 20, centerPt.y), pointOnCircle(glm::vec2(centerPt.x + 20, centerPt.y), rad, (1 - (perc / 2)) * M_2PI), C2);
}

void render() {
	frameBuffer.clearColorAndDepthBuffers();
	closed5x5Square(50, 50, red);
	closed5x5Square(glm::vec2(100, 50), green);
	open5x5Square(glm::vec2(150, 50), blue);
	pieChart(glm::vec2(250, 100), 50, 0.25, red, green);
	frameBuffer.showColorBuffer();
}

void resize(int width, int height) {
	frameBuffer.setFrameBufferSize(width, height);
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case ESCAPE:	glutLeaveMainLoop();
					break;
	}
}

int main(int argc, char *argv[]) {
	glm::vec3 v1{2, 3, 0};
	glm::vec3 v2{4, 3, 0};
	//glm::vec3 v3{5, 0, 0 };

	std::cout << project(v1, v2) << std::endl;
	//std::cout << myNormalize(v2) << std::endl;
	//std::cout << quadratic(10, -5, 1) << std::endl;

	/*glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	GLuint world_Window = glutCreateWindow(__FILE__);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutDisplayFunc(render);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseUtility);

	frameBuffer.setClearColor(black);

	glutMainLoop();*/

	return 0;
}
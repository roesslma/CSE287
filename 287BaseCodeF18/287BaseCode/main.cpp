#include <ctime>
#include <vector>
#include "defs.h"
#include "Utilities.h"
#include "FrameBuffer.h"
#include "ColorAndMaterials.h"

int main(int argc, char *argv[]) {
	std::cout << approximatelyEqual(1.0f, 2.000001f) << std::endl;

	std::cout << normalizeDegrees(420) << std::endl;

	std::cout << min(13, 11, 7) << std::endl;

	std::cout << approximatelyEqual(1.0f, 2.000001f) << std::endl;
	
	return 0;
}
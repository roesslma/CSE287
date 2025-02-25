#pragma once

#include "Utilities.h"
#include "FrameBuffer.h"
#include "Camera.h"
#include "IScene.h"

/**
 * @struct	RayTracer
 * @brief	Encapsulates the functionality of a ray tracer.
 */

struct RayTracer {
	color defaultColor;
	RayTracer(const color &defaultColor);
	void raytraceScene(FrameBuffer &frameBuffer, int depth,
						const IScene &theScene) const;
protected:
	color traceIndividualRay(const Ray &ray, const IScene &theScene, int recursionLevel) const;
};

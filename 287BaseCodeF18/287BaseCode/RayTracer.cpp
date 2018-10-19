#include "RayTracer.h"
#include "IShape.h"

/**
 * @fn	RayTracer::RayTracer(const color &defa)
 * @brief	Constructs a raytracers.
 * @param	defa	The clear color.
 */

RayTracer::RayTracer(const color &defa)
	: defaultColor(defa) {
}

/**
 * @fn	void RayTracer::raytraceScene(FrameBuffer &frameBuffer, int depth, const IScene &theScene) const
 * @brief	Raytrace scene
 * @param [in,out]	frameBuffer	Framebuffer.
 * @param 		  	depth	   	The current depth of recursion.
 * @param 		  	theScene   	The scene.
 */

void RayTracer::raytraceScene(FrameBuffer &frameBuffer, int depth,
								const IScene &theScene) const {
	const RaytracingCamera &camera = *theScene.camera;
	const std::vector<VisibleIShapePtr> &objs = theScene.visibleObjects;
	const std::vector<PositionalLightPtr> &lights = theScene.lights;

	for (int y = 0; y < frameBuffer.getWindowHeight(); ++y) {
		for (int x = 0; x < frameBuffer.getWindowWidth(); ++x) {
			Ray ray = camera.getRay((float)x, (float)y);
			color colorForPixel = traceIndividualRay(ray, theScene, depth);
			frameBuffer.setColor(x, y, colorForPixel);
		}
	}

	frameBuffer.showColorBuffer();
}

/**
 * @fn	color RayTracer::traceIndividualRay(const Ray &ray, const IScene &theScene, int recursionLevel) const
 * @brief	Trace an individual ray.
 * @param	ray			  	The ray.
 * @param	theScene	  	The scene.
 * @param	recursionLevel	The recursion level.
 * @return	The color to be displayed as a result of this ray.
 */

color RayTracer::traceIndividualRay(const Ray &ray, const IScene &theScene, int recursionLevel) const {
	HitRecord theHit = VisibleIShape::findIntersection(ray, theScene.visibleObjects);
	color result = defaultColor;
	
	

	if (theHit.t < FLT_MAX) {
		Ray shadowR(theHit.interceptPoint, glm::normalize(theScene.lights[0]->lightPosition - theHit.interceptPoint));
		HitRecord shadow = VisibleIShape::findIntersection(shadowR, theScene.visibleObjects);

		if (theHit.texture != nullptr) {  // if object has a texture, use it
			float u = glm::clamp(theHit.u, 0.0f, 1.0f);
			float v = glm::clamp(theHit.v, 0.0f, 1.0f);
			result = theHit.texture->getPixel(u, v);
		}

		/*if (shadow.t < FLT_MAX && shadow.t > 0.0001f) {
			result = ambientColor(theHit.material.ambient, theScene.lights[0]->lightColorComponents.ambient);
			// result = totalColor(theHit.material, theScene.lights[0]->lightColorComponents, ray.direction, theHit.surfaceNormal, theScene.lights[0]->lightPosition, theHit.interceptPoint, theScene.lights[0]->attenuationIsTurnedOn, theScene.lights[0]->attenuationParams);
			// result += totalColor(theHit.material, theScene.lights[1]->lightColorComponents, ray.direction, theHit.surfaceNormal, theScene.lights[1]->lightPosition, theHit.interceptPoint, theScene.lights[1]->attenuationIsTurnedOn, theScene.lights[1]->attenuationParams);
		}*/
		else {

			result = illuminate(theHit.interceptPoint, theHit.surfaceNormal, theHit.material, Frame(), false);

			result = totalColor(theHit.material, theScene.lights[0]->lightColorComponents, ray.direction, theHit.surfaceNormal, theScene.lights[0]->lightPosition, theHit.interceptPoint, theScene.lights[0]->attenuationIsTurnedOn, theScene.lights[0]->attenuationParams);
			// result = ambientColor(theHit.material.ambient, theScene.lights[0]->lightColorComponents.ambient);
		}
		
		// result += illuminate(theHit.interceptPoint, theHit.surfaceNormal, theHit.material, Frame, false);
	}
	else {
		// result = ambientColor(theHit.material.ambient, theScene.lights[0]->lightColorComponents.ambient);
		// result += ambientColor(theHit.material.ambient, theScene.lights[1]->lightColorComponents.ambient);
		result = defaultColor;
	}
	return result;
}
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
			float iterX = 1.0f / (camera.nx * 3), iterY = 1.0f / (camera.ny * 3);
			color colorForPixel;
			for (int i = -1; i < 2; i++) {
				for (int j = -1; j < 2; j++) {
					Ray ray = camera.getRay((float)x + (i * iterX), (float)y + (j * iterY));
					colorForPixel += traceIndividualRay(ray, theScene, depth);
				}
			}
			// Ray ray = camera.getRay((float)x, (float)y);
			
			frameBuffer.setColor(x, y, colorForPixel * 0.1112f);
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
	color result;
	
	

	if (theHit.t < FLT_MAX) {

		for (int i = 0; i < theScene.lights.size(); i++) {
			Ray shadowR(theHit.interceptPoint, glm::normalize(theScene.lights[i]->lightPosition - theHit.interceptPoint));
			HitRecord shadow = VisibleIShape::findIntersection(shadowR, theScene.visibleObjects);
			bool inShadow = false;
			Frame f(ray.origin, theScene.camera->cameraFrame.u, theScene.camera->cameraFrame.v, theScene.camera->cameraFrame.w);

			if (shadow.t < FLT_MAX && shadow.t > 0.001f) {
				inShadow = true;
			}

			if (theHit.texture != nullptr) {  // if object has a texture, use it
				float u = glm::clamp(theHit.u, 0.0f, 1.0f);
				float v = glm::clamp(theHit.v, 0.0f, 1.0f);
				result += theHit.texture->getPixel(u, v) * theScene.lights[i]->illuminate(theHit.interceptPoint, theHit.surfaceNormal, theHit.material, f, inShadow);
			}

			/*if (shadow.t < FLT_MAX && shadow.t > 0.01f) {
			result = ambientColor(theHit.material.ambient, theScene.lights[0]->lightColorComponents.ambient);
			// result = totalColor(theHit.material, theScene.lights[0]->lightColorComponents, ray.direction, theHit.surfaceNormal, theScene.lights[0]->lightPosition, theHit.interceptPoint, theScene.lights[0]->attenuationIsTurnedOn, theScene.lights[0]->attenuationParams);
			// result += totalColor(theHit.material, theScene.lights[1]->lightColorComponents, ray.direction, theHit.surfaceNormal, theScene.lights[1]->lightPosition, theHit.interceptPoint, theScene.lights[1]->attenuationIsTurnedOn, theScene.lights[1]->attenuationParams);
			}*/
			else {
				result += theScene.lights[i]->illuminate(theHit.interceptPoint, theHit.surfaceNormal, theHit.material, f, inShadow);
				
				// result = totalColor(theHit.material, theScene.lights[0]->lightColorComponents, ray.direction, theHit.surfaceNormal, theScene.lights[0]->lightPosition, theHit.interceptPoint, theScene.lights[0]->attenuationIsTurnedOn, theScene.lights[0]->attenuationParams);
				// result = ambientColor(theHit.material.ambient, theScene.lights[0]->lightColorComponents.ambient);
			}
		}

		
		
		// result += illuminate(theHit.interceptPoint, theHit.surfaceNormal, theHit.material, Frame, false);
	}
	else {
		// result = ambientColor(theHit.material.ambient, theScene.lights[0]->lightColorComponents.ambient);
		// result += ambientColor(theHit.material.ambient, theScene.lights[1]->lightColorComponents.ambient);
		result = defaultColor;
	}
	if (recursionLevel != 0) {
		result *= traceIndividualRay(Ray(theHit.interceptPoint, ray.direction - 2 * glm::dot(ray.direction, theHit.surfaceNormal)), theScene, recursionLevel - 1);
	}
	
	return glm::clamp(result, { 0 }, { 1 });
}
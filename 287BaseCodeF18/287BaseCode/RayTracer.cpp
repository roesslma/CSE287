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
			float aa = 3, offsX = 1.0f / aa, offsY = 1.0f / aa;
			color colorForPixel;
			if (aa == 3) {
				for (int i = -1; i < 2; ++i) {
					for (int j = -1; j < 2; ++j) {
						Ray ray = camera.getRay((float)x + (i * offsX), (float)y + (j * offsY));
						colorForPixel = colorForPixel + traceIndividualRay(ray, theScene, depth) * 1.0f / (aa*aa);
					}
				}
			}
			else {
				Ray ray = camera.getRay((float)x, (float)y);
				colorForPixel = traceIndividualRay(ray, theScene, depth);
			}
			
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
	color result;

	glm::vec3 offsetpoint = IShape::movePointOffSurface(theHit.interceptPoint, theHit.surfaceNormal);

	if (theHit.t < FLT_MAX) {

		for (int i = 0; i < theScene.lights.size(); i++) {//theScene.lights.size()
			Ray shadowR(offsetpoint, glm::normalize(theScene.lights[i]->lightPosition - offsetpoint));
			HitRecord shadow = VisibleIShape::findIntersection(shadowR, theScene.visibleObjects);
			bool inShadow = false;
			Frame f(ray.origin, theScene.camera->cameraFrame.u, theScene.camera->cameraFrame.v, 
				theScene.camera->cameraFrame.w);

			if (shadow.t < FLT_MAX && shadow.material.alpha == 1.0f) {
				inShadow = true;
			}

			if (theHit.texture != nullptr) {  // if object has a texture, use it
				float u = glm::clamp(theHit.u, 0.0f, 1.0f);
				float v = glm::clamp(theHit.v, 0.0f, 1.0f);
				result += theHit.texture->getPixel(u, v) * theScene.lights[i]->illuminate(theHit.interceptPoint, 
					theHit.surfaceNormal, theHit.material, f, inShadow);
			}
			else {

				if (theHit.material.alpha < 1.0f) {
					result += theHit.material.alpha * theScene.lights[i]->illuminate(theHit.interceptPoint, 
						theHit.surfaceNormal, theHit.material, f, inShadow) + (1 - theHit.material.alpha) * 
						traceIndividualRay(Ray(theHit.interceptPoint, ray.direction), theScene, recursionLevel);
				}
				else {
					result += theScene.lights[i]->illuminate(theHit.interceptPoint, theHit.surfaceNormal, 
						theHit.material, f, inShadow);
				}
				
			}
		}
	}
	else {
		result += defaultColor;
		recursionLevel = 0;
	}

	if (recursionLevel != 0) {
		//glm::vec3 offsetpoint = IShape::movePointOffSurface(theHit.interceptPoint, theHit.surfaceNormal);
//		HitRecord theHit2 = VisibleIShape::findIntersection(ray, theScene.visibleObjects);
		
		color reflectedColor = traceIndividualRay(Ray(offsetpoint, 
			glm::normalize(ray.direction - 2 * glm::dot(ray.direction, theHit.surfaceNormal) * theHit.surfaceNormal)),
			theScene, recursionLevel - 1);
		result += 0.5f * result + 0.5f * reflectedColor;

	}
	
	return glm::clamp(result, { 0 }, { 1 });
}
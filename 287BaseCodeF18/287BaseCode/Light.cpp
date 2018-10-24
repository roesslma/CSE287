#include "Light.h"
#include <algorithm>

/**
 * @fn	color ambientColor(const color &mat, const color &light)
 * @brief	Computes the ambient color produced by a single light at a single point.
 * @param	mat  	Ambient material property.
 * @param	light	Light's ambient color.
 * @return	Ambient color.
  */

color ambientColor(const color &mat, const color &light) {
	return light * mat;
}

/**
 * @fn	color diffuseColor(const color &mat, const color &light, const glm::vec3 &l, const glm::vec3 &n)
 * @brief	Computes diffuse color produce by a single light at a single point.
 * @param	mat		 	Material.
 * @param	light	 	The light.
 * @param	l		 	Light vector.
 * @param	n		 	Normal vector.
 * @return	Diffuse color.
 */

color diffuseColor(const color &mat, const color &light,
					const glm::vec3 &l, const glm::vec3 &n) {
	return light * mat * glm::max(0.0f, glm::dot(l, n));
}

/**
 * @fn	color specularColor(const color &mat, const color &light, float shininess, const glm::vec3 &r, const glm::vec3 &v)
 * @brief	Computes specular color produce by a single light at a single point.
 * @param	mat		 	Material.
 * @param	light	 	The light's color.
 * @param	shininess	Material shininess.
 * @param	r		 	Reflection vector.
 * @param	v		 	Viewing vector.
 * @return	Specular color.
 */

color specularColor(const color &mat, const color &light,
					float shininess,
					const glm::vec3 &r, const glm::vec3 &v) {
	return light * mat * glm::pow(glm::max(0.0f, glm::dot(v, -r)), shininess);
}

/**
 * @fn	color totalColor(const Material &mat, const LightColor &lightColor, const glm::vec3 &viewingDir, const glm::vec3 &normal, const glm::vec3 &lightPos, const glm::vec3 &intersectionPt, bool attenuationOn, const LightAttenuationParameters &ATparams)
 * @brief	Color produced by a single light at a single point.
 * @param	mat			  	Material.
 * @param	lightColor	  	The light's color.
 * @param	v	  			The v vector.
 * @param	normal		  	Normal vector.
 * @param	lightPos	  	Light position.
 * @param	intersectionPt	(x,y,z) of intersection point.
 * @param	attenuationOn 	true if attenuation is on.
 * @param	ATparams	  	Attenuation parameters.
 * @return	Color produced by a single light at a single point.
 */
 
color totalColor(const Material &mat, const LightColor &lightColor,
				const glm::vec3 &v, const glm::vec3 &n,
				const glm::vec3 &lightPos, const glm::vec3 &intersectionPt,
				bool attenuationOn, 
				const LightAttenuationParameters &ATparams) {
	if (DEBUG_PIXEL) {
		std::cout << std::endl;
	}
	glm::vec3 normal = n;
	if (glm::dot(v, n) > 0) { // Checks for backfaces
		normal = -n;
	}
	glm::vec3 l = glm::normalize(lightPos - intersectionPt), r = 2 * glm::dot(l, normal) * normal - l;
	// std::cout << specularColor(mat.specular, lightColor.specular, mat.shininess, r, v) << std::endl;
	color amb = ambientColor(mat.ambient, lightColor.ambient);
	color diff = diffuseColor(mat.diffuse, lightColor.diffuse, l, normal);
	color spec = specularColor(mat.specular, lightColor.specular, mat.shininess, r, v);
	float atten = 1.0f;
	if (attenuationOn) {
		atten = ATparams.factor(glm::distance(lightPos, intersectionPt));
		// return glm::clamp((ambientColor(mat.ambient, lightColor.ambient) + diffuseColor(mat.diffuse, lightColor.diffuse, l, normal) * ATparams.factor(glm::distance(lightPos, intersectionPt)) + specularColor(mat.specular, lightColor.specular, mat.shininess, r, v) * ATparams.factor(glm::distance(lightPos, intersectionPt))), { 0 }, { 1 });
	}
	return glm::clamp((amb + diff * atten + spec * atten), { 0 }, { 1 });
	// return  glm::clamp((ambientColor(mat.ambient, lightColor.ambient) + diffuseColor(mat.diffuse, lightColor.diffuse, l, normal) + specularColor(mat.specular, lightColor.specular, mat.shininess, r, v)), { 0 }, { 1 }); // mat.ambient + mat.diffuse;
}

/**
 * @fn	color PositionalLight::illuminate(const HitRecord &hit, const glm::vec3 &viewingDir, const Frame &eyeFrame, bool inShadow) const
 * @brief	Computes the color this light produces in raytracing applications.
 * @param	hit					The surface properties of the intercept point.
 * @param	v					The v vector
 * @param	eyeFrame			The coordinate frame of the camera.
 * @param	inShadow			true if the point is in a shadow.
 * @return	The color produced at the intercept point, given this light.
 */

color PositionalLight::illuminate(const glm::vec3 &interceptWorldCoords,
									const glm::vec3 &normal,
									const Material &material,
									const Frame &eyeFrame, bool inShadow) const {

	glm::vec3 v = glm::normalize(eyeFrame.origin - interceptWorldCoords);
	
	if (!isOn || inShadow) {
		return ambientColor(material.ambient, this->lightColorComponents.ambient);
		/*return totalColor(material, this->lightColorComponents, -v, normal,
			this->lightPosition, interceptWorldCoords, this->attenuationIsTurnedOn, this->attenuationParams);*/
	}
	else if (!inShadow) {
		return totalColor(material, this->lightColorComponents, -v, normal,
			this->lightPosition, interceptWorldCoords, this->attenuationIsTurnedOn, this->attenuationParams);
	}
}

/**
 * @fn	color SpotLight::illuminate(const HitRecord &hit, const glm::vec3 &viewingDir, const Frame &eyeFrame, bool inShadow) const
 * @brief	Computes the color this light produces in raytracing applications.
 * @param	hit					The surface properties of the intercept point.
 * @param	v					The v vector
 * @param	eyeFrame			The coordinate frame of the camera.
 * @param	inShadow			true if the point is in a shadow.
 * @return	The color produced at the intercept point, given this light.
 */

color SpotLight::illuminate(const glm::vec3 &interceptWorldCoords,
							const glm::vec3 &normal,
							const Material &material,
							const Frame &eyeFrame, bool inShadow) const {

	glm::vec3 v = glm::normalize(eyeFrame.origin - interceptWorldCoords);
	glm::vec3 l = glm::normalize(interceptWorldCoords - this->lightPosition);
	// this->spotDirection

	if (!isOn || inShadow || glm::dot(this->spotDirection, l) > (this->fov / 2.0f)) {
		return ambientColor(material.ambient, this->lightColorComponents.ambient);
	}
	else if (!inShadow) {
		return totalColor(material, this->lightColorComponents, -v, normal,
			this->lightPosition, interceptWorldCoords, this->attenuationIsTurnedOn, this->attenuationParams);
	}
}

/**
* @fn	ostream &operator << (std::ostream &os, const LightAttenuationParameters &at)
* @brief	Output stream for light attenuation parameters.
* @param	os		Output stream.
* @param	at		Attenuation parameters.
* @return	The output stream.
*/

std::ostream &operator << (std::ostream &os, const LightAttenuationParameters &at) {
	os << glm::vec3(at.constant, at.linear, at.quadratic) << std::endl;
	return os;
}

/**
* @fn	ostream &operator << (std::ostream &os, const PositionalLight &pl)
* @brief	Output stream for light attenuation parameters.
* @param	os		Output stream.
* @param	pl		Positional light.
* @return	The output stream.
*/

std::ostream &operator << (std::ostream &os, const PositionalLight &pl) {
	os << (pl.isOn ? "ON" : "OFF") << std::endl;
	os << (pl.isTiedToWorld? "WORLD" : "CAMERA") << std::endl;
	os << " position " << pl.lightPosition << std::endl;
	os << " ambient " << pl.lightColorComponents.ambient << std::endl;
	os << " diffuse " << pl.lightColorComponents.diffuse << std::endl;
	os << " specular " << pl.lightColorComponents.specular << std::endl;
	os << "Attenuation: " << (pl.attenuationIsTurnedOn ? "ON" : "OFF")
		<< " " << pl.attenuationParams << std::endl;
	return os;
}

/**
* @fn	ostream &operator << (std::ostream &os, const SpotLight &sl)
* @brief	Output stream for light attenuation parameters.
* @param	os		Output stream.
* @param	sl		Spotlight.
* @return	The output stream.
*/

std::ostream &operator << (std::ostream &os, const SpotLight &sl) {
	PositionalLight pl = (sl);
	os << pl;
	os << " FOV " << sl.fov << std::endl;
	return os;
}
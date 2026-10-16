
#include <algorithm>
#include <cmath>


#include <TetoEngine/Utils/Easing.hpp>


#include <glm/glm.hpp>

using namespace TetoEngine;

float smoothstep(float edge0, float edge1, float t)
{
	t = std::clamp((t - edge0) / (edge1 - edge0), 0.0f, 1.0f);
	return t * t * (3.0f - 2.0f * t);
}

float easeOutSine(float t)
{
	return std::sin((t * 3.1415926f) / 2.0f);
}

float easeInOutCirc(float t)
{
	t = glm::clamp(t, 0.0f, 1.0f);

	if (t < 0.5f)
	{
		return (1.0f - glm::sqrt(1.0f - glm::pow(2.0f * t, 2.0f))) / 2.0f;
	}
	else
	{
		return (glm::sqrt(1.0f - glm::pow(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f;
	}
}

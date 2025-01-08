#include "Frustrum.h"

Frustrum::Frustrum(const glm::mat4& projection, const glm::mat4& view)
{
	glm::mat4 clip = projection * view;
	// Left plane
	planes[0] = glm::vec4(clip[0][3] + clip[0][0],
						clip[1][3] + clip[1][0], 
						clip[2][3] + clip[2][0],
						clip[3][3] + clip[3][0]);
	// Right plane
	planes[1] = glm::vec4(clip[0][3] - clip[0][0], 
						clip[1][3] - clip[1][0], 
						clip[2][3] - clip[2][0],
						clip[3][3] - clip[3][0]);
	// Bottom plane
	planes[2] = glm::vec4(clip[0][3] + clip[0][1],
						clip[1][3] + clip[1][1], 
						clip[2][3] + clip[2][1],
						clip[3][3] + clip[3][1]);
	// Top plane
	planes[3] = glm::vec4(clip[0][3] - clip[0][1],
						clip[1][3] - clip[1][1],
						clip[2][3] - clip[2][1], 
						clip[3][3] - clip[3][1]);
	// Near plane
	planes[4] = glm::vec4(clip[0][3] + clip[0][2],
						clip[1][3] + clip[1][2],
						clip[2][3] + clip[2][2], 
						clip[3][3] + clip[3][2]);
	// Far plane
	planes[5] = glm::vec4(clip[0][3] - clip[0][2],
						clip[1][3] - clip[1][2],
						clip[2][3] - clip[2][2], 
						clip[3][3] - clip[3][2]);
	// Normalize the planes
	for (auto& plane : planes)
	{
		float length = glm::length(glm::vec3(plane));
		plane /= length;
	}
}

bool Frustrum::IsPointInside(const glm::vec3& point) const
{
	for (const auto& plane : planes)
	{
		if (glm::dot(glm::vec4(point, 1.0f), plane) < 0)
		{
			return false;
		}
	}
	return true;
}

bool Frustrum::IsSphereInside(const glm::vec3 & center, float radius) const
{
	for (const auto& plane : planes)
	{
		if (glm::dot(glm::vec4(center, 1.0f), plane) < -radius)
		{
			return false;
		}
	}
	return true;
}

bool Frustrum::IsBoxInside(const glm::vec3& min, const glm::vec3& max) const
{
	for (const auto& plane : planes)
	{
		glm::vec3 p = min;
		if (plane.x >= 0)
		{
			p.x = max.x;
		}
		if (plane.y >= 0)
		{
			p.y = max.y;
		}
		if (plane.z >= 0)
		{
			p.z = max.z;
		}
		if (glm::dot(glm::vec4(p, 1.0f), plane) < 0)
		{
			return false;
		}
		p = max;
		if (plane.x >= 0)
		{
			p.x = min.x;
		}
		if (plane.y >= 0)
		{
			p.y = min.y;
		}
		if (plane.z >= 0)
		{
			p.z = min.z;
		}
		if (glm::dot(glm::vec4(p, 1.0f), plane) < 0)
		{
			return false;
		}
	}
    return true;
}

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <array>

class Frustrum
{
public:
	Frustrum() = default;
	Frustrum(const glm::mat4& projection, const glm::mat4& view);
	bool IsPointInside(const glm::vec3& point) const;
	bool IsSphereInside(const glm::vec3& center, float radius) const;
	bool IsBoxInside(const glm::vec3& min, const glm::vec3& max) const;

private:
	std::array<glm::vec4, 6> planes;
};

struct BoundingBox {
	glm::vec3 min;
	glm::vec3 max;
};
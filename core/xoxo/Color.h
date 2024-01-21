#include <glm/glm.hpp>

class Color
{
public:
	Color(float r, float g, float b) { color = glm::vec3(r, g, b); };
	Color(float value) { color = glm::vec3(value); };
	Color(glm::vec3 color) { this->color = color; };

	glm::vec3 getColor() { return color; }
	void setColor(float r, float g, float b) { color = glm::vec3(r, g, b); }
	void setColor(float value) { color = glm::vec3(value); }
	void setColor(glm::vec3 value) { color = value; }

	float r = color.x;
	float g = color.y;
	float b = color.z;

private:
	glm::vec3 color;

};
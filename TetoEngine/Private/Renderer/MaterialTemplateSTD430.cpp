#include <TetoEngine/Private/Renderer/MaterialTemplateSTD430.hpp>

using namespace TetoEngine;

size_t MaterialTemplateSTD430::getSize(UniformType type)
{
	switch (type)
	{
		case Float: return 4;
		case Vec2:  return 8;
		case Vec3:  return 12;
		case Vec4:  return 16;
		case Mat4:  return 64;
		default:    return 0;
	}
}

size_t MaterialTemplateSTD430::getAlignment(UniformType type)
{
	switch (type)
	{
		case Float: return 4;
		case Vec2:  return 8;
		case Vec3:  return 16;
		case Vec4:  return 16;
		case Mat4:  return 16;
		default:    return 0;
	}
}
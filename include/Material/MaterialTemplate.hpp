#pragma once

#include <string>
#include <vector>

namespace TLOT
{
	enum UniformType { Vec4, Vec3, Vec2, Float, Mat4, TexQ };

	struct UniformLayout
	{
		UniformType type;
		std::string name;
		size_t      size;
		size_t      offset;
	};

	inline size_t const GetSizeSTD430(UniformType type)
	{
		switch (type)
		{
			case Float: return 4;
			case Vec2:  return 8;
			case Vec3:  return 12;
			case Vec4:  return 16;
			case Mat4:  return 64;
			case TexQ:  return 24;
			default:    return 0;
		}
	}

	inline size_t GetAligmentSTD430(UniformType type)
	{
		switch (type)
		{
			case Float: return 4;
			case Vec2:  return 8;
			case Vec3:  return 16;
			case Vec4:  return 16;
			case Mat4:  return 16;
			case TexQ:  return 8;
			default:    return 0;
		}
	}

	class MaterialTemplateSTD430
	{
	public:
		void AddUniform(std::string name, UniformType type)
		{
			size_t size      = GetSizeSTD430(type);
			size_t alignment = GetAligmentSTD430(type);

			// 1. Calcul du reste de la division entre la taille actuelle et l'alignement requis
			size_t remainder = m_totalSize % alignment;

			// 2. Calcul du padding (l'espace vide à ajouter) pour s'aligner
			size_t padding = (remainder == 0) ? 0 : (alignment - remainder);

			// 3. Le véritable offset est la taille actuelle + le padding
			size_t offset = m_totalSize + padding;

			// Ajout et mise à jour
			m_uniforms.emplace_back(type, name, size, offset);
			m_totalSize = offset + size;
		}

		UniformLayout const * Get(std::string name) const
		{
			for (auto & param : m_uniforms)
			{
				if (param.name == name) return &param;
			}

			return nullptr;
		}

		size_t Size() const
		{
			return m_totalSize;
		}

		std::vector<UniformLayout> const & Layout() const
		{
			return m_uniforms;
		}

	private:
		std::vector<UniformLayout> m_uniforms;
		size_t m_totalSize = 0;
	};
}

#pragma once

#include <type_traits>

#include <Core/ID.hpp>

namespace TLOT
{
	using GeometryID = ID64_t;
	template <typename T> concept arithmetic = std::is_arithmetic_v<T>;

	struct InstanceID
	{
		InstanceID() = default;
		ID64_t Get() const {return m_value;}
		operator bool() const { return m_value != InvalidID64_t; }

	private:
		friend class Renderer;

		template<arithmetic T>
		InstanceID & operator=(T v) { m_value = v; return *this; }
		InstanceID(ID64_t value): m_value {value}{}
		
		ID64_t m_value = InvalidID64_t;
	};

	static InstanceID const InvalidInstance;

	enum class ProjectionMode
	{
		Perspective,
		Orthographic
	};
}
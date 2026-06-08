#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include <Core/ResourceHandle.hpp>
#include <Material/MaterialTemplate.hpp>

#include <AssetManager.hpp>

#include <cstring>

namespace TLOT
{
	class MaterialInstance
	{
	public:
		MaterialInstance(ResourceHandle materialTemplate, size_t materialSize);

		template<class T>
		void Set (std::string name, T && value)
		{
			auto materialTemplate = AssetManager::GetMaterialTemplate(m_template);
			if (!materialTemplate.HasValue()) return;

			auto param = materialTemplate->Get(name);

			if (!param)
				return; // TODO ; log error

			if (sizeof (T) > param->size)
				return; // TODO: Log error

			memcpy(m_data.data() + param->offset, &value, sizeof(T));
		}

		uint64_t Version() const;
		void BumpVersion();

		std::vector<uint8_t> const Data() const;
		std::vector<uint8_t> & GetData();
		size_t const Size() const;

		uint64_t GetTemplateHandle() const;

	private:
		ResourceHandle m_template;
		std::vector<uint8_t> m_data;

		uint64_t m_version = 0;
	};
}

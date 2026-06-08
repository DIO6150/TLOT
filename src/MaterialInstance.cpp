#include <Material/MaterialInstance.hpp>

using namespace TLOT;

MaterialInstance::MaterialInstance(ResourceHandle materialTemplate, size_t materialSize):
	m_template {materialTemplate}
{
	m_data.resize(materialSize);
}

uint64_t MaterialInstance::Version () const
{
	return m_version;
}

void MaterialInstance::BumpVersion()
{
	++m_version;
}

std::vector<uint8_t> const MaterialInstance::Data () const
{
	return m_data;
}

std::vector<uint8_t> & MaterialInstance::GetData()
{
	return m_data;
}

size_t const MaterialInstance::Size () const
{
	return m_data.size();
}


uint64_t MaterialInstance::GetTemplateHandle() const
{
	return m_template;
}
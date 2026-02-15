#include <engine/core/AssetManager.hpp>
#include <engine/core/ResourceManager.hpp>

#include <engine/utils/Types.hpp>
#include <engine/utils/Utils.hpp>


using namespace Engine::Core;
using namespace Engine::Data;

TextureID AssetManager::LoadTexture (const String & name, const String & url) {
	ObjectID * resource_ptr = textureNames.get_ptr (name);
	if (resource_ptr) {
		return (*resource_ptr);
	}
	
	unsigned char * data;
	size_t width, height;
	if (load_texture (url, data, width, height)) {
		ObjectID resource = textureIDs.create (data, width, height);
		textureNames.add (name, resource);

		return (resource);
	} else {
		return (ObjectID::invalid ());
	}


}

ShaderID AssetManager::LoadShader (const String & name, const String & url_vertex, const String & url_fragment) {
	ObjectID * resource_ptr = shaderNames.get_ptr (name);
	if (resource_ptr) {
		return (*resource_ptr);
	}

	ObjectID resource = shaderIDs.create (url_vertex, url_fragment);
	shaderNames.add (name, resource);

	return (resource);
}

MaterialID AssetManager::CreateMaterial (const String & name, ShaderID shader, Vector<TextureID> diffuse) {
	ObjectID * resource_ptr = materialNames.get_ptr (name);
	if (resource_ptr) {
		return (*resource_ptr);
	}

	ObjectID resource = materialIDs.create (shader, diffuse);
	materialNames.add (name, resource);

	return (resource);
}

GeometryID AssetManager::CreateGeometry (const String & name, Vector<Vertex> vertices, Vector<uint32_t> indices) {
	ObjectID * resource_ptr = geometryNames.get_ptr (name);
	if (resource_ptr) {
		return (*resource_ptr);
	}

	ObjectID resource = geometryIDs.create (vertices, indices);
	geometryNames.add (name, resource);

	return (resource);
}

std::optional<MaterialID> AssetManager::GetMaterialID (const String & name) {
	ObjectID * pos = materialNames.get_ptr (name);
	return pos ? std::optional {*pos} : std::nullopt;
}

std::optional<GeometryID> AssetManager::GetGeometryID (const String & name) {
	ObjectID * pos = geometryNames.get_ptr (name);
	return pos ? std::optional {*pos} : std::nullopt;
}

Shader & AssetManager::GetShader (ShaderID resource) const {
	return (shaderIDs.getRef (resource));
}

Material & AssetManager::GetMaterial (MaterialID resource) const {
	return (materialIDs.getRef (resource));
}

Geometry & AssetManager::GetGeometry (GeometryID resource) const {
	return (geometryIDs.getRef (resource));
}

Texture & AssetManager::GetTexture (TextureID resource) const {
	return (textureIDs.getRef (resource));
}
#pragma once

#include <engine/core/ResourceManager.hpp>

#include <engine/utils/Types.hpp>

#include <engine/data/Geometry.hpp>
#include <engine/data/Material.hpp>
#include <engine/data/Mesh.hpp>
#include <engine/data/Shader.hpp>
#include <engine/data/Texture.hpp>

#include <engine/utils/ShortcutTypes.hpp>

#include <optional>
#include <string>

namespace Engine::Core {
	template<class T>
	class RefMap {
	public:
		void add (String name, T ref) {
			t_to_str[ref]	= name;
			str_to_t[name]	= ref;
		}

		T * get_ptr (String name) {
			auto pos = str_to_t.find (name);
			if (pos != str_to_t.end ()) return (&pos->second);
			return (nullptr);
		}

	private:
		std::unordered_map<T, String> t_to_str;
		std::unordered_map<String, T> str_to_t;
	};

	// in the future, resources like shaders and textures should not know about their own url and should instead receive raw data
	class AssetManager {
	public:
		TextureID	LoadTexture (const String & name, const String & url);
		ShaderID	LoadShader (const String & name, const String & url_vertex, const String & url_fragment);
		MaterialID	CreateMaterial (const String & name, ShaderID shader, Vector<TextureID> diffuse);
		GeometryID	CreateGeometry (const String & name, Vector<Data::Vertex> vertices, Vector<uint32_t> indices);


		std::optional<MaterialID> GetMaterialID (const String & name);
		std::optional<GeometryID> GetGeometryID (const String & name);


		Data::Shader   & GetShader (ShaderID resource) const;
		Data::Material & GetMaterial (MaterialID resource) const;
		Data::Geometry & GetGeometry (GeometryID resource) const;
		Data::Texture  & GetTexture (TextureID resource) const;



	private:
		ResourceManager<Data::Geometry>	geometryIDs;
		RefMap<GeometryID>		geometryNames;
		
		ResourceManager<Data::Shader>	shaderIDs;
		RefMap<ShaderID>		shaderNames;

		ResourceManager<Data::Texture>	textureIDs;
		RefMap<TextureID>		textureNames;

		ResourceManager<Data::Material>	materialIDs;
		RefMap<MaterialID>		materialNames;
	};
}

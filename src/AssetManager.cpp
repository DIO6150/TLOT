#include <engine/core/AssetManager.hpp>
#include <engine/core/ResourceManager.hpp>

#include <engine/utils/Types.hpp>
#include <engine/utils/Utils.hpp>

using namespace Engine::Core;
using namespace Engine::Data;
using namespace Engine::Internal;

AssetManager * AssetManager::mInstance = nullptr;

static Vector<String> getTexturePaths (aiMaterial * material, aiTextureType type) {
	Vector<String> paths;
	for(unsigned int i = 0; i < material->GetTextureCount(type); ++i) {
		aiString str;
		material->GetTexture(type, i, &str);
		paths.push_back (std::string {str.C_Str ()});
	}
	
	return (paths);
}

static Vector<Vertex> CreateVertices (aiMesh * mesh) {
	Vector<Vertex> _vertices;

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		float _tex_coord_x = 0.0f;
		float _tex_coord_y = 0.0f;

		if (mesh->HasTextureCoords (0)) { // TODO-fix: maybe try to have all tex coords, idk how to implement it after that...
			_tex_coord_x = mesh->mTextureCoords[0][i].x;
			_tex_coord_y = mesh->mTextureCoords[0][i].y;
		}

		_vertices.emplace_back (
			mesh->mVertices[i].x,		mesh->mVertices[i].y,		mesh->mVertices[i].z,
			mesh->mNormals[i].x,		mesh->mNormals[i].y,		mesh->mNormals[i].z,
			mesh->mTangents[i].x,		mesh->mTangents[i].y,		mesh->mTangents[i].z,
			_tex_coord_x,			_tex_coord_y
		);
	}

	return (_vertices);
}


static Vector<uint32_t> CreateIndices (aiMesh * mesh) {
	Vector<uint32_t> _indices;

	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace _face = mesh->mFaces[i];

		for(unsigned int j = 0; j < _face.mNumIndices; ++j) {	
			_indices.push_back (_face.mIndices[j]);
		}
	}

	return (_indices);
}

// TODO: what if we want to multi-thread, do we have 2 instances ? not good if so
AssetManager * AssetManager::GetInstance () {
	if (!mInstance) {
		mInstance = new AssetManager {};
	}
	return mInstance;
}

AssetManager::AssetManager () {
	// TODO: make it load from code rather than from direct assets

	unsigned char *textureData = new unsigned char[4];
	textureData[0] = 255;
	textureData[1] = 255;
	textureData[2] = 255;
	textureData[3] = 255;
	defaultTextureID = textureIDs.create (textureData, 1, 1);
	textureNames.add ("tlot_default", defaultTextureID);

	textureData = new unsigned char[16];
	textureData[0]  = 0;
	textureData[1]  = 0;
	textureData[2]  = 0;
	textureData[3]  = 255;

	textureData[4]  = 255;
	textureData[5]  = 0;
	textureData[6]  = 0;
	textureData[7]  = 255;

	textureData[8]  = 0;
	textureData[9]  = 0;
	textureData[10] = 0;
	textureData[11] = 255;

	textureData[12] = 255;
	textureData[13] = 0;
	textureData[14] = 0;
	textureData[15] = 255;

	missingTextureID = textureIDs.create (textureData, 1, 1);
	textureNames.add ("tlot_missing", missingTextureID);


	//defaultTextureID = LoadTexture ("default", "data/assets/textures/default.png");
	
	defaultShaderID  = LoadShader ("tlot_default", "data/assets/shaders/default.vertex", "data/assets/shaders/default.fragment");

	defaultMaterialID = CreateMaterial ("tlot_default", {defaultTextureID}, {1.0, 1.0, 1.0});
	
}

HandleID AssetManager::ProcessMesh (aiMesh * assimpMesh, aiMaterial * assimpMaterial, const std::string & directory) {
	String			_meshName;
	String			_materialName;

	HandleID		_verticesID;
	HandleID		_indicesID;
	HandleID		_materialID;
	Vector<HandleID>	_diffuseIDS;

	_meshName = directory + "/" + assimpMesh->mName.C_Str ();

	auto meshID = GetMeshID (_meshName);
	if (meshID.isValid () ) {
		return (meshID);
	}

	Vector<Vertex>	 _vertices = CreateVertices (assimpMesh);
	Vector<uint32_t> _indices  = CreateIndices  (assimpMesh);

	_verticesID = mVerticesIDs.Push (_vertices);
	if (!_verticesID.isValid ()) {
		printf ("failed to allocate vertices\n");
	}

	_indicesID = mIndicesIDs.Push (_indices);
	if (!_indicesID.isValid ()) {
		printf ("failed to allocate indices\n");
	}

	_materialName = assimpMaterial->GetName ().C_Str ();
	
	_materialID = GetMaterialID (_materialName);
	if (!_materialID.isValid ()) {
		for (auto _path: getTexturePaths (assimpMaterial, aiTextureType_DIFFUSE)) {
			std::string _texName = std::filesystem::path {_path}.stem ().string ();
			HandleID _diffuse = LoadTexture (_texName, directory + "/" + _path);

			if (!_diffuse.isValid ()){
				_diffuse = missingTextureID;
			}

			_diffuseIDS.push_back (_diffuse);
		}

		if (_diffuseIDS.empty ()){
			_diffuseIDS.push_back (defaultTextureID);
		}

		aiColor3D _diffuseColorAssimp;
		assimpMaterial->Get (AI_MATKEY_COLOR_DIFFUSE, _diffuseColorAssimp);

		_materialID = CreateMaterial (_materialName, _diffuseIDS, {_diffuseColorAssimp.r, _diffuseColorAssimp.g, _diffuseColorAssimp.b});
	}

	HandleID _resource = meshIDs.create (_verticesID, _indicesID, _materialID, glm::vec3 {0.0}, glm::vec3 {0.0}, glm::vec3 {1.0}, _meshName);
	meshNames.add (_meshName, _resource);

	return (_resource);
}

HandleID AssetManager::LoadTexture (const String & name, const String & url) {
	HandleID * resource_ptr = textureNames.get_ptr (name);
	if (resource_ptr) {
		return *resource_ptr;
	}
	
	unsigned char * data;
	size_t width, height;
	if (load_texture (url, data, width, height)) {
		HandleID resource = textureIDs.create (data, width, height);
		textureNames.add (name, resource);

		return resource;
	}

	std::cout << "Couldn't create texture:" << name << ", defaulting.\n";
	return HandleID::invalid ();
}

HandleID AssetManager::LoadShader (const String & name, const String & url_vertex, const String & url_fragment) {
	HandleID * resource_ptr = shaderNames.get_ptr (name);
	if (resource_ptr) {
		return *resource_ptr;
	}

	HandleID resource = shaderIDs.create (url_vertex, url_fragment);
	shaderNames.add (name, resource);

	return resource;
}

HandleID AssetManager::LoadModel (const String & name, const String & url) {
	HandleID * resource_ptr = mModelNames.get_ptr (name);

	if (resource_ptr) {
		return *resource_ptr;
	}


	Assimp::Importer _importer;
	std::string _directory = url.substr(0, url.find_last_of('/'));

	const aiScene * _scene = _importer.ReadFile (url, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

	if (!_scene || _scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !_scene->mRootNode) {
		std::string _error {_importer.GetErrorString ()};
		std::cout << "Assimp error " << _error << "\n";
		return (HandleID::invalid ());
	}
	
	Vector<HandleID> _meshes;

	for (uint32_t i = 0; i < _scene->mNumMeshes; ++i) {
		printf ("Mesh loaded : %u/%u\n", i + 1, _scene->mNumMeshes);
		_meshes.push_back (ProcessMesh (
				_scene->mMeshes[i],
				_scene->mMaterials[_scene->mMeshes[i]->mMaterialIndex],
				_directory
			)
		);
	}

	HandleID resource = mModelIDs.create (_meshes, name);
	mModelNames.add (name, resource);

	return (resource);
}

HandleID AssetManager::CreateMaterial (const String & name, Vector<HandleID> diffuse, glm::vec3 color) {
	HandleID * resource_ptr = materialNames.get_ptr (name);
	if (resource_ptr) {
		return (*resource_ptr);
	}

	HandleID resource = materialIDs.create (diffuse, color);
	materialNames.add (name, resource);

	return (resource);
}

HandleID AssetManager::GetShaderID (const String & name) const {
	HandleID * pos = shaderNames.get_ptr (name);
	return pos ? *pos : HandleID::invalid ();
}

HandleID AssetManager::GetTextureID (const String & name) const {
	HandleID * pos = textureNames.get_ptr (name);
	return pos ? *pos : HandleID::invalid ();
}

const std::string AssetManager::GetTextureName (const HandleID & handle) const {
	const String * name = textureNames.get_name (handle);
	return name ? *name : "invalid";
}

HandleID AssetManager::GetMaterialID (const String & name) const {
	HandleID * pos = materialNames.get_ptr (name);
	return pos ? *pos : HandleID::invalid ();
}

HandleID AssetManager::GetMeshID (const String & name) const {
	HandleID * pos = meshNames.get_ptr (name);
	return pos ? *pos : HandleID::invalid ();
}

HandleID AssetManager::GetModelID (const String & name) const {
	HandleID * pos = mModelNames.get_ptr (name);
	return pos ? *pos : HandleID::invalid ();
}

Shader * AssetManager::GetShader (const HandleID & resource) const {
	return shaderIDs.get (resource);
}

Texture * AssetManager::GetTexture (const HandleID & resource) const {
	return textureIDs.get (resource);
}

Material * AssetManager::GetMaterial (const HandleID & resource) const {
	return materialIDs.get (resource);
}

Mesh * AssetManager::GetMesh (const HandleID & resource) const {
	return meshIDs.get (resource);
}

Model * AssetManager::GetModel (const HandleID & resource) const {
	return mModelIDs.get (resource);
}

std::vector<Vertex> AssetManager::GetVertices (HandleID verticesID) {
	return (mVerticesIDs[verticesID]);
}

std::vector<uint32_t> AssetManager::GetIndices (HandleID indicesID) {
	return (mIndicesIDs[indicesID]);
}


uint32_t AssetManager::GetVerticesSize (HandleID verticesID) {
	return ((uint32_t)mVerticesIDs.GetSize (verticesID));
}

uint32_t AssetManager::GetIndicesSize  (HandleID indicesID) {
	return ((uint32_t)mIndicesIDs.GetSize  (indicesID));
}

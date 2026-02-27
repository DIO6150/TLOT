#include <filesystem>
#include <vector>

#include <engine/core/SceneGraph.hpp>

#include <engine/utils/ShortcutTypes.hpp>
#include <engine/utils/types.hpp>

#include <engine/data/Instance.hpp>

using namespace Engine::Core;
using namespace Engine::Data;


void SceneNode::addMesh (GeometryID geometry, MaterialID material, glm::mat4 transform) {
	meshes.emplace_back (std::make_unique<Mesh> (geometry, material, transform));
	auto pos = graph->geometry_to_meshes.find (geometry);
	if (pos != graph->geometry_to_meshes.end ()) {
		pos->second.push_back (meshes.back ().get ());
	}
	else {
		graph->geometry_to_meshes.emplace (geometry, Vector<Mesh *> {meshes.back ().get ()});
	}

	++graph->mesh_count;
}

SceneNode::~SceneNode () {
	for (auto & child : children) {
		delete (child);
	}
}

const Vector<UPtr<Mesh>> & SceneNode::getMeshes () {
	return (meshes);
}

const Vector<SceneNode *> & SceneNode::getChildren () {
	return (children);
}

SceneNode::SceneNode (String name, SceneNode * parent, SceneGraph * graph) :
	name		{name},
	parent		{parent},
	graph		{graph}
	{
	if (parent != nullptr) parent->children.push_back (this);
}


SceneGraph::SceneGraph () : 
	root {"root", nullptr, this},
	mesh_count {0}
	{

}

SceneGraph::~SceneGraph () {
		
}

SceneNode * SceneGraph::GetRoot () {
	return (&root);
}

uint32_t SceneGraph::GetTotalMeshCount () const {
	return (mesh_count);
}

uint32_t SceneGraph::GetInstanceCount (GeometryID geometry) const {
	return (geometry_to_meshes.at (geometry).size ());
}

uint32_t SceneGraph::GetGeometryCount () const {
	return (geometry_to_meshes.size ());
}

const ObjectMap<Vector<Mesh *>> & SceneGraph::GetMeshes () const {
	return (geometry_to_meshes);
}
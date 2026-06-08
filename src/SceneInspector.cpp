
#include <Debugger/SceneInspector.hpp>

#include <imgui.h>
#include <AssetManager.hpp>

#include <glad/glad.h>

using namespace TLOT;

template<class T, class ... Args>
void InputText(std::string key, T value, Args ... args)
{
	ImGui::InputText (key.c_str(), (char*)std::to_string(value).c_str(), args...);
}

bool UniformField(UniformLayout layout, void * buffer)
{
	float* pData = static_cast<float*>(buffer);
    bool changed = false;
    ImGui::PushID(layout.name.c_str());

    switch (layout.type)
    {
		case UniformType::Vec4:  changed = ImGui::InputFloat4((layout.name+ "##v").c_str(), pData); break;
		case UniformType::Vec3:  changed = ImGui::InputFloat3((layout.name+ "##v").c_str(), pData); break;
		case UniformType::Vec2:  changed = ImGui::InputFloat2((layout.name+ "##v").c_str(), pData); break;
		case UniformType::Float: changed = ImGui::DragFloat  ((layout.name+ "##v").c_str(), pData, 0.01f); break;

		case UniformType::Mat4:
			changed |= ImGui::InputFloat4((layout.name+ " [0]").c_str(), pData + (0 * 4));
			changed |= ImGui::InputFloat4("[1]", pData + (1 * 4));
			changed |= ImGui::InputFloat4("[2]", pData + (2 * 4));
			changed |= ImGui::InputFloat4("[3]", pData + (3 * 4));
			break;

		case UniformType::TexQ:
			changed |= ImGui::InputFloat2((layout.name+ " pos"  ).c_str(), pData + 0);
			changed |= ImGui::InputFloat2("scale", pData + 2);
			changed |= ImGui::InputFloat ("depth", pData + 4);
			break;

		default: break;
    }

    ImGui::PopID();
    return changed;
}

void SceneInspector::DrawSceneObjectWidget(IObject * renderable)
{
	SceneObject object = renderable->GetSceneObject();

	ImGui::Spacing ();
	ImGui::Spacing ();

	if (ImGui::TreeNode("Renderer Flags"))
	{
		InputText("geometryID",    object.geometryID,    ImGuiInputTextFlags_ReadOnly);
		InputText("materialID",    object.materialID,    ImGuiInputTextFlags_ReadOnly);
		InputText("indexCount",    object.indexCount,    ImGuiInputTextFlags_ReadOnly);
		InputText("firstIndex",    object.firstIndex,    ImGuiInputTextFlags_ReadOnly);
		InputText("baseVertex",    object.baseVertex,    ImGuiInputTextFlags_ReadOnly);
		InputText("matrixIndex",   object.matrixIndex,   ImGuiInputTextFlags_ReadOnly);
		InputText("materialIndex", object.materialIndex, ImGuiInputTextFlags_ReadOnly);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Transform"))
	{
		bool shouldUpdate = false;
		Transform & transform = renderable->GetTransform();
		glm::vec3 eulerRotation = glm::degrees(glm::eulerAngles(transform.rotation));

		bool orthographic = true; // TODO : fix that
		ImGui::Checkbox("orthographic", &orthographic);

		float positionDragSpeed = 0.01f;
		float scaleDragSpeed    = 0.01f;

		if (orthographic)
		{
			positionDragSpeed = 1.0f;
			scaleDragSpeed    = 1.0f;
		}

		shouldUpdate |= ImGui::DragFloat3  ("position", &transform.position[0], positionDragSpeed);
		shouldUpdate |= ImGui::SliderFloat3("rotation", &eulerRotation[0], -180.0, 180.0);
		shouldUpdate |= ImGui::DragFloat3  ("scale",    &transform.scale[0], scaleDragSpeed);

		if (shouldUpdate)
		{
			transform.rotation = glm::quat(glm::radians(eulerRotation));
			renderable->UpdateTransform();
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Material"))
	{
		MaterialInstance & material = renderable->GetMaterial();
		auto materialTemplate = AssetManager::GetMaterialTemplate(material.GetTemplateHandle());
		uint8_t * basePtr = (uint8_t*) material.GetData().data();

		for(auto const & param : materialTemplate->Layout())
		{
			ImGui::PushID(param.name.c_str());
			//ImGui::Text(param.name.c_str());
			void * fieldPtr = basePtr + param.offset;
			if (UniformField(param, fieldPtr))
			{
				renderable->UpdateMaterial();
			}
			ImGui::PopID();
		}

		ImGui::TreePop();
	}

	ImGui::Spacing ();


}

void SceneInspector::RegisterRenderableObject(IObject * renderable)
{
	m_objectInfos.emplace_back(renderable);
}

void SceneInspector::OpenMenu ()
{
	// Verify ABI compatibility between caller code and compiled version of Dear ImGui. This helps detects some build issues.
	IMGUI_CHECKVERSION();
	
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

	ImGuiWindowFlags flags = ImGuiWindowFlags_None;

	// Main body of the Demo window starts here.
	if (!ImGui::Begin("Scene Editor", NULL, flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	AssetManager & assetManager = AssetManager::GetInstance ();
	if (ImGui::CollapsingHeader ("Asset Inspector"))
	{
		ImGui::SeparatorText ("Textures Inspector");
		for (auto & resource : assetManager.m_textures)
		{
			auto handle = resource.GetHandle ();
			auto key = assetManager.m_keyCacheReversed[handle];
			auto path = resource.GetPath ();
			auto & texture = resource.Get ();

			ImGui::Spacing ();
			ImGui::PushID ("texture_key_" + handle);
			ImGui::InputText ("Key", (char *)key.c_str (), key.capacity () + 1, ImGuiInputTextFlags_ReadOnly);
			ImGui::PopID ();

			ImGui::PushID ("texture_path_" + handle);
			ImGui::InputText ("Path", (char *)path.c_str (), path.capacity () + 1, ImGuiInputTextFlags_ReadOnly);
			ImGui::PopID ();
			
			ImGui::PushID ("texture_handle_" + handle);
			ImGui::InputInt ("ResourceHandle", (int *)(&handle), 0, 0, ImGuiInputTextFlags_ReadOnly);
			ImGui::PopID ();
			
			std::string name = "texture_button" + std::to_string (handle);
			ImGui::PushID ((char *)name.c_str ());;
			if (ImGui::Button ("Click to see"))
			{
				ImGui::OpenPopup (resource.GetPath ().c_str ());
			}
			
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			
			if (ImGui::BeginPopupModal (resource.GetPath ().c_str (), NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				float height = 512;
				float width = (height * texture.width) / texture.height;
				ImGui::Image ((ImTextureID)(intptr_t)GetOpenGLTexture (handle, texture), ImVec2 (width, height));
				if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup ();
			}
			ImGui::PopID ();
		}


		ImGui::SeparatorText ("ShaderSource Inspector");
		for (auto & resource : assetManager.m_shaderSources)
		{
			auto handle = resource.GetHandle ();
			auto key = assetManager.m_keyCacheReversed[handle];
			auto path = resource.GetPath ();
			auto & shaderSource = resource.Get ();

			ImGui::Spacing ();
			ImGui::PushID ("shadersource_key" + handle);
			ImGui::InputText ("Key", (char *)key.c_str (), key.capacity () + 1, ImGuiInputTextFlags_ReadOnly);
			ImGui::PopID ();

			ImGui::PushID ("shaderSource_path_" + handle);
			ImGui::InputText ("Path", (char *)path.c_str (), path.capacity () + 1, ImGuiInputTextFlags_ReadOnly);
			ImGui::PopID ();
			
			ImGui::PushID ("shaderSource_handle_" + handle);
			ImGui::InputInt ("ResourceHandle", (int *)(&handle), 0, 0, ImGuiInputTextFlags_ReadOnly);
			ImGui::PopID ();
			
			std::string name = "shaderSource_button_" + std::to_string (handle);
			ImGui::PushID ((char *)name.c_str ());
			if (ImGui::Button ("Click to see"))
			{
				ImGui::OpenPopup (resource.GetPath ().c_str ());
			}
			
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			
			if (ImGui::BeginPopupModal (resource.GetPath ().c_str (), NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::InputTextMultiline ("Source", (char*)shaderSource.source.c_str (), shaderSource.source.capacity () + 1, ImVec2(0, 0),ImGuiInputTextFlags_ReadOnly);
				if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup ();
			}
			ImGui::PopID ();
		}
	}

	if (ImGui::CollapsingHeader ("Renderer Inspector"))
	{
		ImGui::SeparatorText ("SceneObject Inspector");
		
		int id = 0;
		for (auto sceneObject : m_objectInfos)
		{
			
			ImGui::PushID ("scene_object_" + id++);
			DrawSceneObjectWidget(sceneObject);
			ImGui::PopID();
		}
	}

	ImGui::End ();
}

uint32_t SceneInspector::GetOpenGLTexture (ResourceHandle handle, Texture const & texture)
{
	if (m_textures.find (handle) != m_textures.end ()) return m_textures[handle];

	uint32_t obj;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures (1, &obj);
	
	glBindTexture (GL_TEXTURE_2D, obj);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.data);
	glGenerateMipmap (GL_TEXTURE_2D);

	m_textures[handle] = obj;

	return obj;
}
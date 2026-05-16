
#include <Debugger/SceneEditor.hpp>

#include <imgui.h>
#include <AssetManager.hpp>

#include <glad/glad.h>

using namespace TLOT;

void SceneEditor::RegisterRenderer (IRenderer & renderer)
{
	m_renderers.emplace (&renderer);
}

void SceneEditor::OpenMenu ()
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
				ImGui::Image ((ImTextureID)(intptr_t)GetOpenGLTexture (handle, texture), ImVec2 (512, 512));
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

	if (ImGui::CollapsingHeader ("Rendererers"))
	{
		size_t index = 0;
		for (auto & renderer : m_renderers)
		{
			std::string title = "(" + std::to_string (index) + ")";
			if (ImGui::CollapsingHeader (title.c_str ()))
			{
				
			}
		}
	}

	ImGui::End ();
}

uint32_t SceneEditor::GetOpenGLTexture (ResourceHandle handle, Texture const & texture)
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
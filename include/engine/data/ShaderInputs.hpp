#pragma once

#include <engine/utils/ShortcutTypes.hpp>
#include <engine/utils/Types.hpp>

#include <glm/glm.hpp>

#include <functional>

namespace Engine::Internal {
	template<typename T>
	struct RenderInput {
		RenderInput (std::function<const T & (void)> valueGetter) :
			mValueGetter {valueGetter}
			{

		}

		const T & GetValue () const {
			return (mValueGetter ());
		}

	private:
		std::function<const T & (void)> mValueGetter;
	};
}

namespace Engine::Data {
	class ShaderInputs {
	public:
		template<class T> void AddInput (std::string name, std::function<const T & (void)> valueGetter);
		template<class T> void AddInputFromPrevious (std::string name, std::function<const T & (void)> valueGetter); // TODO: implement

		template<class T>
		void RemoveInput (std::string name); // TODO: implement

		void Upload (const Data::Shader & shader);
	private:
		
		Map<Internal::RenderInput<float>>		mInputsFloat;
		Map<Internal::RenderInput<int>>			mInputsInt;
		Map<Internal::RenderInput<Core::HandleID>>	mInputsTextures;
		Map<Internal::RenderInput<glm::mat4>>		mInputsMat4;
		Map<Internal::RenderInput<glm::vec3>>		mInputsVec3;
	};
}
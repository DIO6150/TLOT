
#include <engine/data/Shader.hpp>
#include <engine/data/ShaderInputs.hpp>


using namespace Engine::Data;
using namespace Engine::Internal;
using namespace Engine::Core;

template<>
void ShaderInputs::AddInput<float> (std::string name, std::function<const float & (void)> valueGetter) {
	mInputsFloat.emplace (name, valueGetter);
}

template<>
void ShaderInputs::AddInput<int> (std::string name, std::function<const int & (void)> valueGetter) {
	mInputsInt.emplace (name, valueGetter);
}

template<>
void ShaderInputs::AddInput<glm::mat4> (std::string name, std::function<const glm::mat4 & (void)> valueGetter) {
	mInputsMat4.emplace (name, valueGetter);
}

template<>
void ShaderInputs::AddInput<glm::vec3> (std::string name, std::function<const glm::vec3 & (void)> valueGetter) {
	mInputsVec3.emplace (name, valueGetter);
}

template<>
void ShaderInputs::AddInput<HandleID> (std::string name, std::function<const HandleID & (void)> valueGetter) {
	mInputsTextures.emplace (name, valueGetter);
}

template<class T>
void ShaderInputs::AddInput (std::string name, std::function<const T & (void)> valueGetter) {
	static_assert (false, "Not allowed. Types are {float, int, mat4x4, HandleID}");
}

void ShaderInputs::Upload (const Shader & shader) {
	for (const auto & [name, input] : mInputsInt) {
		shader.upload1i (name, input.GetValue ());
	}

	for (const auto & [name, input] : mInputsVec3) {
		shader.upload3fv (name, input.GetValue ());
	}

	for (const auto & [name, input]: mInputsFloat) {
		shader.upload1f (name, input.GetValue ());
	}

	for (const auto & [name, input]: mInputsMat4) {
		shader.uploadMatrix4 (name, input.GetValue ());
	}

	for (const auto & [name, input] : mInputsTextures) {
		// TODO-fix: implement
	}
}
#include <engine/core/PostProcessing.hpp>

using namespace Engine::Internal;
using namespace Engine::Core;
using namespace Engine::Data;

PostProcessingEffect::PostProcessingEffect (HandleID shader, size_t output):
	postProcess		{shader},
	active			{true},
	colorAttachmentCount	{output}
	{
	
}

PostProcessingEffect::PostProcessingEffect (HandleID shader, size_t output, ShaderInputs && params):
	postProcess		{shader},
	active			{true},
	colorAttachmentCount	{output},
	params			{params}
	{
	
}
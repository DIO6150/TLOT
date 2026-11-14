#pragma once

#include <TetoEngine/Private/GPUResources/GPUResourceTable.hpp>
#include <TetoEngine/Private/GPUResources/OpenGLBuffer.hpp>
#include <TetoEngine/Private/GPUResources/OpenGLShaderStorageBuffer.hpp>
#include <TetoEngine/Private/GPUResources/ShaderProgram.hpp>
#include <TetoEngine/Private/GPUResources/DrawCommand.hpp>


namespace TetoEngine
{
	class Application;

	class GPUResourceManager
	{
	public:
		GPUResourceManager(Application * app);

		void reserveGeometry(ID64 handle);
		void uploadGeometry(ID64 handle);
		void unloadGeometry(ID64 handle);
		std::pair<uint32_t, uint32_t> geometryByteOffset(ID64 handle); // byte offset in vbo, byte offset in ebo
		std::pair<uint32_t, uint32_t> geometryBuffer(ID64 handle); // returns the vbo handle, ebo handle
		bool containsGeometry(ID64 handle);


		void uploadIndirectCommand(uint32_t pass, DrawCommand command);
		void clearPass(uint32_t pass);


		void reserveInstance(ID64 handle);
		template<class property_t> void uploadInstanceProperty(uint32_t pass, uint32_t propertyID, ID64 handle, property_t property)
		{

		}
		void unloadInstanceProperty(uint32_t pass, uint32_t propertyID, ID64 handle);
		bool hasProperty(ID64 instance);


		ID64 generateShaderProgram(ID64 vertex, ID64 fragment);
		ID64 getProgramHash(ID64 vertex, ID64 fragment);
		void updateShaderProgram(ID64 program);

	private:
		std::optional<size_t> findVertexDrawerIndex(ID64 handle);

	private:
		struct BufferStash
		{
			std::map<ID64, size_t> location;
			std::vector<OpenGLBuffer> buffer;
			std::vector<GPUResourceTable> table;
		};

		struct ShaderStorageStash
		{
			std::map<ID64, size_t> location;
			std::vector<ShaderStorageBuffer> buffer;
			std::vector<GPUResourceTable> table;
		};

		struct StashShaderStash
		{
			std::map<ID64, size_t> location;
			std::vector<ShaderStorageStash> buffer;
		};

		struct Program
		{
			ShaderProgram program;

			ID64 vertexHandle;
			ID64 fragmentHandle;
		};

		BufferStash vertices_;
		BufferStash indices_;

		BufferStash commands_; // we use the command index instead of the handle ?

		StashShaderStash instanceProperties_;

		std::map<ID64, Program> programList_;

		size_t defaultVBOSize_ = 10000000; // 10 Mo
		size_t defaultEBOSize_ = 10000000; // 10 Mo

	private:
		Application * app_;
	};
}
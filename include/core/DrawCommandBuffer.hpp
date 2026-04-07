#pragma once

#include <cassert>

#include <opengl/DrawCommand.hpp>

#include <resources/Resource.hpp>

namespace TLOT
{
	class DrawCommandBuffer
	{
	private:
		struct ProtoDrawCommand
		{
			uint32_t index_count;
			uint32_t instance_count;
			uint32_t base_index;
			int32_t  base_vertex;
			uint32_t base_instance;
		};

	public:
		void RegisterCommand (GeometryID geometryID, size_t index_count, size_t vertex_offset, size_t index_offset)
		{
			m_commands.emplace
			(
				std::piecewise_construct,
				std::forward_as_tuple (geometryID),
				std::forward_as_tuple
				(
					index_count,
					0,
					index_offset,
					vertex_offset,
					0
				)
			);
		}

		void AddInstance (GeometryID geometryID)
		{
			assert (m_commands.find (geometryID) != m_commands.end ());

			++m_commands[geometryID].instance_count;

			m_dirty = true;
		}

		void RemoveInstance (GeometryID geometryID)
		{
			assert (m_commands.find (geometryID) != m_commands.end ());

			--m_commands[geometryID].instance_count;

			m_dirty = true;
		}

		// TODO: optimize pulling logic
		std::vector<DrawCommand> & Pull ()
		{
			if (m_dirty)
			{	
				m_dirty = false;
				m_cached.clear ();
				
				uint32_t baseInstance = 0;
				for (auto [geometryID, command] : m_commands)
				{
					DrawCommand finalCommand = command;
					finalCommand.base_instance = baseInstance;
					
					m_cached.push_back (finalCommand);
					
					baseInstance = finalCommand.instance_count;
				}	
			}

			return m_cached;
		}

		size_t GetCommandCount ()
		{
			return m_commands.size ();
		}

	private:
		std::map<GeometryID, DrawCommand> m_commands;

		std::vector<DrawCommand> m_cached;
		bool m_dirty = true;
	};
}
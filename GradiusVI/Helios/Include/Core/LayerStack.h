#pragma once
#include "Layer.h"
#include <vector>
#include <memory>

namespace Helios
{
	class LayerStack
	{
	public:
		LayerStack();
		~LayerStack() = default;
		
		Layer* PushLayer(std::unique_ptr<Layer> a_layer);
		Layer* PushOverlay(std::unique_ptr<Layer> a_overlay);
		void PopLayer(Layer& a_layer);
		void PopOverlay(Layer& a_overlay);

		std::vector<std::unique_ptr<Layer>>::iterator begin() { return m_layers.begin(); }
		std::vector<std::unique_ptr<Layer>>::iterator end() { return m_layers.end(); }
	private:
		std::vector<std::unique_ptr<Layer>> m_layers;
		std::size_t m_layerInsertIndex;
	};
}
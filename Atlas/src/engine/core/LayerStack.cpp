#include "atlaspch.h"
#include "LayerStack.h"

namespace Atlas
{

	LayerStack::LayerStack()
	{
	
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_layers)
		{
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_layers.emplace(m_layers.begin() + m_insertIndex, layer);
		m_insertIndex++;
	}

	void LayerStack::PushOverlay(Layer* overlay) 
	{
		m_layers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto i = std::find(m_layers.begin(), m_layers.end(), layer);
		if (i != m_layers.end())
		{
			m_layers.erase(i);
			m_insertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto i = std::find(m_layers.begin(), m_layers.end(), overlay);
		if (i != m_layers.end())
		{
			m_layers.erase(i);
		}
	}
}
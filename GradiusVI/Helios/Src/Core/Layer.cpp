#include "hepch.h"
#include "Core/Layer.h"

namespace Helios
{
	Layer::Layer(std::string a_debugName)
		: m_debugName(std::move(a_debugName)), m_isEnabled(true)
	{
		
	}
}
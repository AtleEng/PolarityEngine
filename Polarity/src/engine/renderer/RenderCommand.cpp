#include "polpch.h"
#include "RenderCommand.h"

#include "platform/openGL/OpenGLRendererAPI.h"

namespace Polarity {

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}
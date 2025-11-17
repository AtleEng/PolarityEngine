#include "polpch.h"
#include "RenderCommand.h"

#include "platform/openGL/OpenGLRendererAPI.h"

namespace Polarity {

	Scope<RendererAPI> RenderCommand::s_RendererAPI = CreateScope<OpenGLRendererAPI>();
}
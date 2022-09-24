#include "flamepch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Flame {

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;

}
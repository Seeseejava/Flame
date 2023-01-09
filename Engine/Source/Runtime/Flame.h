#pragma once

//For use by Runtime applications


#include "Runtime/Core/Application.h"
#include "Runtime/Core/Input.h"
#include "Runtime/Core/Log.h"

#include "Runtime/Core/Timestep.h"

#include "Runtime/Core/KeyCodes.h"
#include "Runtime/Core/MouseButtonCodes.h"
#include "Runtime/Core/Layer.h"

#include "Runtime/ImGui/ImGuiLayer.h"

#include "Runtime/ECS/Scene/Scene.h"
#include "Runtime/ECS/Component/ComponentGroup.h"
#include "Runtime/ECS/Entity/Entity.h"

// ---Renderer------------------------
#include "Runtime/Renderer/Renderer.h"
#include "Runtime/Renderer/Renderer2D.h"
#include "Runtime/Renderer/Renderer3D.h"
#include "Runtime/Renderer/RenderCommand.h"
		
#include "Runtime/Renderer/VertexBuffer.h"
#include "Runtime/Renderer/IndexBuffer.h"
#include "Runtime/Renderer/FrameBuffer.h"
#include "Runtime/Renderer/Shader.h"
#include "Runtime/Renderer/Texture.h"
#include "Runtime/Renderer/SubTexture2D.h"

#include "Runtime/Renderer/VertexArray.h"


// -----------------------------------


//---Manager-------------
#include "Runtime/Resource/ConfigManager/ConfigManager.h"
#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Runtime/Resource/ModeManager/ModeManager.h"
//-----------------------------

//----Entry Point--------------
//-----------------------------
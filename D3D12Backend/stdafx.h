#pragma once

#include "BoolkaCommon/stdafx.h"

#include "ProjectConfig.h"
#include "ProjectHelpers.h"

// DirectX
#include <d3d12.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <pix3.h>

// Own code

#include "APIWrappers/CommandList/GraphicCommandListImpl.h"
#include "APIWrappers/Resources/Buffers/Buffer.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "APIWrappers/Resources/Textures/Views/DepthStencilView.h"
#include "APIWrappers/Resources/Textures/Views/RenderTargetView.h"
#include "Contexts/RenderContext.h"
#include "Contexts/RenderEngineContext.h"
#include "Contexts/RenderFrameContext.h"
#include "Contexts/RenderThreadContext.h"
#include "DebugHelpers/DebugRenderScope.h"
#include "RenderPass.h"
#include "RenderSchedule/ResourceContainer.h"

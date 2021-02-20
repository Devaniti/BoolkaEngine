#pragma once

#include "BoolkaCommon/stdafx.h"

#include "ProjectConfig.h"

// DirectX
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>
#include <pix3.h>

#include "ProjectHelpers.h"

// Own code

#include "DebugHelpers/DebugRenderScope.h"

#include "RenderPass.h"
#include "Contexts/RenderContext.h"
#include "Contexts/RenderEngineContext.h"
#include "Contexts/RenderFrameContext.h"
#include "Contexts/RenderThreadContext.h"
#include "RenderSchedule/ResourceContainer.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "APIWrappers/Resources/Textures/Views/RenderTargetView.h"
#include "APIWrappers/Resources/Textures/Views/DepthStencilView.h"
#include "APIWrappers/Resources/Buffers/Buffer.h"
#include "APIWrappers/CommandList/GraphicCommandListImpl.h"

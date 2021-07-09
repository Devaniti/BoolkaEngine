#pragma once

#ifdef BLK_RENDER_DEBUG
#define BLK_RENDER_DEBUG_ONLY(...) (__VA_ARGS__)
#define BLK_PIX_SCOPE_COLOR PIX_COLOR(0, 127, 127)
#define BLK_PIX_MARKER_COLOR PIX_COLOR(127, 127, 0)
#define BLK_GPU_SCOPE(commandList, ...) \
    DebugRenderScope BLK_UNIQUE_NAME(renderScope)(commandList, __VA_ARGS__)
#define BLK_GPU_MARKER(commandList, ...) \
    PIXSetMarker(commandList, BLK_PIX_MARKER_COLOR, __VA_ARGS__)
#define BLK_RENDER_PASS_START_DEBUG_INTERNAL(commandList, resourceTracker, passName) \
    BLK_GPU_SCOPE(commandList, passName);                                            \
    resourceTracker.ValidateStates(commandList)
#else //! BLK_RENDER_DEBUG
#define BLK_RENDER_DEBUG_ONLY(...) ((void)0)
#define BLK_GPU_SCOPE(...) ((void)0)
#define BLK_GPU_MARKER(...) ((void)0)
#define BLK_RENDER_PASS_START_DEBUG_INTERNAL(...) ((void)0)
#endif

#ifdef BLK_ENABLE_STATS
#define BLK_RENDER_PASS_START_INTERNAL(passName)                                         \
    BLK_RENDER_PASS_START_DEBUG_INTERNAL(                                                \
        renderContext.GetRenderThreadContext().GetGraphicCommandList(), resourceTracker, \
        #passName);                                                                      \
    renderContext.GetRenderEngineContext().GetTimestampContainer().Mark(                 \
        renderContext.GetRenderThreadContext().GetGraphicCommandList(),                  \
        TimestampContainer::Markers::passName)
#else
#define BLK_RENDER_PASS_START_INTERNAL(passName)                                         \
    BLK_RENDER_PASS_START_DEBUG_INTERNAL(                                                \
        renderContext.GetRenderThreadContext().GetGraphicCommandList(), resourceTracker, \
        #passName)
#endif

#define BLK_RENDER_PASS_START(passName)                                                        \
    static_assert(std::is_same<std::remove_reference<decltype(*this)>::type, passName>::value, \
                  "Incorrect passName was passed to BLK_RENDER_PASS_START");                   \
    BLK_RENDER_PASS_START_INTERNAL(passName)

#define BLK_TEXCUBE_FACE_COUNT 6

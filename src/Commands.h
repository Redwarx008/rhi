#pragma once

#include "CommandAllocator.h"
#include "common/Constants.h"
#include "common/Ref.hpp"
#include "Subresource.h"
#include "BufferBase.h"
#include "BindSetBase.h"
#include <array>
#include <string_view>

namespace rhi::impl
{
	enum class Command
	{
        ClearBuffer,
        BeginRenderPass,
        BeginComputePass,
        BeginDebugLabel,
        CopyBufferToBuffer,
        CopyBufferToTexture,
        CopyTextureToBuffer,
        CopyTextureToTexture,
        Dispatch,
        DispatchIndirect,
        Draw,
        DrawIndexed,
        DrawIndirect,
        DrawIndexedIndirect,
        MultiDrawIndirect,
        MultiDrawIndexedIndirect,
        SetRenderPipeline,
        SetComputePipeline,
        SetViewport,
        SetScissorRects,
        SetIndexBuffer,
        SetVertexBuffer,
        SetPushConstant,
        SetStencilReference,
        SetBlendConstant,
        SetBindSet,
        EndRenderPass,
        EndComputePass,
        EndDebugLabel,
        MapBufferAsync
	};


    struct RenderPassColorAttachment
    {
        RenderPassColorAttachment();
        ~RenderPassColorAttachment();
        Ref<TextureViewBase> view;
        Ref<TextureViewBase> resolveView;
        LoadOp loadOp;
        StoreOp storeOp;
        Color clearColor;
    };


    struct RenderPassDepthStencilAttachment
    {
        RenderPassDepthStencilAttachment();
        ~RenderPassDepthStencilAttachment();
        Ref<TextureViewBase> view;
        LoadOp depthLoadOp;
        StoreOp depthStoreOp;
        LoadOp stencilLoadOp;
        StoreOp stencilStoreOp;
        float depthClearValue;
        uint32_t stencilClearValue;
    };


    struct BeginRenderPassCmd
    {
        BeginRenderPassCmd();
        ~BeginRenderPassCmd();
        std::array<RenderPassColorAttachment, cMaxColorAttachments> colorAttachments;
        RenderPassDepthStencilAttachment depthStencilAttachment;
        uint8_t colorAttachmentCount;
    };


    struct BeginComputePassCmd
    {
        BeginComputePassCmd();
        ~BeginComputePassCmd();
    };

    struct ClearBufferCmd 
    {
        ClearBufferCmd();
        ~ClearBufferCmd();

        Ref<BufferBase> buffer;
        uint32_t value;
        uint64_t offset;
        uint64_t size;
    };

    struct CopyBufferToBufferCmd
    {
        CopyBufferToBufferCmd();
        ~CopyBufferToBufferCmd();

        Ref<BufferBase> srcBuffer;
        uint64_t srcOffset;
        Ref<BufferBase> dstBuffer;
        uint64_t dstOffset;
        uint64_t size;
    };

    struct CopyBufferToTextureCmd
    {
        CopyBufferToTextureCmd();
        ~CopyBufferToTextureCmd();

        Ref<BufferBase> srcBuffer;
        TextureDataLayout dataLayout;
        Ref<TextureBase> dstTexture;
        Origin3D origin;
        Extent3D size;
        uint32_t mipLevel = 0;
        Aspect aspect;
    };

    struct CopyTextureToBufferCmd
    {
        CopyTextureToBufferCmd();
        ~CopyTextureToBufferCmd();

        Ref<TextureBase> srcTexture;
        TextureDataLayout dataLayout;
        Origin3D origin;
        Extent3D size;
        uint32_t mipLevel = 0;
        Aspect aspect;
        Ref<BufferBase> dstBuffer;
    };

    struct CopyTextureToTextureCmd
    {
        CopyTextureToTextureCmd();
        ~CopyTextureToTextureCmd();

        Ref<TextureBase> srcTexture;
        uint32_t srcMipLevel;
        Origin3D srcOrigin;
        Extent3D srcSize;
        Aspect srcAspect;

        Ref<TextureBase> dstTexture;
        uint32_t dstMipLevel;
        Origin3D dstOrigin;
        Extent3D dstSize;
        Aspect dstAspect;
    };

    struct MapBufferAsyncCmd
    {
        MapBufferAsyncCmd();
        ~MapBufferAsyncCmd();

        Ref<BufferBase> buffer;
        MapMode mapMode;
        BufferMapCallback callback;
        void* userData;
    };

    struct BeginDebugLabelCmd
    {
        BeginDebugLabelCmd();
        ~BeginDebugLabelCmd();
        
        uint32_t labelLength;
        Color color;
    };

    struct EndDebugLabelCmd
    {
        EndDebugLabelCmd();
        ~EndDebugLabelCmd();
    };

    const char* EnsureValidString(CommandAllocator& allocator, std::string_view s, uint32_t* length);

    struct SetRenderPipelineCmd
    {
        SetRenderPipelineCmd();
        ~SetRenderPipelineCmd();

        Ref<RenderPipelineBase> pipeline;
    };

    struct VertexBuffer
    {
        VertexBuffer();
        ~VertexBuffer();
        Ref<BufferBase> buffer;
        uint64_t offset;
    };

    struct SetVertexBufferCmd
    {
        SetVertexBufferCmd();
        ~SetVertexBufferCmd();

        std::array<VertexBuffer, cMaxVertexBuffers> buffers;
        uint32_t bufferCount;
        uint32_t firstSlot;
    };

    struct SetIndexBufferCmd
    {
        SetIndexBufferCmd();
        ~SetIndexBufferCmd();

        Ref<BufferBase> buffer;
        IndexFormat format;
        uint64_t offset;
        uint64_t size;
    };

    struct SetScissorRectsCmd
    {
        SetScissorRectsCmd();
        ~SetScissorRectsCmd();

        std::array<Rect, cMaxColorAttachments> scissors;
        uint32_t scissorCount;
        uint32_t firstScissor;
    };

    struct SetStencilReferenceCmd
    {
        uint32_t reference;
    };

    struct SetBlendConstantCmd
    {
        Color color;
    };

    struct SetBindSetCmd
    {
        SetBindSetCmd();
        ~SetBindSetCmd();

        Ref<BindSetBase> set;
        uint32_t setIndex;
        uint32_t dynamicOffsetCount;
    };

    struct SetPushConstantCmd
    {
        SetPushConstantCmd();
        ~SetPushConstantCmd();
        uint32_t size;
    };


    struct DrawCmd 
    {
        uint32_t vertexCount;
        uint32_t instanceCount;
        uint32_t firstVertex;
        uint32_t firstInstance;
    };

    struct DrawIndexedCmd 
    {
        uint32_t indexCount;
        uint32_t instanceCount;
        uint32_t firstIndex;
        int32_t baseVertex;
        uint32_t firstInstance;
    };

    struct DrawIndirectCmd 
    {
        DrawIndirectCmd();
        ~DrawIndirectCmd();

        Ref<BufferBase> indirectBuffer;
        uint64_t indirectOffset;
    };

    struct DrawIndexedIndirectCmd : DrawIndirectCmd
    {
    };

    struct MultiDrawIndirectCmd
    {
        MultiDrawIndirectCmd();
        ~MultiDrawIndirectCmd();

        Ref<BufferBase> indirectBuffer;
        uint64_t indirectOffset;
        uint32_t maxDrawCount;
        Ref<BufferBase> drawCountBuffer;
        uint64_t drawCountOffset;
    };

    struct MultiDrawIndexedIndirectCmd : MultiDrawIndirectCmd
    {
    };

    struct EndRenderPassCmd
    {
        EndRenderPassCmd();
        ~EndRenderPassCmd();
    };

    struct SetViewportCmd
    {
        SetViewportCmd();
        ~SetViewportCmd();

        std::array<Viewport, cMaxColorAttachments> viewports;
        uint32_t viewportCount;
        uint32_t firstViewport;
    };

    struct SetComputePipelineCmd
    {
        SetComputePipelineCmd();
        ~SetComputePipelineCmd();

        Ref<ComputePipelineBase> pipeline;
    };

    struct DispatchCmd
    {
        uint32_t x;
        uint32_t y;
        uint32_t z;
    };

    struct DispatchIndirectCmd
    {
        DispatchIndirectCmd();
        ~DispatchIndirectCmd();

        Ref<BufferBase> indirectBuffer;
        uint64_t indirectOffset;
    };

    struct EndComputePassCmd
    {
        EndComputePassCmd();
        ~EndComputePassCmd();
    };

    void FreeCommands(CommandIterator* commands);
}
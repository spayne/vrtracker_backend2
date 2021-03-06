#pragma once

#include "slab_allocator.h"
#include <openvr.h>

const size_t VRTMPSize = vr::k_unMaxPropertyStringSize;
//using VRFinalAllocatorType = slab_allocator<char>;

const size_t VR_LARGE_SEGMENT_SIZE = 8192;		// segment size for per/frame data.  e.g. 1minute at 90 fps - 5400

template <typename T>
using VRAllocatorTemplate = std::allocator<T>;

#include "result.h"

template <>
struct ValidReturnCode<vr::ETrackedPropertyError>
{
	static const vr::ETrackedPropertyError return_code = vr::TrackedProp_Success;
};

template <>
struct ValidReturnCode<vr::EVRApplicationError>
{
	static const vr::EVRApplicationError return_code = vr::VRApplicationError_None;
};

template <>
struct ValidReturnCode<vr::EVROverlayError>
{
	static const vr::EVROverlayError return_code = vr::VROverlayError_None;
};

template <>
struct ValidReturnCode<vr::EVRSettingsError>
{
	static const vr::EVRSettingsError return_code = vr::VRSettingsError_None;
};

template <>
struct ValidReturnCode<vr::EVRCompositorError>
{
	static const vr::EVRCompositorError return_code = vr::VRCompositorError_None;
};

template <>
struct ValidReturnCode<vr::EVRRenderModelError>
{
	static const vr::EVRRenderModelError return_code = vr::VRRenderModelError_None;
};

template <>
struct ValidReturnCode<vr::EVRTrackedCameraError>
{
	static const vr::EVRTrackedCameraError return_code = vr::VRTrackedCameraError_None;
};


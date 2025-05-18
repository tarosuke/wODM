/************************************************************ toolbox graphics
 * Copyright (C) 2021,2024 tarosuke<webmaster@tarosuke.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include <X11/X.h>
#include <openvr/openvr.h>

#include <tb/matrix.h>
#include <tb/prefs.h>

#include "gl/framebuffer.h"
#include "gl/gl.h"
#include "gl/glx.h"



struct OpenVR : GLX {
private:
	struct Exception {
		const char* message;
		vr::HmdError code;
	};

	OpenVR();
	~OpenVR();

	struct F : tb::Factory<Core> {
		uint Score() final {
			return vr::VR_IsRuntimeInstalled() ? Certitude::passiveMatch : 0;
		};
		Core* New() final { return new OpenVR; };
	};
	static F factory;

	vr::TrackedDevicePose_t devicePoses[vr::k_unMaxTrackedDeviceCount];
	vr::IVRSystem& openVR;

	// フレームバッファ他
	GL::Framebuffer::Size renderSize;
	Eyes eyes;
	static constexpr vr::EVREye eyeIndex[2] = {vr::Eye_Right, vr::Eye_Left};
	vr::Texture_t fbFeatures[2];

	// 初期化サポート
	static vr::IVRSystem& GetOpenVR(); // 失敗したら例外
	static GL::Framebuffer::Size GetRenderSize(vr::IVRSystem&);

	// 姿勢取得
	void UpdatePose() final;
	void Finish() final {
		for (unsigned n(0); n < 2; ++n) {
			vr::VRCompositor()->Submit(eyeIndex[n], &fbFeatures[n]);
		}
	};
};


OpenVR::OpenVR() :
	GLX(eyes),
	openVR(GetOpenVR()),
	renderSize(GetRenderSize(openVR)) {

	Setup(DefaultRootWindow(display));

	// eyes初期化
	for (unsigned n(0); n < 2; ++n) {
		const auto ei(eyeIndex[n]);
		vr::HmdMatrix44_t pm(openVR.GetProjectionMatrix(ei, nearClip, farClip));
		vr::HmdMatrix34_t eh(openVR.GetEyeToHeadTransform(ei));

		Eye eye(renderSize.width, renderSize.height);
		eye.projection.Transpose(pm.m);
		eye.eye2Head.TransposeAffine(eh.m);
		eye.eye2Head.InvertAffine();

		fbFeatures[n].handle =
			(void*)(uintptr_t)eye.framebuffer.GetColorBufferID();
		fbFeatures[n].eType = vr::TextureType_OpenGL;
		fbFeatures[n].eColorSpace = vr::ColorSpace_Gamma;

		eyes.emplace_back(std::move(eye));
	}
}
OpenVR::~OpenVR() { vr::VR_Shutdown(); }

void OpenVR::UpdatePose() {
	// 全デバイスの姿勢を取得
	vr::VRCompositor()->WaitGetPoses(
		devicePoses, vr::k_unMaxTrackedDeviceCount, NULL, 0);
	for (unsigned n(0); n < vr::k_unMaxTrackedDeviceCount; ++n) {
		if (devicePoses[n].bPoseIsValid) {
			switch (openVR.GetTrackedDeviceClass(n)) {
			case vr::TrackedDeviceClass_HMD:
				pose.TransposeAffine(
					devicePoses[n].mDeviceToAbsoluteTracking.m);
				pose.InvertAffine();
				break;
			default:
				break;
			}
		}
	}
};

vr::IVRSystem& OpenVR::GetOpenVR() {
	Exception exception = {"Failed to initialize OpenVR"};
	if (vr::IVRSystem* const o =
			vr::VR_Init(&exception.code, vr::VRApplication_Scene)) {
		return *o;
	}
	throw exception;
}

GL::Framebuffer::Size OpenVR::GetRenderSize(vr::IVRSystem& o) {
	GL::Framebuffer::Size size;
	o.GetRecommendedRenderTargetSize(&size.width, &size.height);
	return size;
}

OpenVR::F OpenVR::factory;

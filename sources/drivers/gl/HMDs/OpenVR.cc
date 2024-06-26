﻿/************************************************************ toolbox graphics
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

#include "core.h"
#include "gl/framebuffer.h"
#include "gl/gl.h"



namespace {
	::Display* GetDisplay() {
		::Display* display(XOpenDisplay(""));

		/** デフォルトのビジュアルフォーマット
		 */
		static int attr[] = {
			GLX_USE_GL,
			GLX_LEVEL,
			0,
			GLX_RGBA,
			GLX_DOUBLEBUFFER,
			GLX_RED_SIZE,
			8,
			GLX_GREEN_SIZE,
			8,
			GLX_BLUE_SIZE,
			8,
			GLX_ALPHA_SIZE,
			8,
			GLX_DEPTH_SIZE,
			24,
			GLX_STENCIL_SIZE,
			8,
			GLX_ACCUM_RED_SIZE,
			0,
			GLX_ACCUM_GREEN_SIZE,
			0,
			GLX_ACCUM_BLUE_SIZE,
			0,
			GLX_ACCUM_ALPHA_SIZE,
			0,
			None};
		XVisualInfo* visual(
			glXChooseVisual(display, DefaultScreen(display), attr));
		GLXContext context(glXCreateContext(display, visual, NULL, True));
		glXMakeCurrent(display, DefaultRootWindow(display), context);

		// glew初期化
		if (GLEW_OK != glewInit()) {
			throw "GLEWが使えません";
		}

		return display;
	}
}


namespace GL {

	class OpenVR : public Core {
	public:
		struct Exception {
			const char* message;
			vr::HmdError code;
		};

		OpenVR();
		~OpenVR();

	private:
		::Display* display;
		struct Pose : public tb::Matrix<4, 4, float> {
			Pose(){};
			Pose(const vr::HmdMatrix44_t& o) { *this = o; };
			Pose(const vr::HmdMatrix34_t& o) { *this = o; };
			void operator=(const vr::HmdMatrix44_t& o) { Transpose(o.m); };
			void operator=(const vr::HmdMatrix34_t& o) {
				TransposeAffine(o.m);
			};
		};

		static constexpr float nearClip = 0.01;
		static constexpr float farClip = 10000;
		vr::TrackedDevicePose_t devicePoses[vr::k_unMaxTrackedDeviceCount];
		Pose headPose;
		vr::IVRSystem& openVR;

		// フレームバッファ他
		Framebuffer::Size renderSize;
		struct Eye {
			Eye(vr::IVRSystem&, vr::EVREye, Framebuffer::Size&);
			const vr::EVREye side;
			Framebuffer framebuffer;
			vr::HmdMatrix44_t projecionMatrix;
			Pose eye2HeadMatrix;
			vr::Texture_t fbFeature;
		} eyes[2];

		// 初期化サポート
		static vr::IVRSystem& GetOpenVR(); // 失敗したら例外
		static Framebuffer::Size GetRenderSize(vr::IVRSystem&);
	};


	OpenVR::OpenVR()
		: display(GetDisplay()), openVR(GetOpenVR()),
		  renderSize(GetRenderSize(openVR)),
		  eyes{
			  {openVR, vr::Eye_Right, renderSize},
			  {openVR, vr::Eye_Left, renderSize}} {

		// 基本設定
		glEnable(GL_POLYGON_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
	}
	OpenVR::~OpenVR() {
		XCloseDisplay((::Display*)display);
		vr::VR_Shutdown();
	}

	// TODO:以下の姿勢取得とOpenVRへの送付を分離(後者はFinishとでも)
	// void OpenVR::Draw(const TB::Matrix<4, 4, float>& view) {
	// 	// 全デバイスの姿勢を取得
	// 	vr::VRCompositor()
	// 		->WaitGetPoses(devicePoses, vr::k_unMaxTrackedDeviceCount, NULL, 0);
	// 	for (unsigned n(0); n < vr::k_unMaxTrackedDeviceCount; ++n) {
	// 		if (devicePoses[n].bPoseIsValid) {
	// 			switch (openVR.GetTrackedDeviceClass(n)) {
	// 			case vr::TrackedDeviceClass_HMD:
	// 				headPose = devicePoses[n].mDeviceToAbsoluteTracking;
	// 				headPose.InvertAffine();
	// 				break;
	// 			default:
	// 				break;
	// 			}
	// 		}
	// 	}

	// 	TG::Scene::SetHeadPose(headPose);

	// 	for (auto& eye : eyes) {
	// 		TB::Framebuffer::Key key(eye.framebuffer);
	// 		glViewport(0, 0, renderSize.width, renderSize.height);

	// 		glMatrixMode(GL_PROJECTION);
	// 		glLoadTransposeMatrixf(&eye.projecionMatrix.m[0][0]);
	// 		glMatrixMode(GL_MODELVIEW);
	// 		Scene::Draw(eye.eye2HeadMatrix * headPose * view);
	// 		vr::VRCompositor()->Submit(eye.side, &eye.fbFeature);
	// 	}
	// }

	vr::IVRSystem& OpenVR::GetOpenVR() {
		Exception exception = {"Failed to initialize OpenVR"};
		if (vr::IVRSystem* const o =
				vr::VR_Init(&exception.code, vr::VRApplication_Scene)) {
			return *o;
		}
		throw exception;
	}

	Framebuffer::Size OpenVR::GetRenderSize(vr::IVRSystem& o) {
		Framebuffer::Size size;
		o.GetRecommendedRenderTargetSize(&size.width, &size.height);
		return size;
	}

	//
	// 片目分
	//
	OpenVR::Eye::Eye(
		vr::IVRSystem& hmd, vr::EVREye eye, Framebuffer::Size& size)
		: side(eye), framebuffer(size),
		  projecionMatrix(hmd.GetProjectionMatrix(side, nearClip, farClip)),
		  eye2HeadMatrix(hmd.GetEyeToHeadTransform(eye)),
		  fbFeature((vr::Texture_t){
			  (void*)(uintptr_t)framebuffer.GetColorBufferID(),
			  vr::TextureType_OpenGL,
			  vr::ColorSpace_Gamma}) {
		eye2HeadMatrix.InvertAffine();
	}
}

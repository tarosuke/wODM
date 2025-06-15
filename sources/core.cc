/** OpenGL番Core
 * Copyright (C) 2024 tarosuke<webmaster@tarosuke.net>
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
#include "core.h"
#include "gl/gl.h"
#include "gl/glx.h"
#include "gl/scenery.h"
#include "pane/rootPane.h"
#include <assert.h>
#include <stdio.h>
#include <syslog.h>
#include <tb/time.h>



tb::Timestamp Core::timestamp;
bool Core::keep(false);
template <> tb::Factory<Core>* tb::Factory<Core>::start(0);


void Core::Run() {
	pose.Identity();
	for (keep = true; keep;) {
		timestamp.Update();

		UpdatePose();

		// 各種Update
		RootPane::UpdateAll(pose, timestamp);
		// World::Update(timestamp);
		GL::Scenery::UpdateAll();

		for (auto& e : eyes) {
			GL::Framebuffer::Key fb(e.framebuffer);
			glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glViewport(0, 0, e.width, e.height);
			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(e.projection);
			glMatrixMode(GL_MODELVIEW);

			/***** 不透明物体
			 * オーバードロー避けでおよそ手前から描画される
			 * またデプスバッファへの書き込み設定を戻す
			 */
			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);

			RootPane::DrawAll(e.eye2Head);

			// opaque World
			glLoadMatrixf(pose * e.eye2Head);
			// World::DrawAll();

			// Scenery
			GL::Scenery::DrawAll();

			/***** 以後透過
			 * wODMにおいて透過は透過率による乗算で実現する。
			 * なのでdepthは評価はされるが書き込まれない。
			 * アルファブレンドが必要なら自分で設定して元に戻す。
			 * 透明な物体は透過率、網などの穴はアルファブレンドを用いる
			 */
			glEnable(GL_BLEND);
			glBlendFunc(GL_ZERO, GL_SRC_COLOR);
			glDepthMask(GL_FALSE);

			// transparent World
			// World::TrawAll();

			// transparent GUI & Navigation
			RootPane::TrawAll(e.eye2Head);

			Finish(e);
		}
		Finish();
	}
}

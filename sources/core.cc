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
#include "gl/gl.h"
#include "gl/glx.h"
#include "gl/scenery.h"
#include "widget/pane.h"
#include "widget/root.h"
#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include <syslog.h>
#include <tb/time.h>



tb::Timestamp Core::timestamp;
tb::Matrix<4, 4, float> Core::pose;
bool Core::keep(false);
template <> tb::Factory<Core>* tb::Factory<Core>::start(0);


struct Login : widget::Pane {
	/*****
	 * ログインを待ち、ログインされたら名前突きパイプを用意してChildをnew
	 * 名前突きパイプに接続されたらaskpass的動作
	 * forkしなかったプロセスでは有名パイプを待つ()
	 * askpassの接続が逆で「画面へ繋ぐ」ことができないのでちょっと考える
	 */
	Login() :
		Pane(tb::Color(widget::Prefs::backColor), position, spread, 0, 0) {};



	static const P position;
	static const S spread;
};

const widget::Frame::P Login::position(0.0f, 0.0f);
const widget::Frame::S Login::spread(256U, 256U);



void Core::Run() {
	pose.Identity();
	widget::Root root(eyes);
	new Login;
	for (keep = true; keep;) {
		timestamp.Update();

		UpdatePose();

		for (const Eye* const e : eyes) {
			Eye::Key key(*e);

			/***** 不透明物
			 * オーバードロー避けでおよそ手前から描画される
			 * またデプスバッファへの書き込み設定を戻す
			 * この領域はブレンドなし、デプス評価、書き換えあり
			 */
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);

			// GUI関連
			root.DrawAll(*e);

			// 通常の物体
			glEnable(GL_CULL_FACE);
			e->Pose(Pose());
			// world::DrawAll(pose * e.eye2Head);
			GL::Scenery::DrawAll();

			/***** 透過物
			 * wODMにおいて透過は透過率による乗算が基本なので順不同ではあるが、
			 * 一応およそ手前から描画される。なお、アルファブレンドを使う場合は
			 * ブレンドモードを元の乗算に戻しておく必要がある。
			 */
			glEnable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glBlendFunc(GL_ZERO, GL_SRC_COLOR);
			glDepthMask(GL_FALSE);

			// 通常の物体
			e->Pose(Pose());
			// World::TrawAll();

			// GUI
			root.TrawAll(*e);

			e->Postdraw();
			Finish(*e);
		}

		// 各種Update
		root.Update();
		// World::Update(timestamp);
		GL::Scenery::UpdateAll();

		Finish();
	}
}

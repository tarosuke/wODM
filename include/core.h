/*****************************************************************************
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
#pragma once

#include "eye.h"
#include <gl/framebuffer.h>
#include <tb/factory.h>
#include <tb/geometry/spread.h>
#include <tb/list.h>
#include <tb/matrix.h>
#include <tb/time.h>



struct Core {
	static Core* New() { return tb::Factory<Core>::Create(); };
	virtual ~Core() {};

	void Run();

	// 周回処理へ引数として渡すオーバーヘッド避けのアクセサ
	static const tb::Timestamp& Timestamp() { return timestamp; };
	static const tb::Matrix<4, 4, float>& Pose() { return pose; };

protected:
	static constexpr float nearClip = 0.01;
	static constexpr float farClip = 10000;

	Core() = default;

	void Register(Eye& e) { eyes.Add(e); };

	/***** 頭の姿勢 */
	static tb::Matrix<4, 4, float> pose;
	virtual void UpdatePose() = 0;

	/***** フレームバッファを画面へ出力
	 * NextEyeで返す行列の視点番号を最初に戻す
	 */
	virtual void Finish(const Eye&) {};
	virtual void Finish() = 0;

	/***** 繰り返しから抜ける */
	static void Quit() { keep = false; };


private:
	static bool keep;
	static tb::Timestamp timestamp;
	const tb::Matrix<4, 4, float>* projection; // 投影(左右の目はこっちで分ける)
	tb::List<Eye> eyes;

	Core(const Core&) = delete;
};

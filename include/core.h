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

#include <tb/factory.h>
#include <tb/list.h>
#include <tb/matrix.h>
#include <tb/spread.h>
#include <tb/time.h>



struct Core {
	struct Object : tb::List<Object> {
		virtual void Draw() {};
		virtual void Traw() {};
	};

	static Core* New() { return tb::Factory<Core>::Create(); };
	virtual ~Core(){};

	void Run();
	static const tb::Timestamp& Timestamp() { return timestamp; };

protected:
	Core(const Core&) = delete;

	Core() = default;

	/***** 姿勢を取得 */
	virtual const tb::Matrix<4, 4, float>& Pose() = 0;

	/***** 次の視野情報取得、設定
	 * 暗黙の描画先を設定
	 * 投影行列のポインタを返す／すべての視野が終了していたら0を返す
	 */
	virtual const tb::Matrix<4, 4, float>* NextEye() = 0;

	/***** フレームバッファを画面へ出力
	 * NextEyeで返す行列の視点番号を最初に戻す
	 */
	virtual void Finish() = 0;

	/***** 繰り返しから抜ける */
	static void Quit() { keep = false; };


private:
	static bool keep;
	static tb::Timestamp timestamp;
	const tb::Matrix<4, 4, float>* projection; // 投影(左右の目はこっちで分ける)
};

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

#include <gl/framebuffer.h>
#include <tb/factory.h>
#include <tb/list.h>
#include <tb/matrix.h>
#include <tb/spread.h>
#include <tb/time.h>
#include <vector>



struct Core {
	struct Object : tb::List<Object> {
		virtual void Draw() {};
		virtual void Traw() {};
	};

	static Core* New() { return tb::Factory<Core>::Create(); };
	virtual ~Core() {};

	void Run();
	static const tb::Timestamp& Timestamp() { return timestamp; };

protected:
	static constexpr float nearClip = 0.01;
	static constexpr float farClip = 10000;

	struct Eye {
		Eye() {};
		Eye(Eye&& o) { *this = std::move(o); };
		Eye(unsigned width, unsigned height) :
			framebuffer(width, height),
			width(width),
			height(height) {};
		void operator=(Eye&& o) {
			projection = o.projection;
			eye2Head = o.eye2Head;
			memo = o.memo;
			width = o.width;
			height = o.height;
			framebuffer = std::move(o.framebuffer);
		};
		tb::Matrix<4, 4, float> projection; // Transpose(GetProjectionMatrix)
		tb::Matrix<4, 4, float>
			eye2Head; // Transpose(GetEyeToHeadTransform()).InvertAffine()
		GL::Framebuffer framebuffer;
		unsigned memo;
		unsigned width;
		unsigned height;
	};
	using Eyes = std::vector<Eye>;
	Eyes& eyes;

	Core(Eyes& e) : eyes(e) {};

	/***** 姿勢を取得 */
	tb::Matrix<4, 4, float> pose;
	virtual void UpdatePose() = 0;

	/***** フレームバッファを画面へ出力
	 * NextEyeで返す行列の視点番号を最初に戻す
	 */
	virtual void Finish(Eye&) {};
	virtual void Finish() = 0;

	/***** 繰り返しから抜ける */
	static void Quit() { keep = false; };


private:
	static bool keep;
	static tb::Timestamp timestamp;
	const tb::Matrix<4, 4, float>* projection; // 投影(左右の目はこっちで分ける)

	Core(const Core&) = delete;
	Core() = delete;
};

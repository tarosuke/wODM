/*****************************************************************************
 * Copyright (C) 2025 tarosuke<webmaster@tarosuke.net>
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

#include "gl/texture.h"
#include "gl/vbo.h"
#include "model.h"
#include "pane.h"
#include <tb/prefs.h>



struct RootPane : Pane {
	struct Nav {
		float in;  // 内径
		float out; // 外径
		float th;  // 太さ
	}; // ナビゲーションリングの諸元

	void UpdateAll(const A&, const tb::Timestamp&);
	void DrawAll(const A& eye2Head);
	void TrawAll(const A& eye2Head);
	void DotNavigation(const P& lookintPoint, const P& center);

	RootPane();
	static void UpdateNav();

private:
	struct M : tb::BufferedImage, Model_C {
		M() :
			BufferedImage(
				tb::Color::Format::Select(tb::Color::Format::XRGB0888),
				512,
				512),
			Model_C(params, *this, textureStyle) {
			Update();
		};
		static constexpr unsigned nVertex = 16;
		static constexpr unsigned nIndex = 16;
		static GL::VBO::V_UV vertexBuffer[nVertex];
		static const unsigned indexBuffer[nIndex][3];
		static const Model_C::Params params;
		static const GL::Texture::Style textureStyle;
		static void Update(); // 頂点バッファのUVと後半を作る
	} navigationPanel;

	static tb::Prefs<float> pDistance;
	static tb::Prefs<float> vDistance;
	static tb::Prefs<float> scale;
	static tb::Prefs<float> navigationAngle;
	static tb::Prefs<float> navigationThick;

	static P lookingPoint;
	static Nav nav;

	void Traw() final;
};

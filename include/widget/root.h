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

#include "base.h"
#include "model.h"



namespace widget {

	// いわゆるルート
	struct Root : Base {
		Root() {
			instance = this;
			notify.pick = 0;
		};

		void UpdateAll(const M&, const tb::Timestamp&);
		void DrawAll(const M&);
		void TrawAll();

		struct Nav {
			float in;  // 内径
			float out; // 外径
			float th;  // 太さ
			float ior; // 内外比(in/out)
		}; // ナビゲーションリングの諸元


	private:
		static tb::Prefs<float> pDistance;
		static tb::Prefs<float> vDistance;
		static tb::Prefs<float> scale;
		static tb::Prefs<float> navigationAngle;
		static tb::Prefs<float> navigationThick;

		static Root* instance;
		static Nav nav;
		static P lookingPoint;

		// NavPanel関連
		struct I : tb::BufferedImage {
			I();
		};
		struct M : I, Model_C {
			M() : Model_C(PrepareParams(), *this, textureStyle) {};
			static constexpr unsigned nVertex = 16;
			static constexpr unsigned nTriangles = 18;
			static GL::VBO::V_UV vertexBuffer[nVertex];
			static const unsigned indexBuffer[nTriangles][3];
			static const Model_C::Params params;
			static const GL::Texture::Style textureStyle;
			static const Model_C::Params& PrepareParams();
		} navigationPanel;
		void DotNavigation();
	};
}

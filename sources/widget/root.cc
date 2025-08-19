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
#include "widget/root.h"
#include "gl/gl.h"



namespace widget {

	tb::Prefs<float> Root::pDistance(
		"widget/paneDistance", 1.0f, "一番手前のWidgetが見える奥行き[m]");
	tb::Prefs<float> Root::vDistance(
		"widget/virtualDistance",
		1.0f,
		"視野中心を決めるための視点の投影面までの距離[m]");
	tb::Prefs<float> Root::scale("widget/scale", 0.001f, "1pxのサイズ[m]");
	tb::Prefs<float> Root::navigationAngle(
		"widget/navigationAngle",
		0.4f,
		"ナビゲーションリングの外径視野角[対奥行比]");
	tb::Prefs<float> Root::navigationThick(
		"widget/navigationAngle", 16.0f, "ナビゲーションリングの太さ[px]");


	Root* Root::instance(0);
	Root::Nav Root::nav;
	Base::P Root::lookingPoint;


	void Root::UpdateAll(const Base::M& pose, const tb::Timestamp& ts) {
		// lookingPoint算出
		const tb::Vector<3, float> fv((const float[3]){0.0f, 0.0f, 1.0f});
		const tb::Vector<3, float> lv(pose * fv + fv); // 正面と頭の向きの中間
		lookingPoint = {
			lv[0] * (float)vDistance / (lv[2] * scale),
			lv[1] * (float)vDistance / (lv[2] * scale)};

		// 子要素Update
		const auto& n(instance->Update(ts));
		if (n.pick) {
			// 奥行再計算
			n.pick->ReDepth();
		}
	}

	void Root::DrawAll(const Base::M& e2h) {
		// Navigation描画
		glLoadMatrixf((const float*)e2h);
		glColor3f(1, 1, 1);
		glPointSize(16);
		glScalef(1.0f / nav.out, 1.0f / nav.out, 1.0);
		glPushMatrix();
		glTranslatef(0, 0, -pDistance);

		glBegin(GL_POINTS);
		DotNavigation(); // 直接の子全てのGetCenterで点を打つ
		glEnd();
		glPopMatrix();

		// 描画
		// glMultMatrixf((const float*)headMatrix);
		glTranslatef(lookingPoint[0], lookingPoint[1], -pDistance);

		// 窓はNevより遠いので輝点のあとに描画
		glColor3f(1, 1, 1);
		Draw();
	}

	void Root::TrawAll() { navigationPanel.Draw(); }


	void Root::DotNavigation() {
		for (tb::List<Base>::I i(children); ++i;) {
			tb::Vector<2, float> p(lookingPoint - (*i).GetCenter());
			const float norm(p.Norm());
			if (norm <= nav.in) {
				// ナビゲーションサークル以内は輝点を表示しない
				continue;
			}

			// リングの内外径に内径-無限遠が収まるよう極軸座標系で計算
			const float r(nav.out - nav.th / (norm - nav.in));
			const tb::Vector<2, float> pp(p * r / norm);
			glVertex2f(pp[0], pp[1]); // glBegin/glEndは処理全体
		}
	}



	Root::I::I() :
		BufferedImage(
			tb::Color::Format::Select(tb::Color::Format::XRGB0888), 512, 512) {
		// NavPanelの描画
		const tb::Color cc(0x80ffffff);
		const tb::Color gc(0x8080c080);
		const unsigned r2(Width() * Width());
		const unsigned i2(r2 * nav.ior * nav.ior);
		const unsigned o2(r2);
		for (unsigned y(0); y < Height(); ++y) {
			for (unsigned x(0); x < Width(); ++x) {
				const unsigned d2(x * x + y * y);
				Set(x, y, i2 < d2 && d2 < o2 ? gc : cc);
			}
		}
	};
	static constexpr float outP = 0.41421356237f; // 八角形の角の位置
	static constexpr float inR = 0.923879532513;  // 外接→内接のサイズ比


	GL::VBO::V_UV Root::M::vertexBuffer[nVertex] = {
		// U/V座標が頂点と一致しているのは頂点が割合でテクスチャ境界がミラーなので
		{{-outP, -1, 0}, {-outP, -1}}, // 0
		{{outP, -1, 0}, {outP, -1}},   // 1
		{{1, -outP, 0}, {1, -outP}},   // 2
		{{1, outP, 0}, {1, outP}},	   // 3
		{{outP, 1, 0}, {outP, 1}},	   // 4
		{{-outP, 1, 0}, {-outP, 1}},   // 5
		{{-1, outP, 0}, {-1, outP}},   // 6
		{{-1, -outP, 0}, {-1, -outP}}, // 7
									   // 8以降は0-7の内側
	};
	const unsigned Root::M::indexBuffer[nTriangles][3] = {
		{0, 1, 8},	{8, 1, 9},	 {1, 2, 9},	 {9, 2, 10},
		{2, 3, 10}, {10, 3, 11}, {3, 4, 11}, {11, 4, 12},
		{4, 5, 12}, {12, 5, 13}, {5, 6, 13}, {13, 6, 14},
		{6, 7, 14}, {14, 7, 15}, {7, 0, 15}, {15, 0, 8}};

	const Model_C::Params Root::M::params = {
		numOfIndex : nTriangles * 3,
		index : &indexBuffer[0][0],
		numOfVertex : nVertex,
		vertex : vertexBuffer
	};

	const GL::Texture::Style Root::M::textureStyle = {
		wrap_s : GL_MIRRORED_REPEAT,
		wrap_t : GL_MIRRORED_REPEAT,
		filter_mag : GL_LINEAR,
		filter_min : GL_LINEAR,
		texture_mode : GL_REPLACE,
		pointSprite : false,
	};


	const Model_C::Params& Root::M::PrepareParams() {
		// 頂点の後半を作る
		for (unsigned n(0); n < 8; ++n) {
			auto& out(vertexBuffer[n]);
			auto& in(vertexBuffer[n + 8]);
			in.texture.u = in.vertex.x = out.vertex.x * inR * nav.ior;
			in.texture.v = in.vertex.y = out.vertex.y * inR * nav.ior;
			in.vertex.z = out.vertex.z;
		}
		return params;
	}

}

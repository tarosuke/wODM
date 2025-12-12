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
#include "core.h"
#include "gl/gl.h"
#include "widget/prefs.h"
#include <functional>



namespace {
	static constexpr float outP = 0.41421356237f; // 八角形の角の位置
	static constexpr float inR = 0.923879532513;  // 内接/概説のサイズ比
	static constexpr unsigned nVertex = 16;
	static constexpr unsigned nTriangles = 18;


	// VBOのためのデータ
	GL::VBO::V_UV vertexBuffer[nVertex] = {
		// U/V座標が頂点と一致しているのは頂点が割合でテクスチャ境界がミラー故
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
	const unsigned indexBuffer[nTriangles][3] = {{0, 1, 8}, {8, 1, 9},
		{1, 2, 9}, {9, 2, 10}, {2, 3, 10}, {10, 3, 11}, {3, 4, 11}, {11, 4, 12},
		{4, 5, 12}, {12, 5, 13}, {5, 6, 13}, {13, 6, 14}, {6, 7, 14},
		{14, 7, 15}, {7, 0, 15}, {15, 0, 8}};
	const Model_C::Params params = {
		numOfIndex : nTriangles * 3,
		index : &indexBuffer[0][0],
		numOfVertex : nVertex,
		vertex : vertexBuffer
	};
	const GL::Texture::Style textureStyle = {
		wrap_s : GL_MIRRORED_REPEAT,
		wrap_t : GL_MIRRORED_REPEAT,
		filter_mag : GL_LINEAR,
		filter_min : GL_LINEAR,
		texture_mode : GL_REPLACE,
		pointSprite : false,
	};

	unsigned out;
	float in;	  // 内径
	float ior;	  // 内外比(in/out)
	float nScale; // ナビゲーションリングの実際の大きさ

}

namespace widget {

	Root* Root::instance(0);
	Frame::P Root::lookingPoint;


	Root::Root(const tb::List<Eye>& eyes) : navPanel(PrepareNavPanel(eyes)) {
		instance = this;
	};
	Root::~Root() {
		if (navPanel) {
			delete navPanel;
		}
	}

	void Root::Register(Window& w) {
		instance->windows.Add(static_cast<Frame&>(w));
	}

	void Root::Update() {
		// 必要なら順序を変更して奥行き再計算
		Notify n;
		for (tb::List<Frame>::I i(windows); ++i;) {
			const Notify nn((*i).Update());
			n.raw |= nn.raw;
		}
		if (n.thickUpdated) {
			float d(0);
			for (tb::List<Frame>::I i(windows); ++i;) { d += (*i).GetDepth(); }
		}
	}

	void Root::DrawAll(const Eye& eye) {
		// lookingPoint算出、適用
		const tb::Vector<3, float> fv((const float[3]){0.0f, 0.0f, 1.0f});
		const tb::Vector<3, float> lv(
			Core::Pose() * fv + fv); // 正面と頭の向きの中間
		const Frame::P lpTarget = {
			-lv[0] * (float)Prefs::vDistance / (lv[2] * Prefs::scale),
			lv[1] * (float)Prefs::vDistance / (lv[2] * Prefs::scale)};

#if 0
		lookingPoint += (lpTarget - lookingPoint) / (lpTarget.Norm() + 1);
#else
		lookingPoint[0] +=
			(lpTarget[0] - lookingPoint[0]) / (fabsf(lpTarget[0]) + 1);
		lookingPoint[1] +=
			(lpTarget[1] - lookingPoint[1]) / (fabsf(lpTarget[1]) + 1);
#endif

		mask = Frame::R(Frame::P2(lookingPoint[0] - eye.width,
							lookingPoint[1] - eye.height),
			Frame::P2(
				lookingPoint[0] + eye.width, lookingPoint[1] + eye.height));

		glDisable(GL_CULL_FACE);

		glColor3f(1, 1, 1);
		glPointSize(3);
		eye.GUI();
		glPushMatrix();
		glTranslatef(0, 0, -Prefs::nDistance);
		glScalef(nScale, nScale, 1);
		glBegin(GL_POINTS);
		windows.Foreach(&Frame::Dot);
		glEnd();
		glPopMatrix();

		glTranslatef(-lookingPoint[0], -lookingPoint[1], -Prefs::pDistance);
		windows.Foreach(&Frame::DrawEntity, GetMask());
	}

	void Root::TrawAll(const Eye& eye) {
		glDisable(GL_CULL_FACE);

		glColor4f(1, 1, 1, 1);
		eye.Short11();
		glTranslatef(0, 0, -Prefs::nDistance);
		glScalef(nScale, nScale, 1);
		navPanel->Draw();


		eye.GUI();
		glTranslatef(-lookingPoint[0], -lookingPoint[1], -Prefs::pDistance);
		windows.Foreach(&Frame::TrawEntity);
	}


	Model_C* Root::PrepareNavPanel(const tb::List<Eye>& eyes) {
		out = (eyes.Top()->min) / 2;

		in = out - Prefs::navThick;
		ior = in / out;
		nScale = Prefs::nDistance / Prefs::pDistance;

		/***** パネル画像生成
		 */
		tb::Image* image(new tb::BufferedImage(
			tb::Color::Format::Select(tb::Color::Format::XRGB0888), out, out));

		// NavPanelの描画
		const tb::Color cc(0xffffff);
		const tb::Color gc(Prefs::navRingColor);
		const unsigned r2(out * out);
		const unsigned i2(r2 * ior * ior);
		const unsigned o2(r2);
		for (unsigned y(0); y < out; ++y) {
			for (unsigned x(0); x < out; ++x) {
				const unsigned d2(x * x + y * y);
				image->Set(x, y, i2 < d2 && d2 < o2 ? gc : cc);
			}
		}

		/***** モデル生成
		 */
		// 頂点の後半を作る(navThickが可変なので動的計算が必要)
		for (unsigned n(0); n < 8; ++n) {
			auto& o(vertexBuffer[n]);
			auto& i(vertexBuffer[n + 8]);
			i.texture.u = i.vertex.x = o.vertex.x * inR * ior;
			i.texture.v = i.vertex.y = o.vertex.y * inR * ior;
			i.vertex.z = o.vertex.z;
		}

		Model_C* np(new Model_C(params, *image, textureStyle));
		delete image;
		return np;
	}

	void Root::Dot(const Frame::P& p) {
		const Frame::P pp(p - lookingPoint);
		const float n(pp.Norm());
		if (n <= in) {
			// ナビゲーションリングの内側なので表示しない
			return;
		}

		const Frame::P ppp(pp * (out - ((Prefs::navThick * in) / n)) / n);
		glVertex2f(ppp[0], ppp[1]);
	}
}

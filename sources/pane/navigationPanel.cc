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
#include "gl/gl.h"
#include "pane/rootPane.h"



static constexpr float outP = 0.41421356237f; // 八角形の角の位置
static constexpr float inR = 0.923879532513;  // 外接→内接のサイズ比


GL::VBO::V_UV RootPane::M::vertexBuffer[nVertex] = {
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
const unsigned RootPane::M::indexBuffer[nTriangles][3] = {
	{0, 1, 8},	{8, 1, 9},	 {1, 2, 9},	 {9, 2, 10},  {2, 3, 10}, {10, 3, 11},
	{3, 4, 11}, {11, 4, 12}, {4, 5, 12}, {12, 5, 13}, {5, 6, 13}, {13, 6, 14},
	{6, 7, 14}, {14, 7, 15}, {7, 0, 15}, {15, 0, 8}};

const Model_C::Params RootPane::M::params = {
	numOfIndex : nTriangles * 3,
	index : &indexBuffer[0][0],
	numOfVertex : nVertex,
	vertex : vertexBuffer
};

const GL::Texture::Style RootPane::M::textureStyle = {
	wrap_s : GL_MIRRORED_REPEAT,
	wrap_t : GL_MIRRORED_REPEAT,
	filter_mag : GL_LINEAR,
	filter_min : GL_LINEAR,
	texture_mode : GL_REPLACE,
	pointSprite : false,
};


const Model_C::Params& RootPane::M::PrepareParams() {
	// リングの内径の外径に対する割合
	const float inr(RootPane::nav.in / RootPane::nav.out);

	// 頂点の後半を作る
	for (unsigned n(0); n < 8; ++n) {
		auto& out(vertexBuffer[n]);
		auto& in(vertexBuffer[n + 8]);
		in.texture.u = in.vertex.x = out.vertex.x * inR * inr;
		in.texture.v = in.vertex.y = out.vertex.y * inR * inr;
		in.vertex.z = out.vertex.z;
	}
	return params;
}


RootPane::I::I() :
	BufferedImage(
		tb::Color::Format::Select(tb::Color::Format::XRGB0888), 512, 512) {
	// NavPanelの描画
	const tb::Color cc(0x80ffffff);
	const tb::Color gc(0x8080c080);
	const unsigned r2(Width() * Width());
	const unsigned i2(r2 * inR * inR);
	const unsigned o2(r2);
	for (unsigned y(0); y < Height(); ++y) {
		for (unsigned x(0); x < Width(); ++x) {
			const unsigned d2(x * x + y * y);
			Set(x, y, i2 < d2 && d2 < o2 ? gc : cc);
		}
	}
};

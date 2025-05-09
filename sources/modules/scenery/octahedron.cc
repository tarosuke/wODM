/*
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
#include "gl/scenery.h"
#include "gl/texture.h"

/***** 正八面体背景
 * テクスチャ上では下のような並びにあるとき
 * ABC
 * DEF
 * GHI
 * 正八面体を上から見ると下のような並びになる(XはACGIと底がE)
 *  B
 * DXF
 *  H
 * 使うときはメンバ変数を頂点バッファ、インデクスバッファにする
 * 正八面体のサイズは-1〜1なので使いたいサイズにスケールする
 */
struct Octahedron : Scenery {
	Octahedron(
		const Params& params,
		const tb::Image& image,
		const GL::Texture::Style& style) :
		Scenery(params, image, style) {};

	static constexpr unsigned nVertex = 9;
	static const GL::VBO::V_UV vertexBuffer[nVertex];
	static constexpr unsigned nIndex = 8;
	static const unsigned indexBuffer[nIndex][3];

	static const Model_C::Params params;

	struct F : public Factory {
		Scenery* New(const tb::Image& i) final {
			return new Octahedron(params, i, GL::Texture::defaultStyle);
		};
		uint Score(const tb::Image& i) final {
			return i.Width() == i.Height() ? Certitude::activeMatch : 0;
		};
	};
	static F factory;
};

Octahedron::F Octahedron::factory;

const GL::VBO::V_UV Octahedron::vertexBuffer[nVertex] = {
	{{0, 0, 1}, {0, 0}},	{{0, 1, 0}, {0.5, 0}},	  {{0, 0, 1}, {1, 0}},
	{{-1, 0, 0}, {0, 0.5}}, {{0, 0, -1}, {0.5, 0.5}}, {{1, 0, 0}, {1, 0.5}},
	{{0, 0, 1}, {0, 1}},	{{0, -1, 0}, {0.5, 1}},	  {{0, 0, 1}, {1, 1}},
};
const unsigned Octahedron::indexBuffer[nIndex][3] = {
	{0, 1, 3}, {2, 5, 1}, {8, 5, 7}, {6, 7, 3},
	{4, 3, 1}, {4, 1, 5}, {4, 5, 7}, {4, 7, 3}};

const Model_C::Params Octahedron::params = {
	numOfIndex : nIndex * 3,
	index : &indexBuffer[0][0],
	numOfVertex : nVertex,
	vertex : vertexBuffer
};

/** Skybox
 * Copyright (C) 2017,2019 tarosuke<webmaster@tarosuke.net>
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

#include <assert.h>
#include <syslog.h>

#include "gl/gl.h"
#include "gl/scenery.h"



#define elementsOf(a) (sizeof(a) / sizeof(a[0]))



class Skybox : public Scenery {

	Skybox(const Params& params, const tb::Image<tb::Pixel<tb::u8>>& image)
		: Scenery(params, image) {};

	static constexpr int scale = 5000;
	struct F : Scenery::Factory {
		Scenery* New() final;
		uint Score() final;
		Scenery* New(const Param*) final { return 0; };
		uint Score(const Param*) final { return 0; };
	};
	static F factory;
	static unsigned indexes[];
	static GL::VBO::V_UV vertexes[];
	static tb::Prefs<tb::String> path;
	static constexpr int defaultTextureScale = 3;
	static GL::VBO::V_UV defaultVertexes[];
	static const tb::Pixel<tb::u8> defaultTexture[8][8];
	static constexpr tb::u32 bc = 0x00101020;
	static constexpr tb::u32 fc = 0x00404060;
};

// コマンドラインオプション--skybox
tb::Prefs<tb::String> Skybox::path("--skybox", "背景にスカイボックスを使う");



Skybox::F Skybox::factory;

unsigned Skybox::indexes[] = {
	0,	2,	1, 2,  3,  1, // left
	2,	4,	3, 4,  5,  3, // front
	4,	6,	5, 6,  7,  5, // right
	6,	8,	7, 8,  9,  7, // rear
	2,	10, 4, 10, 11, 4, // top
	12, 13, 9, 13, 7,  9 // bottom
};

GL::VBO::V_UV Skybox::vertexes[] = {
	// 0:左後方
	{{-scale, scale, scale}, {0.0 / 4, 1.0 / 3}},
	{{-scale, -scale, scale}, {0.0 / 4, 2.0 / 3}},
	// 2:左前方
	{{-scale, scale, -scale}, {1.0 / 4, 1.0 / 3}},
	{{-scale, -scale, -scale}, {1.0 / 4, 2.0 / 3}},
	// 4:右前方
	{{scale, scale, -scale}, {2.0 / 4, 1.0 / 3}},
	{{scale, -scale, -scale}, {2.0 / 4, 2.0 / 3}},
	// 6:右後方
	{{scale, scale, scale}, {3.0 / 4, 1.0 / 3}},
	{{scale, -scale, scale}, {3.0 / 4, 2.0 / 3}},
	// 8:左後方(後方テクスチャ用)
	{{-scale, scale, scale}, {4.0 / 4, 1.0 / 3}},
	{{-scale, -scale, scale}, {4.0 / 4, 2.0 / 3}},
	// 10:上後方左右
	{{-scale, scale, scale}, {1.0 / 4, 0.0 / 3}},
	{{scale, scale, scale}, {2.0 / 4, 0.0 / 3}},
	// 12:下後方左右
	{{-scale, -scale, -scale}, {4.0 / 4, 3.0 / 3}},
	{{scale, -scale, -scale}, {3.0 / 4, 3.0 / 3}},
};
GL::VBO::V_UV Skybox::defaultVertexes[] = {
	// 0:左後方
	{{-scale, scale, scale},
	 {0.0 * defaultTextureScale, 1.0 * defaultTextureScale}},
	{{-scale, -scale, scale},
	 {0.0 * defaultTextureScale, 2.0 * defaultTextureScale}},
	// 2:左前方
	{{-scale, scale, -scale},
	 {1.0 * defaultTextureScale, 1.0 * defaultTextureScale}},
	{{-scale, -scale, -scale},
	 {1.0 * defaultTextureScale, 2.0 * defaultTextureScale}},
	// 4:右前方
	{{scale, scale, -scale},
	 {2.0 * defaultTextureScale, 1.0 * defaultTextureScale}},
	{{scale, -scale, -scale},
	 {2.0 * defaultTextureScale, 2.0 * defaultTextureScale}},
	// 6:右後方
	{{scale, scale, scale},
	 {3.0 * defaultTextureScale, 1.0 * defaultTextureScale}},
	{{scale, -scale, scale},
	 {3.0 * defaultTextureScale, 2.0 * defaultTextureScale}},
	// 8:左後方(後方テクスチャ用)
	{{-scale, scale, scale},
	 {4.0 * defaultTextureScale, 1.0 * defaultTextureScale}},
	{{-scale, -scale, scale},
	 {4.0 * defaultTextureScale, 2.0 * defaultTextureScale}},
	// 10:上後方左右
	{{-scale, scale, scale},
	 {1.0 * defaultTextureScale, 0.0 * defaultTextureScale}},
	{{scale, scale, scale},
	 {2.0 * defaultTextureScale, 0.0 * defaultTextureScale}},
	// 12:下後方左右
	{{-scale, -scale, -scale},
	 {4.0 * defaultTextureScale, 3.0 * defaultTextureScale}},
	{{scale, -scale, -scale},
	 {3.0 * defaultTextureScale, 3.0 * defaultTextureScale}},
};

uint Skybox::F::Score() {
	if (!((std::string)path).size()) {
		// 他に何も指定されていなければこれが選択される
		return Certitude::passiveMatch;
	}
	return 0U;
};

Scenery* Skybox::F::New() {
	if (!((std::string)path).size()) {
		/** 背景が何も選択されなかったので最小限の背景になる
		 * 即ち、中央に十字の線が入った小さいテクスチャの繰り返しを使う
		 * 一マスに入る線をn本とするとU/V座標を横4n倍、縦3n倍にする
		 */
		const Params params = {
			numOfIndex : elementsOf(Skybox::indexes),
			index : Skybox::indexes,
			numOfVertex : elementsOf(Skybox::defaultVertexes),
			vertex : Skybox::defaultVertexes
		};
		const tb::Image<tb::Pixel<tb::u8>> image((void*)defaultTexture, 8, 8);
		return new Skybox(params, image);
	}

	return 0;
};



/***** デフォルトのテクスチャ
 */
const tb::Pixel<tb::u8> Skybox::defaultTexture[8][8] = {
	{bc, bc, bc, fc, fc, bc, bc, bc},
	{bc, bc, bc, fc, fc, bc, bc, bc},
	{bc, bc, bc, fc, fc, bc, bc, bc},
	{fc, fc, fc, fc, fc, fc, fc, fc},
	{fc, fc, fc, fc, fc, fc, fc, fc},
	{bc, bc, bc, fc, fc, bc, bc, bc},
	{bc, bc, bc, fc, fc, bc, bc, bc},
	{bc, bc, bc, fc, fc, bc, bc, bc},
};

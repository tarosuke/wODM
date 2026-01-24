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

#include "pane.h"



namespace widget {

	struct Login : widget::VerticalList {
		/*****
		 * ログインを待ち、ログインされたら名前突きパイプを用意してChildをnew
		 * 名前突きパイプに接続されたらaskpass的動作
		 * forkしなかったプロセスでは有名パイプを待つ()
		 * askpassの接続が逆で「画面へ繋ぐ」ことができないのでちょっと考える
		 */
		Login() :
			VerticalList(spread),
			user(*this, 14, 12, "username"),
			password(*this, 14, 12, "password") {};



		static const S spread;

		widget::VerticalList::Item<widget::LineInputPane> user;
		widget::VerticalList::Item<widget::LineInputPane> password;
	};
}

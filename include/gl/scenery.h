/** Scenery
 * Copyright (C) 2017,2019,2024 tarosuke<webmaster@tarosuke.net>
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
 *
 * スカイスフィアやスカイボックスなどの抽象
 */
#pragma once

#include <filesystem>

#include <tb/factory.h>
#include <tb/image.h>
#include <tb/prefs.h>
#include <tb/string.h>

#include "model.h"



class Scenery : public Model_C {
	Scenery() = delete;
	Scenery(const Scenery&) = delete;
	void operator=(const Scenery&) = delete;

public:
	using Factory = tb::Factory<Scenery>;
	Scenery* New(const std::filesystem::path& path);

	struct Param : Factory::Param {
		Param(tb::Image<tb::Pixel<tb::u8>>& image) : image(image) {};
		tb::Image<tb::Pixel<tb::u8>>& image;
	};

protected:
	Scenery(const Params&, const tb::Image<tb::Pixel<tb::u8>>&);

private:
	static Scenery* instance;
};

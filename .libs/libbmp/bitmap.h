/*
 * bitmap.h
 * Copyright (C) 2013 David Jolly
 * ----------------------
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BITMAP_H_
#define BITMAP_H_

#include <cstdint>
#include <string>

namespace bitmap {

	enum {
		CHANNEL_BLUE = 0,
		CHANNEL_GREEN,
		CHANNEL_RED,
	};

	#define MAX_CHANNEL CHANNEL_RED
	#define NULL_PIXEL {0}
	#define NULL_UNIQUE_ID {0}
	#define UNIQUE_ID_BLOCKS (2)

#pragma pack(push)
#pragma pack(1)
	typedef struct _pixel_t {
		uint8_t channel[MAX_CHANNEL + 1];
	} pixel_t, *pixel_ptr;
#pragma pack(pop)

	typedef struct _uid_t {
		uint64_t block[UNIQUE_ID_BLOCKS];
	} uid_t, *uid_ptr;

	bool operator==(
		const pixel_t &left,
		const pixel_t &right
		);

	bool operator!=(
		const pixel_t &left,
		const pixel_t &right
		);

	bool operator==(
		const uid_t &left,
		const uid_t &right
		);

	bool operator!=(
		const uid_t &left,
		const uid_t &right
		);

	void cleanup(void);

	bool contains(
		const uid_t &id
		);

	std::string convert_pixel_to_string(
		const pixel_t &px
		);

	size_t count(void);

	uid_t create(
		uint32_t width,
		uint32_t height
		);

	pixel_t get_pixel(
		uint32_t x,
		uint32_t y,
		const uid_t &id
		);

	void initialize(void);

	uid_t read(
		const std::string &path
		);

	void remove(
		const uid_t &id
		);

	void remove_all(void);

	void set_pixel(
		uint32_t x,
		uint32_t y,
		const pixel_t &px,
		const uid_t &id
		);

	std::string version(
		bool verbose
		);

	void write(
		const std::string &path,
		const uid_t &id
		);
}

#endif
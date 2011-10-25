/*!The Tiny Platform Library
 * 
 * TPlat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TPlat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TPlat; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		tbox.c
 *
 */

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* ////////////////////////////////////////////////////////////////////////
 * helper
 */
static tb_bool_t tb_check_word_order()
{
	tb_uint16_t x = 0x1234;
	tb_byte_t const* p = (tb_byte_t const*)&x;

#ifdef TB_WORDS_BIGENDIAN
	// is big endian?
	return (p[0] == 0x12 && p[1] == 0x34)? TB_TRUE : TB_FALSE;
#else
	// is little endian?
	return (p[0] == 0x34 && p[1] == 0x12)? TB_TRUE : TB_FALSE;
#endif
}
static tb_bool_t tb_check_float_order()
{
#ifdef TB_CONFIG_TYPE_FLOAT
	union 
	{
		tb_uint32_t i[2];
		double 		f;

	} conv;
	conv.f = 1.0f;

# 	ifdef TB_FLOAT_BIGENDIAN
	// is big endian?
	return (!conv.i[1] && conv.i[0])? TB_TRUE : TB_FALSE;
# 	else
	// is little endian?
	return (!conv.i[0] && conv.i[1])? TB_TRUE : TB_FALSE;
# 	endif
#else
	return TB_TRUE;
#endif
}
/* ////////////////////////////////////////////////////////////////////////
 * implemention
 */

tb_bool_t tb_init(tb_byte_t* data, tb_size_t size)
{
	TB_DBG("init: %x %d", data, size);

	// check types
	TB_STATIC_ASSERT(sizeof(tb_byte_t) == 1);
	TB_STATIC_ASSERT(sizeof(tb_uint_t) == 4);
	TB_STATIC_ASSERT(sizeof(tb_uint8_t) == 1);
	TB_STATIC_ASSERT(sizeof(tb_uint16_t) == 2);
	TB_STATIC_ASSERT(sizeof(tb_uint32_t) == 4);
	TB_STATIC_ASSERT(sizeof(tb_uint64_t) == 8);
	TB_STATIC_ASSERT(TB_CPU_BITSIZE == (sizeof(tb_size_t) << 3));
	TB_STATIC_ASSERT(TB_CPU_BITSIZE == (sizeof(tb_long_t) << 3));
	TB_STATIC_ASSERT(TB_CPU_BITSIZE == (sizeof(tb_void_t*) << 3));
	TB_STATIC_ASSERT(TB_CPU_BITSIZE == (sizeof(tb_handle_t) << 3));

	// check byteorder
	TB_ASSERT(tb_check_word_order());
	TB_ASSERT(tb_check_float_order());

#ifdef TB_CONFIG_MEMORY_POOL_ENABLE
	// init memory pool
	if (!tb_memory_init(data, size)) return TB_FALSE;
#endif

	// init socket
	if (!tb_socket_init()) return TB_FALSE;

	// ok
	TB_DBG("init: ok");

	return TB_TRUE;
}

tb_void_t tb_exit()
{
	// exit socket
	tb_socket_exit();

#ifdef TB_CONFIG_MEMORY_POOL_ENABLE
	//tb_memory_dump();
	tb_memory_exit();
#endif
	
	// ok
	TB_DBG("exit: ok");
}

tb_char_t const* tb_version()
{
	static tb_char_t version_data[32] = {0};
	static tb_size_t version_size = 0;
	
	if (!version_size)
	{
		version_size = tb_snprintf(version_data, 32, "tbox-v%u.%u.%u", TB_VERSION_MAJOR, TB_VERSION_MINOR, TB_VERSION_ALTER);
		version_data[version_size] = '\0';
	}
	return version_data;
}

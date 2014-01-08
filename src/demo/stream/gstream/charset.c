/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_stream_gstream_charset_main(tb_int_t argc, tb_char_t** argv)
{
	// create stream
	tb_gstream_t* ist = tb_gstream_init_from_url(argv[1]);
	tb_gstream_t* ost = tb_gstream_init_from_url(argv[2]);
	tb_gstream_t* cst = tb_gstream_init_filter_from_charset(ist, tb_charset_type(argv[3]), tb_charset_type(argv[4]));
	if (!ist || !ost || !cst) goto end;

	// init option
	if (tb_gstream_type(ost) == TB_GSTREAM_TYPE_FILE)
		tb_gstream_ctrl(ost, TB_GSTREAM_CTRL_FILE_SET_MODE, TB_FILE_MODE_WO | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC);

	// open stream
	if (!tb_gstream_bopen(ist)) goto end;
	if (!tb_gstream_bopen(ost)) goto end;
	if (!tb_gstream_bopen(cst)) goto end;
	
#if 0
	// save stream
	tb_hize_t size = tb_gstream_save(cst, ost);
	tb_print("save: %llu bytes", size);
#else
	// read data
	tb_byte_t 		data[TB_GSTREAM_BLOCK_MAXN];
	tb_hize_t 		read = 0;
	tb_hize_t 		left = tb_gstream_left(cst);
	tb_hong_t 		base = tb_mclock();
	tb_hong_t 		basc = tb_mclock();
	do
	{
		// read data
		tb_long_t n = tb_gstream_aread(cst, data, TB_GSTREAM_BLOCK_MAXN);
//		tb_trace("read: %d, offset: %llu, left: %llu, size: %llu", n, tb_gstream_offset(ist), tb_gstream_left(ist), tb_gstream_size(ist));
		if (n > 0)
		{
			// writ data
			if (!tb_gstream_bwrit(ost, data, n)) break;

			// update read
			read += n;
		}
		else if (!n) 
		{
			// wait
			tb_long_t e = tb_gstream_wait(ist, TB_GSTREAM_WAIT_READ, tb_gstream_timeout(cst));
			tb_assert_and_check_break(e >= 0);

			// timeout?
			tb_check_break(e);

			// has read?
			tb_assert_and_check_break(e & TB_GSTREAM_WAIT_READ);
		}
		else break;

		// is end?
		if (left && read >= left) break;

		// print info
		if (tb_mclock() - basc > 5000) 
		{
			tb_print("[gst]: load: %llu bytes, speed: %llu bytes / s", tb_gstream_offset(ist), (tb_gstream_offset(ist) * 1000) / (tb_mclock() - base));
			basc = tb_mclock();
		}

	} while(1);

	tb_print("[gst]: load: %llu bytes, size: %llu bytes, time: %llu ms", read, tb_gstream_size(ist), tb_mclock() - base);
#endif


end:

	// exit stream
	tb_gstream_exit(cst);
	tb_gstream_exit(ist);
	tb_gstream_exit(ost);

	return 0;
}
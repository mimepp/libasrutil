#ifndef	__ASRUTIL_OSD_H__
#define	__ASRUTIL_OSD_H__

typedef enum {
	OSD_POS_BOTTOM = 0,
	OSD_POS_TOP,
} osd_pos_top_bottom;

int asrutil_osd_init();
int asrutil_osd_uninit();

// we could send msg to show on bottom or top. e.g.: Chinese on bottom, English on top;
int asrutil_osd_msg_send(char *utf8, int utf8_len, osd_pos_top_bottom top_bottom);

// when there is no new message, we need to clean the osd after several seconds, e.g.: 3 seconds;
int asrutil_osd_msg_clean(osd_pos_top_bottom top_bottom);

// set the x/y of bottom/top osd window
int asrutil_osd_set_pos(int x, int y, osd_pos_top_bottom top_bottom);
#endif

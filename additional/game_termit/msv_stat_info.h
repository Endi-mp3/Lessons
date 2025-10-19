#ifndef MSV_STAT_INFO_H
#define MSV_STAT_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

typedef const char* (*msv_stat_getter_fn)(void);

int msv_stat_init(int split_id);
int msv_stat_add(const char *title, msv_stat_getter_fn getter);
int msv_stat_draw(void);

int msv_stat_step(int split_id);
#ifdef __cplusplus
}
#endif

#endif


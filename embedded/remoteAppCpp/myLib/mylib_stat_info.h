#ifndef MSV_STAT_INFO_H
#define MSV_STAT_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

typedef const char* (*mylib_stat_getter_fn)(void);

int mylib_stat_init(int split_id);
int mylib_stat_add(const char *title, mylib_stat_getter_fn getter);
int mylib_stat_draw(void);
int mylib_stat_step(int split_id);
#ifdef __cplusplus
}
#endif

#endif


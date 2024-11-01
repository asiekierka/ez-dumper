#ifndef __MAIN_H__
#define __MAIN_H__

bool dir_exists(const char *path);
bool find_next_dir(char *buffer, const char *suffix);
void show_message(bool is_error, const char *text);

#endif /* __MAIN_H__ */

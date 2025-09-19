#ifndef __KEY_H__
#define __KEY_H__
#include <stm32f4xx.h>
void init_key(void);

u8 get_key_val_diff(void);

u8 get_key_val_same(u8 num);


#endif


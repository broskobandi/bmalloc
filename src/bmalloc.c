#include "bmalloc_utils.h"

_Thread_local static_buff_t g_static_buff;
_Thread_local sbrk_buff_t g_sbrk_buff;

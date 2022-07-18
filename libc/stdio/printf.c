#include <stdarg.h>
#include <stdio.h>

__attribute__ ((format (printf, 1, 2))) int printf(const char* format, ...) {
    va_list list;
    va_start(list, format);
    int ret = vprintf(format, list);
    va_end(list);
    return ret;
}
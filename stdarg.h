#ifndef _STDARG_H_
#define _STDARG_H_

#define va_start __builtin_va_start
#define va_copy __builtin_va_copy
#define va_arg __builtin_va_arg
#define va_end __builtin_va_end
#define va_list __builtin_va_list

#endif//_STDARG_H_

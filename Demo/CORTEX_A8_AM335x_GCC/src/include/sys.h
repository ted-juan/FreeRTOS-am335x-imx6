#ifndef __SYS_H_
#define __SYS_H_

#define printf sys_printf
extern int test_printf(void);
extern int sys_printf(const char *format, ...);

#endif

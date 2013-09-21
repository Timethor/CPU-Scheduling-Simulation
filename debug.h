/* 
 * File:   debug.h
 * Author: Timethor
 *
 * Created on September 21, 2013, 12:15 PM
 */

#ifndef DEBUG_H
#define	DEBUG_H

/*macros to enable debug print statements

call debug_printf(fmt,...,...,...); to print debug info
will only print  if 
#define DEBUG
is present
*/

#include <stdio.h>

#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif


#define debug_print(fmt, ...) \
        do { if (DEBUG_TEST) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); } while (0)

#define debug_print_string(fmt) \
        do { if (DEBUG_TEST) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__); } while (0)


#endif	/* DEBUG_H */


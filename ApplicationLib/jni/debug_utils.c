/*========================================================================
Copyright (c) 2010 by LG Cappuccino Inc.  All Rights Reserved.

			EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when		who			what, where, why
----------	----------	---------------------------------------------------
2010/10/11	kihoon.kim	cappuccino Initial release.
=========================================================================*/
#include <stdio.h>
#include <cutils/logd.h>
#include <cutils/log.h>

void __debug_trace(const char* catagory,const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
	__android_log_vprint(ANDROID_LOG_INFO,catagory,fmt,ap);

}

void debug_trace(const char* fmt,const char* file_name,const int line_num)
{    
	__debug_trace("WICA","[%s:%d]%s\n", file_name, line_num, fmt);
}

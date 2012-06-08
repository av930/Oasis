/*========================================================================
Copyright (c) 2010 by LG Cappuccino Inc.  All Rights Reserved.

			EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when		who			what, where, why
----------	----------	---------------------------------------------------
2010/10/11	kihoon.kim	cappuccino Initial release.
=========================================================================*/
#ifndef __DEBUG_UTILS_H__
#define __DEBUG_UTILS_H__ 
#ifdef __cplusplus
#define __C__ "C"
#else
#define __C__
#endif
	extern __C__ void __debug_trace(const char* catagory,const char* fmt, ...);
	extern __C__ void debug_trace(const char* fmt, const char* file_name, const int line_num);
#undef __C__

#define		CAPP_TRACE(MSG)        debug_trace(MSG,__FILE__,__LINE__);

#endif

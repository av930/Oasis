/*========================================================================
Copyright (c) 2010 by LG Cappuccino Inc.  All Rights Reserved.

			EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when		who			what, where, why
----------	----------	---------------------------------------------------
2010/09/13	kihoon.kim	cappuccino Initial release.
=========================================================================*/
package com.lge.util;

import com.lge.config.AppConfig;

import android.util.Log;

public class TimeMeasure {
    private long 	mStartTime;
    private long 	mStopTime;
    private boolean mProcessing;

    public TimeMeasure() {
        mStartTime 	= 0;
        mStopTime 	= 0;
        mProcessing = false;
    }

    public void start() {
        if ( mProcessing == true )
            return ;
        mStartTime = System.currentTimeMillis();
        mProcessing = true;
        if ( AppConfig.LOGD ) Log.i(AppConfig.TAG, "TimeMeasure Start!!! start time = " + mStartTime);
    }

    public void stop() {
        if ( mProcessing == false )
            return ;
        mStopTime = System.currentTimeMillis();
        mProcessing = false;
        if ( AppConfig.LOGD ) Log.i(AppConfig.TAG, "TimeMeasure Stop!!! start time = " + mStartTime + ", stop time = "+ mStopTime);
        if ( AppConfig.LOGD ) Log.i(AppConfig.TAG, "elapsed time = " + (mStopTime - mStartTime));
    }

    public void print() {
        if ( mProcessing == false )
            return ;

        long curTime = System.currentTimeMillis();
        if ( AppConfig.LOGD ) Log.i(AppConfig.TAG, "TimeMeasure print!!! elapsed time = " + (curTime - mStartTime));
    }
}

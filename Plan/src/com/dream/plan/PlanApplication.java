/*========================================================================
Copyright (c) 2010 by LG Cappuccino Inc.  All Rights Reserved.

			EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when		who			what, where, why
----------	----------	---------------------------------------------------
2012/06/08	kihoon.kim	cappuccino Initial release.
=========================================================================*/
package com.dream.plan;
import android.app.Application;
import android.util.Log;
import com.lge.config.AppConfig;

//------------------------------------------------------------------------
/// 최초 실행시, 초기화 루틴
//------------------------------------------------------------------------
public class PlanApplication extends Application {
    @Override
    public void onCreate() {
        if ( AppConfig.LOGD ) Log.d(AppConfig.TAG, "ActivityLifeCycle mAction = ");
        super.onCreate();
        AppConfig.init(getBaseContext());
    }
}

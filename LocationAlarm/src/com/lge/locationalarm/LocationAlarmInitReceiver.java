package com.lge.locationalarm;

import com.lge.config.AppConfig;

import android.content.Context;
import android.content.Intent;
import android.content.BroadcastReceiver;
import android.util.Log;

public class LocationAlarmInitReceiver extends BroadcastReceiver {

    @Override
    public void onReceive(Context context, Intent intent) {
        String action = intent.getAction();
        Log.d(AppConfig.TAG, "LocationAlarmInitReceiver" + action);

        if (action.equals(Intent.ACTION_BOOT_COMPLETED)) {
           
        }
    }
}

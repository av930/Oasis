package com.lge.generaltest;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

public class TickService_Svc extends Service {
    private final BroadcastReceiver mIntentReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Toast.makeText(getBaseContext(), "onReceive() "+intent.getAction(), Toast.LENGTH_SHORT).show();
            Log.d("kihoon.kim","onReceive() "+intent.getAction());
        }
    };
        
    @Override
    public void onCreate() {
        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_TIME_TICK);
        filter.addAction(Intent.ACTION_BATTERY_CHANGED);
        getBaseContext().registerReceiver(mIntentReceiver, filter, null, null);
        Toast.makeText(getBaseContext(), "onCreate() TIME_TICK service start", Toast.LENGTH_SHORT).show();
        super.onCreate();
    }

    @Override
    public void onDestroy() {
        Toast.makeText(getBaseContext(), "onDestroy() TIME_TICK service stop", Toast.LENGTH_SHORT).show();
        getBaseContext().unregisterReceiver(mIntentReceiver);
        super.onDestroy();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
}

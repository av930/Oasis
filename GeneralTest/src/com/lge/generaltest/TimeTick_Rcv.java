package com.lge.generaltest;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.widget.Toast;

public class TimeTick_Rcv extends BroadcastReceiver {
    @Override
    public void onReceive(Context context, Intent intent) {
        Toast.makeText(context , "Time Tick", Toast.LENGTH_SHORT).show();
    }
}

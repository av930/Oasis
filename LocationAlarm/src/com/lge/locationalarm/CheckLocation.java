package com.lge.locationalarm;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

public class CheckLocation extends Service {

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
}

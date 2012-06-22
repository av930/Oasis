package com.lge.generaltest;

import android.app.Activity;
import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class WakeupAlarm_Act extends Activity {
    private static final int 	TIME_MIN = 3;
    private Button 				mButton01;
    private Button 				mButton02;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.wakeupalarm_layout);

        mButton01 = (Button) findViewById(R.id.button01);
        mButton01.setText("set wake alarm every "+TIME_MIN);
        mButton01.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                AlarmManager am = (AlarmManager) getBaseContext().getSystemService(Context.ALARM_SERVICE);
                PendingIntent pi = PendingIntent.getActivity(WakeupAlarm_Act.this, 0, new Intent(WakeupAlarm_Act.this, CurrentTime_Act.class), 0);
                am.setRepeating(AlarmManager.RTC_WAKEUP, System.currentTimeMillis()+1000*60*TIME_MIN, 1000*60*TIME_MIN, pi);
//            	am.setRepeating(AlarmManager.ELAPSED_REALTIME_WAKEUP, 1000*TIME_MIN*60, 1000*TIME_MIN*60, pi);
                Toast.makeText(getBaseContext(), TIME_MIN+" min later..", Toast.LENGTH_LONG).show();
                Log.d("kihoon.kim","set wake up alarm....");
            }
        });

        mButton02 = (Button) findViewById(R.id.button02);
        mButton02.setText("not yet");
        mButton02.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
            }
        });
    }
}
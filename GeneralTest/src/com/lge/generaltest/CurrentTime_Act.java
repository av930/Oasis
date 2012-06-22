package com.lge.generaltest;

import android.app.Activity;
import android.os.Bundle;
import android.text.format.DateFormat;
import android.util.Log;
import android.widget.TextView;
import java.util.Calendar;
import java.util.TimeZone;

public class CurrentTime_Act extends Activity {
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.currenttime_layout);
        TextView tv = (TextView)findViewById(R.id.timetext);
        tv.setText("current time="+getTimeFormatString());
        Log.d("kihoon.kim","now wake up Alarm called");
        finish();
    }
    public String getTimeFormatString() {
        Calendar mCalendar;
        mCalendar = Calendar.getInstance(TimeZone.getDefault());
        String result2 = DateFormat.getTimeFormat(getBaseContext()).format(mCalendar.getTime());
        int indexStart = result2.indexOf(":") - 2;
        int indexEnd = result2.indexOf(":") + 3;

        if (indexStart < 0)
            indexStart = 0;
        if (indexEnd > result2.length())
            indexEnd = result2.length() - 1;
        String result = result2.substring(indexStart, indexEnd);
        return result;
    }
}
package com.lge.generaltest;

import com.lge.widget.control.DigitPicker;
import android.app.Activity;
import android.os.Bundle;

public class Roller_Act extends Activity {
    DigitPicker mMinutePicker;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.roller_layout);
        mMinutePicker = (DigitPicker) findViewById(R.id.minute);
    }
}
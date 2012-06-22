package com.lge.generaltest;

import android.app.Activity;
import android.os.Bundle;
import android.widget.Button;

public class ScrollableButton_Act extends Activity {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.scrollablebutton_layout);

        Button mButton01 = (Button)findViewById(R.id.button01);
        mButton01.setBackgroundResource(R.drawable.btn_bt);
        Button mButton02 = (Button)findViewById(R.id.button02);
        mButton02.setBackgroundResource(R.drawable.btn_gps);
        Button mButton03 = (Button)findViewById(R.id.button03);
        mButton03.setBackgroundResource(R.drawable.btn_mute);
        Button mButton04 = (Button)findViewById(R.id.button04);
        mButton04.setBackgroundResource(R.drawable.btn_plane);
        Button mButton05 = (Button)findViewById(R.id.button05);
        mButton05.setBackgroundResource(R.drawable.btn_rotate);
        Button mButton06 = (Button)findViewById(R.id.button06);
        mButton06.setBackgroundResource(R.drawable.btn_speaker);
        Button mButton07 = (Button)findViewById(R.id.button07);
        mButton07.setBackgroundResource(R.drawable.btn_vibrate);
        Button mButton08 = (Button)findViewById(R.id.button08);
        mButton08.setBackgroundResource(R.drawable.btn_wifi);
    }
}
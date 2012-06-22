package com.lge.generaltest;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class Audio_Act extends Activity {
    private Button 				mButton01;
    private Button 				mButton02;
    private Button 				mButton03;
    private Button 				mButton04;
    private Button 				mButton05;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.audio_layout);

        mButton01 = (Button) findViewById(R.id.button01);
        mButton01.setText("permanent");
        mButton01.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
            }
        });

        mButton02 = (Button) findViewById(R.id.button02);
        mButton02.setText("transient");
        mButton02.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
            }
        });
        mButton03 = (Button) findViewById(R.id.button03);
        mButton03.setText("transient with ducking");
        mButton03.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
            }
        });

        mButton04 = (Button) findViewById(R.id.button04);
        mButton04.setText("");
        mButton04.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
            }
        });
        mButton05 = (Button) findViewById(R.id.button05);
        mButton05.setText("");
        mButton05.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
            }
        });
    }
}
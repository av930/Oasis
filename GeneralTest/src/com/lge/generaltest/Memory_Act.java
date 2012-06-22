package com.lge.generaltest;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class Memory_Act extends Activity {
    private Button 				mButton01;
    private Button 				mButton02;
    private Button 				mButton03;
    private int[] array = new int[100];

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.memory_layout);

        mButton01 = (Button) findViewById(R.id.button01);
        mButton01.setText("array overflow java");
        mButton01.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {

                for (int i=0;i<101;i++) {
                    array[i] = i;
                }
            }
        });

        mButton02 = (Button) findViewById(R.id.button02);
        mButton02.setText("break memory");
        mButton02.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                MemoryNative.test1();
            }
        });

        mButton03 = (Button) findViewById(R.id.button03);
        mButton03.setText("break stack address");
        mButton03.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                MemoryNative.test2();
            }
        });
    }
}
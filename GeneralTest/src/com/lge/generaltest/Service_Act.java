package com.lge.generaltest;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class Service_Act extends Activity {
    private Button 		mButton01;
    private Button 		mButton02;
    private Button 		mButton03;
    private Button 		mButton04;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.service_layout);
        mButton01 = (Button) findViewById(R.id.button01);
        mButton02 = (Button) findViewById(R.id.button02);
        mButton03 = (Button) findViewById(R.id.button03);
        mButton04 = (Button) findViewById(R.id.button04);

        mButton01.setText("TickService Start");
        mButton01.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Intent intent = new Intent(Service_Act.this, TickService_Svc.class);
                startService(intent);
            }
        });

        mButton02.setText("TickService Stop");
        mButton02.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Intent i=new Intent(getBaseContext(),TickService_Svc.class);
                stopService(i);
            }
        });

        mButton03.setText("Receiver Leak Start");
        mButton03.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Intent intent = new Intent(Service_Act.this, ReceiverLeakTest_Svc.class);
                startService(intent);
            }
        });

        mButton04.setText("Receiver Leak Stop");
        mButton04.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Intent i=new Intent(getBaseContext(),ReceiverLeakTest_Svc.class);
                stopService(i);
            }
        });
    }
}
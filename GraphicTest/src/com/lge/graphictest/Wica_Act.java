package com.lge.graphictest;

import com.lge.view.WicaView;

import android.app.Activity;
import android.os.Bundle;

public class Wica_Act extends Activity {
    WicaView wicaView1;
    WicaView wicaView2;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.wica_layout);

        wicaView1 = (WicaView) findViewById(R.id.wicaview1);
        wicaView1.load("/system/etc/aa.wca");
        wicaView2 = (WicaView) findViewById(R.id.wicaview2);
        wicaView2.load("/system/etc/bb.wca");
    }
}
package com.lge.generaltest;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class SignalClusterView_Act extends Activity {
    private Button 				mButton01;
    private Button 				mButton02;
    private SignalClusterView	mSignalCluster1;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.signalcluster_layout);

        mSignalCluster1 = (SignalClusterView) findViewById(R.id.signal_cluster);

        mSignalCluster1.setWifiIndicators(true,R.drawable.stat_sys_wifi_signal_1_fully,R.drawable.stat_sys_wifi_out,"wifi");
        mSignalCluster1.setMobileDataIndicators(true,R.drawable.stat_sys_signal_1_fully,R.drawable.stat_sys_signal_in,R.drawable.stat_sys_data_fully_connected_4g, "test", "data2");
        mButton01 = (Button) findViewById(R.id.button01);
        mButton01.setText("not yet");
        mButton01.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {

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
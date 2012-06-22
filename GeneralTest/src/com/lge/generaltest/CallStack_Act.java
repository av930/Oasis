package com.lge.generaltest;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import com.lge.config.AppConfig;
import android.widget.Toast;
import android.util.Log;
import java.io.PrintWriter;
import java.io.StringWriter;

public class CallStack_Act extends Activity {
    private Button 				mButton01;
    private Button 				mButton02;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.callstack_layout);

        mButton01 = (Button) findViewById(R.id.button01);
        mButton01.setText("solution 01.");
        mButton01.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Throwable tr = new Throwable();
                StringWriter sw = new StringWriter();
                tr.printStackTrace(new PrintWriter(sw));
                StringBuilder sb = new StringBuilder(sw.toString());
                Toast.makeText(getBaseContext(),sb , Toast.LENGTH_LONG).show();
                Log.d(AppConfig.TAG, sb.toString());
            }
        });

        mButton02 = (Button) findViewById(R.id.button02);
        mButton02.setText("solution 02.");
        mButton02.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                RuntimeException re = new RuntimeException();
                re.printStackTrace();
                Toast.makeText(getBaseContext(),"check it in logcat" , Toast.LENGTH_LONG).show();
            }
        });
    }
}
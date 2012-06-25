package com.lge.generaltest;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import com.lge.widget.CustomView;
public class CustomView_Act extends Activity {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.customview_layout);
        final CustomView view = (CustomView)findViewById(R.id.customview);
        Button button01 = (Button)findViewById(R.id.button01);
        button01.setText("Angle");
        button01.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                view.mAngle += 10;
                view.invalidate();
            }
        });
        Button button02 = (Button)findViewById(R.id.button02);
        button02.setText("x");
        button02.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                view.mX += 10;
                view.invalidate();
            }
        });
        Button button03 = (Button)findViewById(R.id.button03);
        button03.setText("y");
        button03.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                view.mY += 10;
                view.invalidate();
            }
        });
    }
}
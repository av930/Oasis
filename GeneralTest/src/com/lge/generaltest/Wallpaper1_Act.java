package com.lge.generaltest;

import java.io.IOException;

import android.app.Activity;
import android.app.WallpaperManager;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class Wallpaper1_Act extends Activity {
    private Button 				mButton01;
    private Button 				mButton02;
    private Button 				mButton03;
    private Button 				mButton04;
    private Button 				mButton05;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.template_layout);

        final WallpaperManager wallpaperManager = WallpaperManager.getInstance(this);

        mButton01 = (Button) findViewById(R.id.button01);
        mButton01.setText("wallpaper1");
        mButton01.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                try {
                    wallpaperManager.setResource(R.raw.wallpaper1);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });

        mButton02 = (Button) findViewById(R.id.button02);
        mButton02.setText("wallpaper2");
        mButton02.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                try {
                    wallpaperManager.setResource(R.raw.wallpaper2);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });
    }
}
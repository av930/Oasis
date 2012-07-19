package com.lge.generaltest;

import java.io.IOException;

import android.app.Activity;
import android.app.WallpaperManager;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;

public class Wallpaper_Act extends Activity {
    private WallpaperManager 	mWallpaperManager = null;
    private Button 				mButton01;
    private Button 				mButton02;
    private int					mOffsetX;
    private FrameLayout 		mFrame;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.wallpaper_layout);
        mFrame = (FrameLayout) findViewById(R.id.total);
        
        mWallpaperManager = WallpaperManager.getInstance(this);
        mOffsetX = 0;
		mWallpaperManager.setWallpaperOffsetSteps((float) 0.1666, 0 );
		
        mButton01 = (Button) findViewById(R.id.button01);
        mButton01.setText("wallpaper1");
        mButton01.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                try {
                	mWallpaperManager.setResource(R.raw.wallpaper1);
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
                	mWallpaperManager.setResource(R.raw.wallpaper2);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });
    }

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		mOffsetX = (int) event.getX();
		if( mWallpaperManager != null ) {
			mWallpaperManager.setWallpaperOffsets(mFrame.getWindowToken(), (float)((float)(720 - mOffsetX)/(float)(520))-(float)0.2, 0 );
		}
		return super.onTouchEvent(event);
	}
}
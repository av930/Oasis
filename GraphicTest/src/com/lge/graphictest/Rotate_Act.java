package com.lge.graphictest;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.widget.ImageView;
import android.widget.ScrollView;

public class Rotate_Act extends Activity {
    private BitmapDrawable[] 	mDrawable;
    private ImageView 			mImage02;
    private int 				mCount = 0;
    private final int 			TOTAL = 10;
    private Bitmap[] 			mBmp;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.rotate_layout);

        ScrollView scroll;
        Bitmap origin01;
        int w,h;

        mImage02 = (ImageView) findViewById(R.id.ImageView02);
        origin01 = BitmapFactory.decodeResource(getResources(),R.drawable.album_view_image);
        w = origin01.getWidth();
        h = origin01.getHeight();

        mBmp = new Bitmap[TOTAL];
        mDrawable = new BitmapDrawable[TOTAL];
        for (int i=0;i<TOTAL;i++) {
            Paint paint = new Paint();
            mBmp[i] = Bitmap.createBitmap(w+50, h+50, Config.ARGB_8888);
            Canvas canvas = new Canvas(mBmp[i]);
            canvas.rotate((i*2), (w+50)/2,(h+50)/2);
            canvas.drawBitmap(origin01, 0, 0, paint);
            mDrawable[i] = new BitmapDrawable(mBmp[i]);
        }
        scroll = (ScrollView) findViewById(R.id.ScrollView01);
        scroll.setOnTouchListener(new View.OnTouchListener() {
            public boolean onTouch(View v, MotionEvent event) {
                mCount++;
                mImage02.setImageDrawable(mDrawable[mCount%TOTAL]);
                return false;
            }
        });
    }

    @Override
    protected void onDestroy() {
        // TODO Auto-generated method stub
        super.onDestroy();
        for (int i=0;i<TOTAL;i++) {
            mBmp[i].recycle();
        }
        System.gc();
    }
}
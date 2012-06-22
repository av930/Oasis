package com.lge.graphictest;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;

public class Reflect_Act extends Activity {
    private ReflectView 	mReflectView;
    private BitmapDrawable 	mDrawable;
    Bitmap origin01;

//  mTetrisView = (TetrisView) findViewById(R.id.game);
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.reflect_layout);
        mDrawable = new BitmapDrawable();
        origin01 = BitmapFactory.decodeResource(getResources(),R.drawable.album_view_image);
        mDrawable = new BitmapDrawable(origin01);

        mReflectView =  (ReflectView) findViewById(R.id.reflectimageview);
        mReflectView.setImageDrawable(mDrawable);
    }
}
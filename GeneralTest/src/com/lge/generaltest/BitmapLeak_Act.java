package com.lge.generaltest;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.lge.generaltest.R;

public class BitmapLeak_Act extends Activity {
    private static final int		MAX_BITMAP = 10000;
    private static BitmapDrawable 	mDrawable[];
    private static Bitmap  			mBitmap;
    private static int 				mCount = 0;
    private Button 					mButton01;

    static {
        mDrawable = new BitmapDrawable[MAX_BITMAP];
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.bitmapleak_layout);
        mButton01 = (Button) findViewById(R.id.button01);
        mButton01.setText("Decode Bitmap");
        mButton01.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                mDrawable[mCount] = new BitmapDrawable();
                mBitmap = BitmapFactory.decodeResource(getResources(),R.drawable.album_view_image);
                mDrawable[mCount] = new BitmapDrawable(mBitmap);
                mCount++;
                Toast.makeText(getBaseContext(), "Count="+mCount, Toast.LENGTH_SHORT).show();
            }
        });
    }
}
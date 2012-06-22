/*========================================================================
Copyright (c) 2010 by LG Cappuccino Inc.  All Rights Reserved.

			EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when		who			what, where, why
----------	----------	---------------------------------------------------
2010/04/29	kihoon.kim	cappuccino Initial release.
=========================================================================*/
package com.lge.graphictest;

import com.lge.config.AppConfig;
import com.lge.view.RotateLayout;
import com.lge.view.OnChangeIndexListener;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

public class WidgetPreview2_Act extends Activity implements View.OnClickListener, OnChangeIndexListener {
    private RotateLayout 	mRotateLayout;
    private ImageView 		mImageView01;
    private ImageView 		mImageView02;
    private ImageView 		mImageView03;
    private BitmapDrawable 	mDrawable01;
    private BitmapDrawable 	mDrawable02;
    private BitmapDrawable 	mDrawable03;
    private Bitmap 			mBitmap01;
    private Bitmap 			mBitmap02;
    private Bitmap 			mBitmap03;
    private TextView		mWidgetTitle;
    private String 			mWidgetName;
    private int 			mPreviewNum = 3;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.widgetpreview2_layout);
        Intent intent = getIntent(); ;
        mWidgetName 	= intent.getStringExtra("widgetName");

        mBitmap01 = BitmapFactory.decodeResource(getResources(),R.drawable.preview01);
        mDrawable01 = new BitmapDrawable(mBitmap01);
        mImageView01 =  (ImageView) findViewById(R.id.imageview01);
        mImageView01.setImageDrawable(mDrawable01);
        mImageView01.layout(0,0,mBitmap01.getWidth(), mBitmap01.getHeight());

        mBitmap02 = BitmapFactory.decodeResource(getResources(),R.drawable.preview02);
        mDrawable02 = new BitmapDrawable(mBitmap02);
        mImageView02 =  (ImageView) findViewById(R.id.imageview02);
        mImageView02.setImageDrawable(mDrawable02);
        mImageView02.layout(0,0,mBitmap02.getWidth(), mBitmap02.getHeight());

        mBitmap03 = BitmapFactory.decodeResource(getResources(),R.drawable.preview03);
        mDrawable03 = new BitmapDrawable(mBitmap03);
        mImageView03 =  (ImageView) findViewById(R.id.imageview03);
        mImageView03.setImageDrawable(mDrawable03);
        mImageView03.layout(0,0,mBitmap03.getWidth(), mBitmap03.getHeight());

        mWidgetTitle = (TextView)findViewById(R.id.widgettitle);

        mRotateLayout = (RotateLayout)findViewById(R.id.rotatelayout);
        mRotateLayout.setOnePageWidth((int)(AppConfig.mShortAxis * 0.75));
        mRotateLayout.setChildNum(mPreviewNum);

        mRotateLayout.setOnItemClickListener(this);
        mRotateLayout.setOnChangeIndexListener(this);
        mWidgetTitle.setText(mWidgetName+"\n"+(mRotateLayout.getSelIndex()+1)+" / "+mPreviewNum);
    }

    public void onClick(View view) {
        Intent data = new Intent();
        data.putExtra("SelIndex",mRotateLayout.getSelIndex());
        setResult(RESULT_OK, data);
        finish();
    }
    public void onChangeIndex() {
        mWidgetTitle.setText(mWidgetName+"\n"+(mRotateLayout.getSelIndex()+1)+"/"+mPreviewNum);
    }
}



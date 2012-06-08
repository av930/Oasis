/*========================================================================
Copyright (c) 2010 by LG Cappuccino Inc.  All Rights Reserved.

			EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when		who			what, where, why
----------	----------	---------------------------------------------------
2009/12/14	kihoon.kim	cappuccino Initial release.
=========================================================================*/
package com.lge.view;

import android.os.Handler;
import android.os.Message;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import android.graphics.Color;

import com.lge.config.AppConfig;
import com.lge.wica.Wica;

//------------------------------------------------------------------------
/// BoardView
//------------------------------------------------------------------------
public class WicaView extends View {
    private Bitmap mBmp;
    private AnimationHandler 	mAnimationHandler 	= new AnimationHandler();
    private int mCount = 0;
    private Wica mWica;

    //------------------------------------------------------------------------
    ///
    //------------------------------------------------------------------------
    class AnimationHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            WicaView.this.update();
            WicaView.this.invalidate();
        }

        public void sleep(long delayMillis) {
            this.removeMessages(0);
            sendMessageDelayed(obtainMessage(0), delayMillis);
        }
    };

    public void update() {
        if ( AppConfig.LOGD ) Log.i("", "update Count= "+mCount);
        if (mBmp!=null) {
            mBmp.recycle();
            System.gc();
        }
        mBmp= Bitmap.createBitmap(mWica.decode(mCount), mWica.getWidth(), mWica.getHeight(), Bitmap.Config.ARGB_8888);
        mCount++;
        if (mCount>mWica.getFrameNum())
            mCount = 0;
        mAnimationHandler.sleep(50);
    }

    public WicaView(Context context) {
        super(context);
        Init();
    }

    public WicaView(Context context, AttributeSet attrs) {
        super(context, attrs);
        Init();
    }
    public void Init() {
        mWica = new Wica();
    }
    public void load(String fileName) {
        mWica.create(fileName);
        update();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        Paint paint = new Paint();
        paint.setColor(Color.WHITE);
        paint.setStrokeWidth(0);
        canvas.drawBitmap(mBmp, 0, 0, paint);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        setMeasuredDimension(mWica.getWidth(), mWica.getHeight());
    }
}



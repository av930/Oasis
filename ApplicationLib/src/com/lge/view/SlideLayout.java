/*========================================================================
Copyright (c) 2010 by LG Cappuccino Inc.  All Rights Reserved.

			EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when		who			what, where, why
----------	----------	---------------------------------------------------
2010/09/13	kihoon.kim	cappuccino Initial release.
=========================================================================*/
package com.lge.view;


import com.lge.config.AppConfig;

import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;

//------------------------------------------------------------------------
/// SlideLayout
//------------------------------------------------------------------------
public class SlideLayout extends ViewGroup {
    //-------------------------------------------------------------------------
    // Member Variables
    private int 			mCurMoveDistance; 			// Pointing event 있을때 잠시 사용하는 변수
    private boolean 		mPointingDown 		= false;
    private int 			mOnePageWidth;
    private int	 			mCurOffset;
    private AnimateHandler 	mAnimateHandler 	= new AnimateHandler();

    //------------------------------------------------------------------------
    ///
    //------------------------------------------------------------------------
    class AnimateHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            int modular = mCurOffset % mOnePageWidth;
            if ( modular == 0 || mPointingDown == true )
                return ;
            SlideLayout.this.autoScrolling();
        }

        public void sleep(long delayMillis) {
            this.removeMessages(0);
            sendMessageDelayed(obtainMessage(0), delayMillis);
        }
    };

    //------------------------------------------------------------------------
    // Member Functions
    public SlideLayout(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public SlideLayout(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        initWorkspace();
    }

    private void initWorkspace() {
        mCurOffset = 0;
    }

    public void setOnePageWidth(int width) {
        mOnePageWidth = width;
    }

    @Override
    public void addView(View child, int index, LayoutParams params) {
        super.addView(child, index, params);
    }

    @Override
    public void addView(View child) {
        super.addView(child);
    }

    @Override
    public void addView(View child, int index) {
        super.addView(child, index);
    }

    @Override
    public void addView(View child, int width, int height) {
        super.addView(child, width, height);
    }

    @Override
    public void addView(View child, LayoutParams params) {
        super.addView(child, params);
    }

    @Override
    protected void onLayout(boolean changed,
                            int l, int t, int r, int b) {
        final int count = getChildCount();
        for ( int i = 0; i < count; i++ ) {
            final View child = getChildAt(i);
            int left = i * child.getWidth() - mCurOffset;
            int right = i * child.getWidth() + r - mCurOffset;
            if ( left > mOnePageWidth ) {
                child.layout(left - mOnePageWidth, t, right - mOnePageWidth, b);
                if ( AppConfig.LOGD ) Log.i("", "i = "+i+", x1 = "+(left - mOnePageWidth)+", y1 = "+t+" ,x2 = "+(right - mOnePageWidth)+", y2 = "+b);
            }
            if ( left <= mOnePageWidth ) {
                child.layout(left, t, right, b);
                if ( AppConfig.LOGD ) Log.i("", "i = "+i+", x1 = "+left+", y1 = "+t+" ,x2 = "+right+", y2 = "+b);
            }
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent ev) {
        if ( ev.getAction() == MotionEvent.ACTION_DOWN ) {
            mPointingDown = true;
            mCurMoveDistance = mCurOffset + (int)ev.getX();
        } else if ( ev.getAction() == MotionEvent.ACTION_UP ) {
            mPointingDown = false;
            autoScrolling();
        } else if ( ev.getAction() == MotionEvent.ACTION_MOVE && mPointingDown == true ) {
            mCurOffset = mCurMoveDistance - (int)ev.getX();
            if ( mCurOffset < 0 )
                mCurOffset = 0;
            if ( mCurOffset > mOnePageWidth )
                mCurOffset = mOnePageWidth;

            requestLayout();
            invalidate();
        }
        if ( AppConfig.LOGD ) Log.i(AppConfig.TAG,"onTouchEvent===="+mCurOffset);
        return true;
    }

    public void autoScrolling() {
        int modular = mCurOffset % mOnePageWidth;
        if ( modular != 0 ) {
            int half = mOnePageWidth / 2;
            if ( modular >5 && modular <mOnePageWidth - 5 ) {
                if ( modular > half )
                    mCurOffset += (mOnePageWidth - modular) * 0.6;
                else
                    mCurOffset -= modular * 0.6;
            } else if ( modular <= 5 ) {
                mCurOffset -= modular;
            } else if ( modular >= mOnePageWidth - 5 ) {
                mCurOffset += (mOnePageWidth - modular);
            }

            if ( AppConfig.LOGD ) Log.i(AppConfig.TAG,"autoScrolling==mCurOffset=="+mCurOffset);
            if ( AppConfig.LOGD ) Log.i(AppConfig.TAG,"autoScrolling==modular=="+modular);
            requestLayout();
            invalidate();
            mAnimateHandler.sleep(100);
        }
    }
}

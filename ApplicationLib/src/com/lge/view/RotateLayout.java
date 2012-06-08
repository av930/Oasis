/*========================================================================
Copyright (c) 2010 by LG Cappuccino Inc.  All Rights Reserved.

			EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when		who			what, where, why
----------	----------	---------------------------------------------------
2010/11/02	kihoon.kim	cappuccino Initial release.
=========================================================================*/
package com.lge.view;

import com.lge.config.AppConfig;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Point;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;

//------------------------------------------------------------------------
/// RotateLayout
//------------------------------------------------------------------------

public class RotateLayout extends ViewGroup {
    private final int				mSensitivity		= 20;
    private int 					mCurMoveDistance; 			// Pointing event 있을때 잠시 사용하는 변수
    private boolean 				mPointingDown 		= false;
    private int 					mOnePageWidth;
    private int	 					mCurOffset;
    private int						mChildNum;
    private Point 					mCurPoint 			= new Point(0,0);
    private boolean					mPressed;
    private OnClickListener 		mClickListener;
    private OnChangeIndexListener 	mChangeIndexListener;
    private int						mCurSelIndex;
    private int						mOldSelIndex;

    private AnimateHandler 	mAnimateHandler 	= new AnimateHandler();

    class AnimateHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            int modular = mCurOffset % mOnePageWidth;
            if ( modular == 0 || mPointingDown == true )
                return ;
            RotateLayout.this.autoScrolling();
        }

        public void sleep(long delayMillis) {
            this.removeMessages(0);
            sendMessageDelayed(obtainMessage(0), delayMillis);
        }
    };

    //------------------------------------------------------------------------
    // Member Functions
    public RotateLayout(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public RotateLayout(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        initWorkspace();
    }

    private void initWorkspace() {
        mCurOffset 	= 0;
        mChildNum = 0;
    }

    public void setOnePageWidth(int width) {
        mOnePageWidth = width;
    }

    public void setChildNum(int childNum) {
        mChildNum = childNum;
    }

    @Override
    public void addView(View child, int index, LayoutParams params) {
        super.addView(child, index, params);
    }

    @Override
    public void addView(View child, LayoutParams params) {
        super.addView(child, params);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        // TODO Auto-generated method stub
        super.onDraw(canvas);
        Paint   paintPen 	= new Paint();
        paintPen.setColor(Color.WHITE);
        paintPen.setStrokeWidth(1);

        canvas.drawText("HasdfaasdfsafsdI", 10, 10, paintPen );
    }

    @Override
    protected void onLayout(boolean changed,
                            int l, int t, int r, int b) {
        final int count = mChildNum;
        if ( AppConfig.LOGD ) Log.d(AppConfig.TAG, "onLayout() mChildNum = "+mChildNum);

        for ( int i = 0; i < count; i++ ) {
            final View child = getChildAt(i);
            int top = getTop(child, i);
            int left = getLeft(child, i);
            int right = left + child.getWidth();
            child.layout(left, top, right, top+child.getHeight());

            if ( AppConfig.LOGD ) Log.d(AppConfig.TAG, "onLayout() i = "+i+"width = "+ child.getWidth()+" height = "+child.getHeight());
        }
    }

    private int	getTop(View child, int index) {
        return AppConfig.mLongAxis - child.getHeight() - (int)(AppConfig.mLongAxis * 0.2) - ((index * mOnePageWidth - mCurOffset) * (index * mOnePageWidth - mCurOffset) / (AppConfig.mLongAxis*2));
    }

    private int getLeft(View child, int index) {
        return index * mOnePageWidth - mCurOffset + (AppConfig.mShortAxis - child.getWidth())/2;
    }

    private boolean ptInRect(int index, Point point) {
        final View child = getChildAt(index);
        int top = getTop(child, index);
        int bottom = top + child.getHeight();
        int left = getLeft(child, index);
        int right = left + child.getWidth();
        if (		top <= point.y &&
                point.y <= bottom &&
                left <= point.x &&
                point.x <= right )
            return true;

        return false;
    }


    @Override
    public boolean onTouchEvent(MotionEvent ev) {
        if ( ev.getAction() == MotionEvent.ACTION_DOWN ) {
            mCurPoint.x = (int)ev.getX();
            mCurPoint.y = (int)ev.getY();
            if (ptInRect(mCurSelIndex, mCurPoint))
                mPressed = true;
            else
                mPressed = false;

            mPointingDown = true;
            if ( AppConfig.LOGD ) Log.d(AppConfig.TAG, "mPressed = "+mPressed);
            mCurMoveDistance = mCurOffset + (int)ev.getX();
        } else if ( ev.getAction() == MotionEvent.ACTION_UP ) {
            if ( AppConfig.LOGD ) Log.d(AppConfig.TAG, "mPressed = "+mPressed);
            if (		mPressed &&
                    ((mCurPoint.x - (int)ev.getX()) < mSensitivity ) &&
                    ((mCurPoint.x - (int)ev.getX()) > -mSensitivity )&&
                    ((mCurPoint.y - (int)ev.getY()) < mSensitivity ) &&
                    ((mCurPoint.y - (int)ev.getY()) > -mSensitivity )	) {
                if ( AppConfig.LOGD ) Log.d(AppConfig.TAG, "==========Click");
                mClickListener.onClick(null);
            }
            mPointingDown = false;
            autoScrolling();

        } else if ( ev.getAction() == MotionEvent.ACTION_MOVE && mPointingDown == true ) {
            mCurOffset = mCurMoveDistance - (int)ev.getX();
            if ( mCurOffset < 0 )
                mCurOffset = 0;
            if ( mCurOffset > mOnePageWidth * (mChildNum-1) )
                mCurOffset = mOnePageWidth * (mChildNum-1);

            CalcSelIndex();

            if (mChangeIndexListener != null && mOldSelIndex != mCurSelIndex) {
                mChangeIndexListener.onChangeIndex();
            }
            mOldSelIndex = mCurSelIndex;
            requestLayout();
            invalidate();
        }
        if ( AppConfig.LOGD ) Log.d(AppConfig.TAG, "onTouchEvent() mCurOffset = "+mCurOffset+" mChildNum = "+ mChildNum);

        return true;
    }

    public void setOnItemClickListener(OnClickListener c) {
        mClickListener = c;
    }

    public void setOnChangeIndexListener(OnChangeIndexListener c) {
        mChangeIndexListener = c;
    }

    public void autoScrolling() {
        int modular = mCurOffset % mOnePageWidth;
        if ( modular != 0 ) {
            if (mCurOffset < 0) {
                if ( 5 < modular )
                    mCurOffset += (mOnePageWidth - modular) * 0.7;
                else if ( 0 < modular && modular <= 5 )
                    mCurOffset += modular;
                else if ( modular <= 0)
                    mCurOffset -= modular;
            } else if ( 0 < mCurOffset && mCurOffset < (mChildNum - 1) * mOnePageWidth   ) {
                int half = mOnePageWidth / 2;
                if ( modular >5 && modular <mOnePageWidth - 5 ) {
                    if ( modular > half )
                        mCurOffset += (mOnePageWidth - modular) * 0.7;
                    else
                        mCurOffset -= modular * 0.7;
                } else if ( modular <= 5 ) {
                    mCurOffset -= modular;
                } else if ( modular >= mOnePageWidth - 5 ) {
                    mCurOffset += (mOnePageWidth - modular);
                }
            } else if ((mChildNum - 1) * mOnePageWidth < mCurOffset)	{
                if ( modular > 5 )
                    mCurOffset -= (mOnePageWidth - modular) * 0.7;
                else if ( 0 < modular && modular <= 5 )
                    mCurOffset -= modular;
                else if ( modular < 0 )
                    mCurOffset += modular;
            }
            if ( AppConfig.LOGD ) Log.d(AppConfig.TAG, "autoScrolling() mCurOffset = "+mCurOffset+ ", modular = "+modular);

            requestLayout();
            invalidate();
            mAnimateHandler.sleep(100);
        }
    }

    private void CalcSelIndex() {
        mCurSelIndex = (mCurOffset + mOnePageWidth/2) / mOnePageWidth;
    }

    public int getSelIndex() {
        return mCurSelIndex;
    }
}

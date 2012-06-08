/*========================================================================
Copyright (c) 2010 by LG Cappuccino Inc.  All Rights Reserved.

			EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when		who			what, where, why
----------	----------	---------------------------------------------------
2010/12/18	kihoon.kim	cappuccino Initial release.
=========================================================================*/

package com.lge.view;

import com.lge.config.AppConfig;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

//------------------------------------------------------------------------
/// LEDView
//------------------------------------------------------------------------
public class LEDView extends View {
    public static final int	LED_MINSIZE			= 10;
    public static final int	LED_MAXSIZE			= 30;
    private Bitmap 			mTextBitmap;
    private Canvas 			mTextCanvas;
    private	int				mFontSize;
    private String 			mScreenText;
    private int				mSlidingPos;
    private int				mScreenTextWidth;

    private	int				mCellNumWidth;
    private	int				mCellNumHeight;
    private	int				mCellWidth;
    private	int				mCellHeight;

    private SlidingTimer	mSlidingTimer 		= new SlidingTimer();

    // TextCanvas 에서 사용하는 Paint
    private Paint 			mBGPaint 			= new Paint();
    private Paint 			mTextPaint 			= new Paint();

    // LED 를 그릴때 사용하는 Paint
    private Paint 			mLightPaint 		= new Paint();
    private Paint 			mShadowPaint 		= new Paint();
    private Paint 			mLinePaint 			= new Paint();
    private Paint 			mCurPaint 			= new Paint();

    private boolean 		mTimerOn 			= true;

    private SeekBarAreaTouchListener mSeekBarAreaTouchListener;

    public LEDView(Context context, AttributeSet attrs) {
        super(context, attrs);
        Init();
    }

    public LEDView(Context context) {
        super(context);
        Init();
    }

    public void Init() {
        setLEDSize(LED_MINSIZE);

        mTextBitmap = Bitmap.createBitmap(mCellNumWidth, mCellNumHeight, Config.ARGB_8888);
        mTextCanvas = new Canvas(mTextBitmap);

        mBGPaint.setColor(Color.BLACK);

        mTextPaint.setColor(Color.YELLOW);

        mLightPaint.setColor(Color.WHITE);
        mLightPaint.setStyle(Paint.Style.STROKE);
        mLightPaint.setStrokeWidth(1);

        mShadowPaint.setColor(Color.BLACK);
        mShadowPaint.setStyle(Paint.Style.STROKE);
        mShadowPaint.setStrokeWidth(1);

        mLinePaint.setColor(Color.GRAY);
        mLinePaint.setStyle(Paint.Style.STROKE);
        mLinePaint.setStrokeWidth(1);

        setText("");
        update();
    }

    public void setSeekBarAreaTouchListener(SeekBarAreaTouchListener l) {
        mSeekBarAreaTouchListener = l;
    }

    public void pause() {
        mTimerOn = false;
    }

    public void resume() {
        mTimerOn = true;
        update();
    }

    public void setText(String str) {
        mSlidingPos 		= 0;
        mScreenText 		= str;
        mScreenTextWidth 	= (int)mTextPaint.measureText(mScreenText, 0, mScreenText.length());
    }

    public String getText() {
        return mScreenText;
    }

    public void setLEDSize(int size) {
        if ( size < LED_MINSIZE )
            size = LED_MINSIZE;
        else if (size > LED_MAXSIZE)
            size = LED_MAXSIZE;

        mCellHeight 	= size;
        mCellWidth 		= size;

        mCellNumWidth 	= AppConfig.mWidth / mCellWidth;
        mCellNumHeight 	= AppConfig.mHeight / mCellHeight;
        mFontSize		= mCellNumHeight;
        mTextPaint.setTextSize(mFontSize - 1);
        if (mScreenText!=null) {
            mScreenTextWidth = (int)mTextPaint.measureText(mScreenText, 0, mScreenText.length());
        }
    }

    public int getLEDSize() {
        return mCellHeight;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        int x,y;
        for ( y = 0 ; y < mCellNumHeight ; y++) {
            for ( x = 0 ; x < mCellNumWidth ; x++) {
                int color = mTextBitmap.getPixel(x, y);
                drawLED(canvas, x, y, color);
            }
        }
        // 디버깅을 위한 코드
//		Rect mTotalRect = new Rect(0,0,100,100);
//		canvas.drawBitmap(mTextBitmap, mTotalRect, mTotalRect, mBGPaint);
    }

    private void drawLED(Canvas canvas, int x, int y, int color) {
        RectF rect = new RectF( x * mCellWidth, y * mCellHeight,
                                (x + 1) * mCellWidth , (y + 1) * mCellHeight );
        rect.inset(1,1);
        mCurPaint.setColor(color);
        canvas.drawOval(rect, mCurPaint);

        canvas.drawArc(rect, 360 - 30, 150, false, mLightPaint);
        canvas.drawArc(rect, 180 - 60, 210, false, mLinePaint);
        rect.inset(2,2);
        canvas.drawArc(rect, 180 + 20, 50, false, mLightPaint);
    }

    private void update() {
        mSlidingPos ++;
        if (mScreenTextWidth <= mSlidingPos)
            mSlidingPos = 0;
        mTextCanvas.drawRect(0, 0, mCellNumWidth, mCellNumHeight, mBGPaint);
//    	mTextCanvas.drawText(mScreenText, 0 - mSlidingPos, mCellNumHeight-3, mTextPaint);
        mTextCanvas.drawText(mScreenText, 0 - mSlidingPos, mCellNumHeight-mCellNumHeight/6, mTextPaint);
        mSlidingTimer.sleep(100);
    }

    public interface SeekBarAreaTouchListener {
        void onSeekBarAreaTouch();
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        float y = event.getY();
        switch (event.getAction()) {
        case MotionEvent.ACTION_DOWN:
            if (y<100)
                mSeekBarAreaTouchListener.onSeekBarAreaTouch();
            break;
        }
        return super.onTouchEvent(event);
    }

    class SlidingTimer extends Handler {
        @Override
        public void handleMessage(Message msg) {
            if (mTimerOn) {
                LEDView.this.update();
                LEDView.this.invalidate();
            }
        }

        public void sleep(long delayMillis) {
            this.removeMessages(0);
            sendMessageDelayed(obtainMessage(0), delayMillis);
        }
    }
}
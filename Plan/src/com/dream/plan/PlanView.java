/*========================================================================
Copyright (c) 2010 by LG Cappuccino Inc.  All Rights Reserved.

			EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when		who			what, where, why
----------	----------	---------------------------------------------------
2012/06/08	kihoon.kim	cappuccino Initial release.
=========================================================================*/
package com.dream.plan;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapShader;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Point;
import android.graphics.RectF;
import android.graphics.Shader;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.EdgeEffect;
import android.widget.EditText;
import android.widget.OverScroller;
import android.widget.Toast;

import java.util.Date;
import java.util.Timer;
import java.util.TimerTask;

import com.lge.config.AppConfig;
import com.dream.plan.Plan.Entry;

public class PlanView extends View implements OnGestureListener {

    private static final int		MSG_UPDATE 				= 1;	// Fling 시 업데이트 될수 있게
    private static final int 		ACTIVE_NONE 			= -1;	// 선택된 항목이 없을때
    private static final int		STATUS_NO_ACTIVE 		= 0;	// 선택된 항목이 없을때
    private static final int		STATUS_ACTIVE_INDEX 	= 1;	// 선택되었을때
    private static final int		STATUS_ACTIVE_START		= 2;	// 시작시간이 선택 되었을때
    private static final int		STATUS_ACTIVE_END		= 3;	// 종료시간이 선택 되었을때

    private static final int		POINT_SIZE 				= -8;	// 시작 시간, 종료 시간  영역
    private static final int		TOUCH_AREA 				= -30;	// 시작 시간, 종료 시간  터치영역
    private static final int		LINE_WIDTH 				= 2;	//

    private static final float		MIN_SCALE 				= (float) 1.0;

    private final GestureDetector 	mGestureDetector;		// Touch 인식
    private Shader 					mBG;					// 빈공간일때 배경
    private FlingHandler			mFlingHandler;			// Fling 화면 갱신 제어
    private Plan 					mPlan;					// 데이타 관리
    private Paint 					mTotalCirclePaint;		// 빈공간일때 배경
    private Paint 					mActiveArcLinePaint;	// 활성화된 라인
    private Paint 					mInactiveArcLinePaint;	// 비활성화된 라인
    private Paint 					mActiveArcPointPaint;	// 활성화된 점
    private Paint 					mInactiveArcPointPaint;	// 비활성화된 점
    private Context					mContext;

    private int						mStatus;				// 현재의 상태
    private int 					mActiveArcIndex;		// 선택된 항목 인덱스
    private int 					mRadius;				// 반지름
    private ExRectF					mCircleRect;			// 원 영역
    private ExPoint 				mCenter;				// 원 중심
    private OverScroller 			mScroller;				// 스크롤
    private ExPoint					mScrollMin;
    private ExPoint					mScrollMax;
    private float					mScale;					// 확대된 수치
    private float					mOldScale;				// 이전의 수치
    private float					mTouchDownScale;		// Touch Down 되었을때의 수치
    private float 					mCurDist;				// Pinch in, Pinch out 을 하기 위해 두점사이의 거리
    private float 					mOldDist;
    private boolean 				mMultiTouch;			// 2 Touch 중인지...

    private EdgeEffect				mEdgeGlowTop;
    private EdgeEffect				mEdgeGlowBottom;
    private EdgeEffect				mEdgeGlowLeft;
    private EdgeEffect				mEdgeGlowRight;

    private EditText				mEditText;
    private InputMethodManager 		mImm = null;

    Timer timer = new Timer();
    TimerTask timertask = new TimerTask() {
        public void run() {
            //@if ( AppConfig.LOGD ) Log.d(AppConfig.TAG,"TimerTask getCurrX()="+mScroller.getCurrX()+", getCurrY()="+mScroller.getCurrY());
            //@if ( AppConfig.LOGD ) Log.d(AppConfig.TAG,"TimerTask getScrollX()="+getScrollX()+", getScrollY()="+getScrollY());
        }
    };

    public PlanView(Context context) {
        this(context,null,0);
    }

    public PlanView(Context context, AttributeSet attrs) {
        this(context, attrs,0);
    }

    public PlanView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        mGestureDetector 	= new GestureDetector(context, this);

        Bitmap bm = Bitmap.createBitmap(new int[] { 0xFFFFFFFF, 0xFFEEEEEE, 0xFFEEEEEE, 0xFFFFFFFF }, // make a ckeckerboard pattern
                                        2, 2, Bitmap.Config.RGB_565);
        mBG = new BitmapShader(bm, Shader.TileMode.REPEAT, Shader.TileMode.REPEAT);
        Matrix m = new Matrix();
        m.setScale(15, 15);
        mBG.setLocalMatrix(m);

        mFlingHandler = new FlingHandler();
        mPlan 				= new Plan(context);

        mContext 			= context;

        mTotalCirclePaint 	= new Paint();
        mTotalCirclePaint.setARGB(255, 225, 225, 225);
        mTotalCirclePaint.setStyle(Paint.Style.FILL);
        mTotalCirclePaint.setShader(mBG);

        mActiveArcLinePaint 	= new Paint();
        mInactiveArcLinePaint 	= new Paint();
        mActiveArcPointPaint 	= new Paint();
        mInactiveArcPointPaint 	= new Paint();
        mActiveArcLinePaint.setStyle(Paint.Style.STROKE);
        mActiveArcLinePaint.setStrokeWidth(LINE_WIDTH);
        mInactiveArcLinePaint.setStyle(Paint.Style.STROKE);
        mInactiveArcLinePaint.setStrokeWidth(LINE_WIDTH);

        mActiveArcLinePaint.setARGB(255, 0, 0, 0);
        mActiveArcPointPaint.setARGB(255, 0, 0, 0);
        mInactiveArcLinePaint.setARGB(255, 128, 128, 128);
        mInactiveArcPointPaint.setARGB(255, 128, 128, 128);
        mActiveArcPointPaint.setTextSize(40);

        mActiveArcIndex 	= ACTIVE_NONE;
        mStatus 			= STATUS_NO_ACTIVE;

        mScroller			= new OverScroller(mContext);
        mScrollMin			= new ExPoint(0, 0);
        mScrollMax			= new ExPoint(0, 0);

        mRadius 			= 0;
        mScale				= 1;
        mOldScale			= 1;
        mTouchDownScale		= mScale;

        mOldDist 			= 0;
        mCurDist			= 0;
        mMultiTouch 		= false;

        mEdgeGlowTop = new EdgeEffect(mContext);
        mEdgeGlowBottom = new EdgeEffect(mContext);
        mEdgeGlowLeft = new EdgeEffect(mContext);
        mEdgeGlowRight = new EdgeEffect(mContext);

        timer.schedule(timertask, new Date(), 1000);

        mImm = (InputMethodManager)mContext.getSystemService(Context.INPUT_METHOD_SERVICE);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);

        int width = MeasureSpec.getSize(widthMeasureSpec);
        int height = MeasureSpec.getSize(heightMeasureSpec);

        if (width < height) {
        	mRadius 			= (width - getPaddingLeft() - getPaddingRight())/2;
        } else {
            mRadius 			= (height - getPaddingTop() - getPaddingBottom())/2;
        }

        calcCoordinate();
        setScrollMinMax();
        mEdgeGlowTop.setSize(width, height);
        mEdgeGlowBottom.setSize(width, height);
        mEdgeGlowLeft.setSize(height, width);
        mEdgeGlowRight.setSize(height, width);
    	coordinateEdit();

    }

    @Override
    public void draw(Canvas canvas) {
        super.draw(canvas);
        final int scrollX = getScrollX();
        final int scrollY = getScrollY();
        if (!mEdgeGlowTop.isFinished()) {
            if ( AppConfig.LOGD ) Log.d(AppConfig.TAG,"draw()");
            final int restoreCount = canvas.save();

            canvas.translate(getPaddingLeft()+scrollX, scrollY);
            if (mEdgeGlowTop.draw(canvas)) {
                invalidate();
            }

            canvas.restoreToCount(restoreCount);
        }
        if (!mEdgeGlowBottom.isFinished()) {
            final int restoreCount = canvas.save();

            final int width = getWidth();
            final int height = getHeight();
            canvas.translate(scrollX+getPaddingLeft(), height+scrollY);
            canvas.rotate(180, width/2, 0);
            if (mEdgeGlowBottom.draw(canvas)) {
                invalidate();
            }

            canvas.restoreToCount(restoreCount);
        }
        if (!mEdgeGlowLeft.isFinished()) {
            final int restoreCount = canvas.save();

            final int height = getHeight() - getPaddingTop() - getPaddingBottom();
            canvas.rotate(270);
            canvas.translate(-height+getPaddingTop(), scrollX);
            if (mEdgeGlowLeft.draw(canvas)) {
                invalidate();
            }

            canvas.restoreToCount(restoreCount);
        }
        if (!mEdgeGlowRight.isFinished()) {
            final int restoreCount = canvas.save();

            final int width = getWidth();
            canvas.rotate(90);
            canvas.translate(-getPaddingTop(), -scrollX-width);
            if (mEdgeGlowRight.draw(canvas)) {
                invalidate();
            }

            canvas.restoreToCount(restoreCount);
        }
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        canvas.drawArc(mCircleRect, 0, (float) (360), true, mTotalCirclePaint);
        int size = mPlan.size();
        for (int i=0;i<size;i++) {
            Entry entry = mPlan.get(i);
            Paint arcPaint = new Paint();
            arcPaint.setColor(entry.bgcolor);
            int rcStartAngle = convertTime2Angle(entry.startTime); 	//RectangularCoordinate
            int rcEndAngle = convertTime2Angle(entry.endTime);
            int scStartAngle = rcStartAngle - 90;					//ScreenCoordinate
            if (rcStartAngle < rcEndAngle) {
                canvas.drawArc(mCircleRect, scStartAngle, (float) rcEndAngle - rcStartAngle, true, arcPaint);
            } else {
                canvas.drawArc(mCircleRect, scStartAngle, (float) rcEndAngle - rcStartAngle + 360, true, arcPaint);
            }
            int x = 0, y = 0;
            if ( 315 <= rcStartAngle  || rcStartAngle < 45 ) {
                x = (int) (mCenter.x + mRadius/2 * Math.cos(scStartAngle * Math.PI/2/90));
                y = (int) (mCenter.y + mRadius/2 * Math.sin(scStartAngle * Math.PI/2/90));
            } else if (45 <= rcStartAngle && rcStartAngle < 135) {
                int angle = ( rcStartAngle + rcEndAngle )/2 - 90;
                x = (int) (mCenter.x + mRadius/4 * Math.cos(angle * Math.PI/2/90));
                y = (int) (mCenter.y + mRadius/4 * Math.sin(angle * Math.PI/2/90));
            } else if (135 <= rcStartAngle && rcStartAngle < 225) {
                int angle = rcEndAngle - 90;
                x = (int) (mCenter.x + mRadius/2 * Math.cos(angle * Math.PI/2/90));
                y = (int) (mCenter.y + mRadius/2 * Math.sin(angle * Math.PI/2/90));
            } else if (225 <= rcStartAngle && rcStartAngle < 315) {
                int angle = ( rcStartAngle + rcEndAngle )/2 - 90;
                x = (int) (mCenter.x + mRadius * Math.cos(angle * Math.PI/2/90));
                y = (int) (mCenter.y + mRadius * Math.sin(angle * Math.PI/2/90));
            }

            final int restoreCount = canvas.save();
            int angle = ( rcStartAngle + rcEndAngle + 10 )/2 - 90;
            x = (int) (mCenter.x + mRadius/4 * Math.cos(angle * Math.PI/2/90));
            y = (int) (mCenter.y + mRadius/4 * Math.sin(angle * Math.PI/2/90));

            canvas.translate(x, y);
            canvas.rotate(angle, 0, 0);

            canvas.drawText(entry.title, 0, 0, mActiveArcPointPaint);
            canvas.restoreToCount(restoreCount);
        }

        if ( mActiveArcIndex != ACTIVE_NONE ) {
            Entry entry = mPlan.get(mActiveArcIndex);

            Paint curArcLinePaint;
            Paint curArcPointPaint;
            if ( 	mStatus == STATUS_ACTIVE_START ||
                    mStatus == STATUS_ACTIVE_END ) {
                curArcLinePaint = mActiveArcLinePaint;
                curArcPointPaint = mActiveArcPointPaint;
            } else {
                curArcLinePaint = mInactiveArcLinePaint;
                curArcPointPaint = mInactiveArcPointPaint;
            }
            int rcStartAngle = convertTime2Angle(entry.startTime);
            int rcEndAngle = convertTime2Angle(entry.endTime);
            int scStartAngle = rcStartAngle - 90;
            if (rcStartAngle < rcEndAngle) {
                canvas.drawArc(mCircleRect, scStartAngle, (float) rcEndAngle - rcStartAngle, true, curArcLinePaint);
            } else {
                canvas.drawArc(mCircleRect, scStartAngle, (float) rcEndAngle - rcStartAngle + 360, true, curArcLinePaint);
            }

            RectF rect1 = new RectF(mCenter.x, mCenter.y, mCenter.x, mCenter.y);
            rect1.inset(mScale*POINT_SIZE, mScale*POINT_SIZE);
            canvas.drawRect(rect1, curArcPointPaint);

            Point point = getPoint(entry.startTime);
            RectF rect2 = new RectF(mScale*point.x, mScale*point.y, mScale*point.x, mScale*point.y);
            rect2.inset(mScale*POINT_SIZE, mScale*POINT_SIZE);
            rect2.offset(getPaddingLeft(), getPaddingTop());
            canvas.drawRect(rect2, curArcPointPaint);

            point = getPoint(entry.endTime);
            RectF rect3 = new RectF(mScale*point.x, mScale*point.y, mScale*point.x, mScale*point.y);
            rect3.inset(mScale*POINT_SIZE, mScale*POINT_SIZE);
            rect3.offset(getPaddingLeft(), getPaddingTop());
            canvas.drawRect(rect3, curArcPointPaint);
        }
    }

    @Override
    protected int computeVerticalScrollRange() {
        return (int)(mRadius * 2 * mScale)+getPaddingTop()+getPaddingBottom();
    }

    @Override
    protected int computeHorizontalScrollRange() {
        return (int)(mRadius * 2 * mScale)+getPaddingLeft()+getPaddingRight();
    }

    @Override
    protected int computeVerticalScrollExtent() {
        return getHeight();
    }

    @Override
    protected int computeHorizontalScrollExtent() {
        return  getWidth();
    }

    @Override
    public void computeScroll() {
        super.computeScroll();
        if (mScroller.computeScrollOffset()) {
            int oldX = getScrollX();
            int oldY = getScrollY();
            int x = mScroller.getCurrX();
            int y = mScroller.getCurrY();
            if (oldX != x || oldY != y) {
                final int verticalScrollRange 	= computeVerticalScrollRange();
                final int horizontalScrollRange = computeHorizontalScrollRange();
                final int verticalScrollExt 	= computeVerticalScrollExtent();
                final int horizontalScrollExt 	= computeHorizontalScrollExtent();
                //@if ( AppConfig.LOGD ) Log.d(AppConfig.TAG,"computeScroll() verticalScrollRange="+verticalScrollRange+", verticalScrollExt="+verticalScrollExt);
                //@if ( AppConfig.LOGD ) Log.d(AppConfig.TAG,"computeScroll() horizontalScrollRange="+horizontalScrollRange+", horizontalScrollExt="+horizontalScrollExt);
                overScrollBy(0, 0, getScrollX(), getScrollY(), verticalScrollRange, horizontalScrollRange, verticalScrollExt, horizontalScrollExt, true);

                awakenScrollBars();
                postInvalidate();
            }
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (mGestureDetector.onTouchEvent(event))
            return true;
        int action = event.getAction();
        float originX1 = event.getX();
        float originY1 = event.getY();
        //@if ( AppConfig.LOGD ) Log.d(AppConfig.TAG," onTouchEvent() event = " + event);
        switch (action) {
        case MotionEvent.ACTION_UP:
            TouchUpEvent(event);
            break;
        case MotionEvent.ACTION_MOVE:
        case MotionEvent.ACTION_POINTER_DOWN | 0x100:
            int pointCount = event.getPointerCount();
            if (pointCount > 1) {
                float originX2 = event.getX(1);
                float originY2 = event.getY(1);
                // distance between
                mCurDist = (float) Math.sqrt(Math.pow(originX2 - originX1, 2) + Math.pow(originY2 - originY1, 2));
                if (action == MotionEvent.ACTION_MOVE) {
                    mScale = mTouchDownScale + mCurDist / mOldDist - 1;
                    if (mScale < MIN_SCALE) {
                        mScale = MIN_SCALE;
                    }
                    setScrollMinMax();
                    checkInvalidScrollRange();
                    mOldScale = mScale;
                    //@if ( AppConfig.LOGD ) Log.d(AppConfig.TAG, "onTouchEvent() scale="+mScale);
                    calcCoordinate();
                    invalidate();
                } else if (action == (MotionEvent.ACTION_POINTER_DOWN | 0x100)) {
                    mOldDist 			= mCurDist;
                    mTouchDownScale 	= mOldScale = mScale;
                    mMultiTouch = true;
                }
            } else {
                if (action == MotionEvent.ACTION_MOVE) {
                    TouchMoveEvent(event);
                }
            }
            break;
        case MotionEvent.ACTION_POINTER_UP | 0x100:
            break;
        }
        return super.onTouchEvent(event);
    }

    public boolean onDown(MotionEvent event) {
        //@if ( AppConfig.LOGD ) Log.d(AppConfig.TAG,"onDown()");
        mScroller.forceFinished(true);
        calcCoordinate();

        float originX = event.getX();
        float originY = event.getY();

        if ( mActiveArcIndex != ACTIVE_NONE) {
            Entry entry = mPlan.get(mActiveArcIndex);

            Point startPoint = getPoint(entry.startTime);
            ExRectF startRect = new ExRectF(startPoint.x, startPoint.y, startPoint.x, startPoint.y);
            startRect.magnify(mScale);
            startRect.offset(getPaddingLeft(), getPaddingTop());
            startRect.offset(-mScroller.getCurrX(), -mScroller.getCurrY());
            startRect.inset(TOUCH_AREA, TOUCH_AREA);

            Point endPoint = getPoint(entry.endTime);
            ExRectF endRect = new ExRectF(endPoint.x, endPoint.y, endPoint.x, endPoint.y);
            endRect.magnify(mScale);
            endRect.offset(getPaddingLeft(), getPaddingTop());
            endRect.offset(-mScroller.getCurrX(), -mScroller.getCurrY());
            endRect.inset(TOUCH_AREA, TOUCH_AREA);

            if (startRect.contains(originX, originY)) {
                //@if ( AppConfig.LOGD ) Log.d(AppConfig.TAG," startRect touch down");
                mStatus = STATUS_ACTIVE_START;
                invalidate();
                return true;
            } else if (endRect.contains(originX, originY)) {
                //@if ( AppConfig.LOGD ) Log.d(AppConfig.TAG," endRect touch down");
                mStatus = STATUS_ACTIVE_END;
                invalidate();
                return true;
            }
        }
        return false;
    }

    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
        if (	mStatus == STATUS_ACTIVE_START ||
                mStatus == STATUS_ACTIVE_END ) {
            return false;
        }
        if (mMultiTouch)
            return false;

        if ( AppConfig.LOGD ) Log.d(AppConfig.TAG,"onFling() velocityX="+velocityX+", velocityY="+velocityY);
        //@if ( AppConfig.LOGD ) Log.d(AppConfig.TAG,"onFling() velocityX="+velocityX+", velocityY="+velocityY);

        mScroller.fling(mScroller.getCurrX(), mScroller.getCurrY(), (int)-velocityX, (int)-velocityY, mScrollMin.x, mScrollMax.x, mScrollMin.y, mScrollMax.y);
        Message msg = mFlingHandler.obtainMessage(MSG_UPDATE);
        mFlingHandler.sendMessage(msg);
        return false;
    }

    public void onLongPress(MotionEvent e) {
        float originX = e.getX();
        float originY = e.getY();
        float x = originX - mRadius * mScale + mScroller.getCurrX() - getPaddingLeft();
        float y = originY - mRadius * mScale + mScroller.getCurrY() - getPaddingTop();
        if ( Math.pow((x), 2) + Math.pow((y), 2) <  Math.pow(mRadius * mScale, 2) ) {
            double angle = getAngle(x, y);
            int size = mPlan.size();
            mActiveArcIndex = ACTIVE_NONE;
            mStatus = STATUS_NO_ACTIVE;
            for (int i=0;i<size;i++) {
                Entry entry = mPlan.get(i);
                int time = (int)(angle * 4);
                if ( entry.isInTime(time) ) {
                    mActiveArcIndex = i;
                    //mStatus = STATUS_ACTIVE_INDEX;
                    Toast.makeText(mContext, i+" is selected", Toast.LENGTH_SHORT).show();
                }
            }
            //invalidate();
        }
    }

    public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
        if (	mStatus == STATUS_ACTIVE_START ||
                mStatus == STATUS_ACTIVE_END ) {
            return false;
        }
        if (mMultiTouch)
            return false;
        //@if ( AppConfig.LOGD ) Log.d(AppConfig.TAG,"onScroll() distanceX="+distanceX+", distanceY="+distanceY);

        if (mScroller.getCurrX()+distanceX < mScrollMin.x) {
            mEdgeGlowLeft.onPull(distanceX/getWidth());
            distanceX = (mScrollMin.x - mScroller.getCurrX());
        }
        if (mScroller.getCurrX()+distanceX > mScrollMax.x) {
            mEdgeGlowRight.onPull(distanceX/getWidth());
            distanceX = (mScrollMax.x - mScroller.getCurrX());
        }
        if (mScroller.getCurrY()+distanceY < mScrollMin.y) {
            mEdgeGlowTop.onPull(distanceY/getHeight());
            distanceY = (mScrollMin.y - mScroller.getCurrY());
        }
        if (mScroller.getCurrY()+distanceY > mScrollMax.y) {
            mEdgeGlowBottom.onPull(distanceY/getHeight());
            distanceY = (mScrollMax.y - mScroller.getCurrY());
        }

        mScroller.startScroll(mScroller.getCurrX(), mScroller.getCurrY(), (int)distanceX, (int)distanceY, 0);
        mScroller.abortAnimation();

        calcCoordinate();
        scrollTo(mScroller.getCurrX(), mScroller.getCurrY());
        invalidate();
        return false;
    }

    public void onShowPress(MotionEvent e) {
        //@if ( AppConfig.LOGD ) Log.d(AppConfig.TAG,"onShowPress()");
    }

    public boolean onSingleTapUp(MotionEvent e) {
        //@if ( AppConfig.LOGD ) Log.d(AppConfig.TAG,"onSingleTapUp()");

        float originX = e.getX();
        float originY = e.getY();
        float x = originX - mRadius * mScale + mScroller.getCurrX() - getPaddingLeft();
        float y = originY - mRadius * mScale + mScroller.getCurrY() - getPaddingTop();
        saveText(false);
        if ( Math.pow((x), 2) + Math.pow((y), 2) <  Math.pow(mRadius * mScale, 2) ) {
            double angle = getAngle(x, y);
            //@if ( AppConfig.LOGD ) Log.d(AppConfig.TAG,"angle="+angle);
            int size = mPlan.size();

            for (int i=0;i<size;i++) {
                Entry entry = mPlan.get(i);
                int time = (int)(angle * 4);
                if ( entry.isInTime(time) ) {
                    //@if ( AppConfig.LOGD ) Log.d(AppConfig.TAG," i = "+i);
                    if (mActiveArcIndex == i) {
                        if (mEditText.getVisibility() == View.GONE) {
                        	coordinateEdit();
                            mEditText.setText(entry.title);
                            mEditText.setVisibility(View.VISIBLE);
                            mImm.showSoftInput(mEditText, 0);
                        } else {
                            mImm.hideSoftInputFromWindow(mEditText.getWindowToken(), 0);
                            mEditText.setVisibility(View.GONE);
                        }
                    } else {
                        mImm.hideSoftInputFromWindow(mEditText.getWindowToken(), 0);
                        mEditText.setVisibility(View.GONE);
                    }
                    mActiveArcIndex = i;
                    mStatus = STATUS_ACTIVE_INDEX;
                    invalidate();
                    return false;
                }
            }
        }
//        saveText(true);
        mImm.hideSoftInputFromWindow(mEditText.getWindowToken(), 0);
        mEditText.setVisibility(View.GONE);

        mActiveArcIndex = ACTIVE_NONE;
        mStatus = STATUS_NO_ACTIVE;
        invalidate();
        return false;
    }
    
    private void coordinateEdit() {
    	if(mActiveArcIndex != ACTIVE_NONE && STATUS_ACTIVE_INDEX == mStatus) {
	        Entry entry = mPlan.get(mActiveArcIndex);
	
	        ViewGroup.MarginLayoutParams lp = (ViewGroup.MarginLayoutParams)mEditText.getLayoutParams();
	        //lp.leftMargin = (int)originX;
	        //lp.topMargin = (int)originY;
	        int rcStartAngle = convertTime2Angle(entry.startTime);
	        int rcEndAngle = convertTime2Angle(entry.endTime);
	
	        int angleAve = ( rcStartAngle + rcEndAngle - 10)/2 - 90;
	        int x = (int) (mCenter.x + mRadius*0.80 * Math.cos(angleAve * Math.PI/2/90));
	        int y = (int) (mCenter.y + mRadius*0.80 * Math.sin(angleAve * Math.PI/2/90));
	
	        lp.leftMargin = (int)x;
	        lp.topMargin = (int)y;
	
	        mEditText.setLayoutParams(lp);
    	}
    }

    private void saveText(boolean hideImm) {
        if ( mActiveArcIndex != ACTIVE_NONE ) {
            if ( mEditText.getVisibility() == View.VISIBLE ) {
                Entry oldEntry = mPlan.get(mActiveArcIndex);
                oldEntry.title = mEditText.getText().toString();
                mEditText.setText("");
                if (hideImm) {
                    mImm.hideSoftInputFromWindow(mEditText.getWindowToken(), 0);
                    mEditText.setVisibility(View.GONE);
                }
            }
        }
    }

    public void addDefaultPlan() {
        mPlan.addDefaultPlan();
        invalidate();
    }

    public void registerEditText(EditText et) {
        mEditText = et;
    }

    public void restore() {
        mPlan.restore();
    }
	
    public void save() {
        mPlan.save();
    }
	
    private void calcCoordinate() {
        //@if ( AppConfig.LOGD ) Log.d(AppConfig.TAG,"calcCoordinate()");
        mCircleRect 		= new ExRectF(0, 0, 2 * mRadius , 2 * mRadius );
        mCenter 			= new ExPoint(mRadius, mRadius);

        mCircleRect.magnify(mScale);
        mCircleRect.offset(getPaddingLeft(), getPaddingTop());
        mCenter.magnify(mScale);
        mCenter.offset(getPaddingLeft(), getPaddingTop());
    }

    private void setScrollMinMax() {
        int scrollX = (int)(2*mRadius*mScale) + getPaddingLeft() + getPaddingRight() - getWidth();
        int scrollY = (int)(2*mRadius*mScale) + getPaddingTop() + getPaddingBottom() - getHeight();
        mScrollMax.x = Math.max(0, scrollX);
        mScrollMax.y = Math.max(0, scrollY);
    }

    private void checkInvalidScrollRange() {
        int diff;
        boolean isExpand = false;
        if (mOldScale < mScale) {
            isExpand = true;
        }
        if (mScroller.getCurrX() < mScrollMin.x) {
            diff = mScrollMin.x - mScroller.getCurrX();
            mScroller.startScroll(mScroller.getCurrX(), mScroller.getCurrY(), isExpand ? diff : -diff, (int)0, 0);
            mScroller.abortAnimation();
        } else if (mScroller.getCurrX() > mScrollMax.x) {
            diff = mScroller.getCurrX() - mScrollMax.x;
            mScroller.startScroll(mScroller.getCurrX(), mScroller.getCurrY(), isExpand ? diff : -diff, (int)0, 0);
            mScroller.abortAnimation();
        }

        if (mScroller.getCurrY() < mScrollMin.y) {
            diff = mScrollMin.y - mScroller.getCurrY();
            mScroller.startScroll(mScroller.getCurrX(), mScroller.getCurrY(), (int)0, isExpand ? diff : -diff, 0);
            mScroller.abortAnimation();
        } else if (mScroller.getCurrY() > mScrollMax.y) {
            diff = mScroller.getCurrY() - mScrollMax.y;
            mScroller.startScroll(mScroller.getCurrX(), mScroller.getCurrY(), (int)0, isExpand ? diff : -diff, 0);
            mScroller.abortAnimation();
        }
        scrollTo(mScroller.getCurrX(), mScroller.getCurrY());
    }

    private int convertTime2Angle(int time) {
        return (360 / 24 * time / 60);
    }

    private double getAngle(float x, float y) {
        return (1 - (Math.atan2(x , y)/Math.PI))*180;
    }

    private Point getPoint(int time) {
        Point point = new Point();
        int rcAngle = convertTime2Angle(time);	//RectangularCoordinate
        int scAngle = rcAngle - 90;    			//ScreenCoordinate

        int x = (int) (Math.cos(scAngle * Math.PI/2/90)*mRadius) + mRadius;
        int y = (int) (Math.sin(scAngle * Math.PI/2/90)*mRadius) + mRadius;

        point.x = x;
        point.y = y;
        return point;
    }

    private void TouchMoveEvent(MotionEvent event) {
        float originX = event.getX();
        float originY = event.getY();
        float x = originX - mRadius * mScale - getPaddingLeft() + mScroller.getCurrX();
        float y = originY - mRadius * mScale - getPaddingTop() + mScroller.getCurrY();
        if ( mStatus == STATUS_ACTIVE_START ) {
            double angle = getAngle(x, y);
            angle = ((int)((angle+3.5) / 7.5)) * 7.5;
            Entry entry = mPlan.get(mActiveArcIndex);
            entry.startTime = (int) (angle * 4);
            //@if ( AppConfig.LOGD ) Log.d(AppConfig.TAG," Move angle = "+angle);
            invalidate();
        } else if (mStatus == STATUS_ACTIVE_END) {
            double angle = getAngle(x, y);
            angle = ((int)((angle+3.5) / 7.5)) * 7.5;
            Entry entry = mPlan.get(mActiveArcIndex);
            entry.endTime = (int) (angle * 4);
            //@if ( AppConfig.LOGD ) Log.d(AppConfig.TAG," Move angle = "+angle);
            invalidate();
        }
    }

    private void TouchUpEvent(MotionEvent event) {
        if (	mStatus == STATUS_ACTIVE_START ||
                mStatus == STATUS_ACTIVE_END ) {
            mStatus = STATUS_ACTIVE_INDEX;
        } else {
            if ( mMultiTouch ) {
                mMultiTouch = false;
            }
            endDrag();
        }
    }

    private void endDrag() {
        if ( AppConfig.LOGD ) Log.d(AppConfig.TAG,"computeScroll endDrag()");
        mEdgeGlowTop.onRelease();
        mEdgeGlowBottom.onRelease();
        mEdgeGlowLeft.onRelease();
        mEdgeGlowRight.onRelease();
    }

    class ExRectF extends RectF {
        public ExRectF(float left, float top, float right, float bottom) {
            super(left, top, right, bottom);
        }

        public void magnify(float mag) {
            left    *= mag;
            top     *= mag;
            right   *= mag;
            bottom  *= mag;
        }
    }

    class ExPoint extends Point {
        public ExPoint(int x, int y) {
            super(x, y);
        }

        public void magnify(float mag) {
            x    *= mag;
            y    *= mag;
        }
    }

    class FlingHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what)	{
            case MSG_UPDATE:
                if (mScroller.isFinished()==false) {
                    int curX, curY;
                    int oldX = getScrollX();
                    int oldY = getScrollY();

                    mScroller.computeScrollOffset();
                    calcCoordinate();
                    scrollTo(mScroller.getCurrX(), mScroller.getCurrY());
                    curX = mScroller.getCurrX();
                    curY = mScroller.getCurrY();

                    if (oldY>mScrollMin.y && curY<=mScrollMin.y) {
                        mEdgeGlowTop.onAbsorb((int) mScroller.getCurrVelocity());
                    }
                    if (oldY<mScrollMax.y && curY>=mScrollMax.y) {
                        mEdgeGlowBottom.onAbsorb((int) mScroller.getCurrVelocity());
                    }
                    if (oldX>mScrollMin.x && curX<=mScrollMin.x) {
                        mEdgeGlowLeft.onAbsorb((int) mScroller.getCurrVelocity());
                    }
                    if (oldX<mScrollMax.x && curX>=mScrollMax.x) {
                        mEdgeGlowRight.onAbsorb((int) mScroller.getCurrVelocity());
                    }
                    invalidate();
                    Message updateMsg = mFlingHandler.obtainMessage(MSG_UPDATE);
                    mFlingHandler.sendMessage(updateMsg);
                } else {
                    mEdgeGlowTop.onRelease();
                    mEdgeGlowBottom.onRelease();
                    mEdgeGlowLeft.onRelease();
                    mEdgeGlowRight.onRelease();
                }
                break;
            }
            super.handleMessage(msg);
        }
    }
}

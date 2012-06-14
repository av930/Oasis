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
import android.widget.OverScroller;

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
    private static final int		MARGIN_SIZE 			= 30;	// 여백
    private static final int		LINE_WIDTH 				= 2;	//

    private static final float		MIN_SCALE 				= (float) 0.5;

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
    private ExPoint 				mMargin;				// 여백
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

    public PlanView(Context context) {
        this(context,null,0);
    }

    public PlanView(Context context, AttributeSet attrs) {
        this(context, attrs,0);
    }

    public PlanView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        mGestureDetector 	= new GestureDetector(this);

        Bitmap bm = Bitmap.createBitmap(new int[] { 0xFFFFFFFF, 0xFFEEEEEE, 0xFFEEEEEE, 0xFFFFFFFF }, // make a ckeckerboard pattern
                                        2, 2, Bitmap.Config.RGB_565);
        mBG = new BitmapShader(bm, Shader.TileMode.REPEAT, Shader.TileMode.REPEAT);
        Matrix m = new Matrix();
        m.setScale(15, 15);
        mBG.setLocalMatrix(m);

        mFlingHandler = new FlingHandler();
        mPlan 				= new Plan();

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

        mActiveArcIndex 	= ACTIVE_NONE;
        mStatus 			= STATUS_NO_ACTIVE;

        mMargin 			= new ExPoint(MARGIN_SIZE, MARGIN_SIZE);
        mScroller				= new OverScroller(mContext);
        mScrollMin			= new ExPoint(0, 0);
        mScrollMax			= new ExPoint(0, 0);

        mRadius 			= (AppConfig.mShortAxis)/2 - mMargin.x;
        mScale				= 1;
        mOldScale			= 1;
        mTouchDownScale		= mScale;
        mMargin.magnify(mScale);

        mOldDist 			= 0;
        mCurDist			= 0;
        mMultiTouch 		= false;
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);

        int shortAxis;
        if (getWidth()>getHeight()) {
            shortAxis = getHeight();
        } else {
            shortAxis = getWidth();
        }

        mRadius 			= shortAxis / 2 - mMargin.x;
        calcCoordinate();
        setScrollMinMax();
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
            int rcStartAngle = convertTime2Angle(entry.startTime);
            int rcEndAngle = convertTime2Angle(entry.endTime);
            int scStartAngle = rcStartAngle - 90;
            if (rcStartAngle < rcEndAngle) {
                canvas.drawArc(mCircleRect, scStartAngle, (float) rcEndAngle - rcStartAngle, true, arcPaint);
            } else {
                canvas.drawArc(mCircleRect, scStartAngle, (float) rcEndAngle - rcStartAngle + 360, true, arcPaint);
            }
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
            rect2.offset(mScale*(mMargin.x) - mScroller.getCurrX(), mScale*(mMargin.y) - mScroller.getCurrY());
            canvas.drawRect(rect2, curArcPointPaint);

            point = getPoint(entry.endTime);
            RectF rect3 = new RectF(mScale*point.x, mScale*point.y, mScale*point.x, mScale*point.y);
            rect3.inset(mScale*POINT_SIZE, mScale*POINT_SIZE);
            rect3.offset(mScale*(mMargin.x) - mScroller.getCurrX(), mScale*(mMargin.y) - mScroller.getCurrY());
            canvas.drawRect(rect3, curArcPointPaint);
        }
    }
    @Override
	protected void dispatchDraw(Canvas canvas) {
		// TODO Auto-generated method stub
		super.dispatchDraw(canvas);
	}

	@Override
	public void draw(Canvas canvas) {
		// TODO Auto-generated method stub
		super.draw(canvas);
	}

	private int getScrollRange() {
    	/*
         int scrollRange = 0; 
         if (getChildCount() > 0) {
             View child = getChildAt(0);
             scrollRange = Math.max(0,  
                     child.getHeight() - (getHeight() - mPaddingBottom - mPaddingTop));
         }                                                                             
         return scrollRange;
         */
    	return 1000;
    }

	static int xxxx1 = 50;
    static int xxxx2 = 300;
    static int yyyy1 = 50;
    static int yyyy2 = 300;
    
    @Override
	public void computeScroll() {		
		super.computeScroll();		
        if (mScroller.computeScrollOffset()) {                                                                                                            
           int oldX = mScroller.getCurrX();  ;                                                                                                                           
           int oldY = mScroller.getCurrY();  ;                                                                                                                           
           int x = mScroller.getCurrX();                                                                                                                  
           int y = mScroller.getCurrY();                                                                                                                  
                                   
           
           //if (oldX != x || oldY != y) {                                                                                                                  
               final int range = getScrollRange();         
               setOverScrollMode(OVER_SCROLL_ALWAYS);
               //scrollTo(xxxx1,yyyy2);
               final int overscrollMode = getOverScrollMode();                                                                                            
               final boolean canOverscroll = overscrollMode == OVER_SCROLL_ALWAYS ||                                                                      
                       (overscrollMode == OVER_SCROLL_IF_CONTENT_SCROLLS && range > 0);                                                                   
               
               overScrollBy(xxxx1, yyyy1, xxxx2, yyyy2, 0, range,                                                                                     
                       0, 1000, true);    
               Log.i(AppConfig.TAG, "xxxx1"+xxxx1);
               xxxx1 += 5;
               xxxx2 += 5;
               yyyy1 += 5;
               yyyy2 += 5;
               
               /*
               onScrollChanged(mScrollX, mScrollY, oldX, oldY);                                                                                           
                                                                                                                                                          
               if (canOverscroll) {                                                                                                                       
                   if (y < 0 && oldY >= 0) {                                                                                                              
                       mEdgeGlowTop.onAbsorb((int) mScroller.getCurrVelocity());                                                                          
                   } else if (y > range && oldY <= range) {                                                                                               
                       mEdgeGlowBottom.onAbsorb((int) mScroller.getCurrVelocity());                                                                       
                   }                                                                                                                                      
               } 
               */                                                                                                                                         
           //}                                                                                                                                              
                                                                                                                                                          
           awakenScrollBars();                                                                                                                            
                                                                                                                                                          
           // Keep on drawing until the animation has finished.                                                                                           
           postInvalidate();		
        } else {             
        	/*
            if (mFlingStrictSpan != null) {                                                                                                               
                mFlingStrictSpan.finish();                                                                                                                
                mFlingStrictSpan = null;                                                                                                                  
            } 
            */                                                                                                                                            
        }        
	}

	@Override
    public boolean onTouchEvent(MotionEvent event) {
        if (mGestureDetector.onTouchEvent(event))
            return true;
        int action = event.getAction();
        float originX1 = event.getX();
        float originY1 = event.getY();
        //if ( AppConfig.LOGD ) Log.d(AppConfig.TAG," onTouchEvent() event = " + event);
        switch (action) {
        case MotionEvent.ACTION_DOWN:
            mScroller.forceFinished(true);
            calcCoordinate();
            TouchDownEvent(event);
            break;
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
                    if ( AppConfig.LOGD ) Log.d(AppConfig.TAG, "onTouchEvent() scale="+mScale);

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

    public boolean onDown(MotionEvent arg0) {
        if ( AppConfig.LOGD ) Log.d(AppConfig.TAG,"onDown()");
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

        mScroller.fling(mScroller.getCurrX(), mScroller.getCurrY(), (int)-velocityX, (int)-velocityY, mScrollMin.x, mScrollMax.x, mScrollMin.y, mScrollMax.y);
        Message msg = mFlingHandler.obtainMessage(MSG_UPDATE);
        mFlingHandler.sendMessage(msg);
        return false;
    }

    public void onLongPress(MotionEvent e) {
        if ( AppConfig.LOGD ) Log.d(AppConfig.TAG,"onLongPress()");
    }

    public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
        if (	mStatus == STATUS_ACTIVE_START ||
                mStatus == STATUS_ACTIVE_END ) {
            return false;
        }
        if (mMultiTouch)
            return false;

        if ( AppConfig.LOGD ) Log.d(AppConfig.TAG,"onScroll() distanceX="+distanceX+", distanceY="+distanceY);
        //mScroller.x -= distanceX;
        //mScroller.y -= distanceY;

        if (mScroller.getCurrX()+distanceX < mScrollMin.x) {
            distanceX = (mScrollMin.x - mScroller.getCurrX());
        }
        if (mScroller.getCurrX()+distanceX > mScrollMax.x) {
            distanceX = (mScrollMax.x - mScroller.getCurrX());
        }
        if (mScroller.getCurrY()+distanceY < mScrollMin.y) {
            distanceY = (mScrollMin.y - mScroller.getCurrY());
        }
        if (mScroller.getCurrY()+distanceY > mScrollMax.y) {
            distanceY = (mScrollMax.y - mScroller.getCurrY());
        }
        mScroller.startScroll(mScroller.getCurrX(), mScroller.getCurrY(), (int)distanceX, (int)distanceY, 0);
        mScroller.abortAnimation();
        calcCoordinate();
        invalidate();
        return false;
    }

    public void onShowPress(MotionEvent e) {
        if ( AppConfig.LOGD ) Log.d(AppConfig.TAG,"onShowPress()");
    }

    public boolean onSingleTapUp(MotionEvent e) {
        if ( AppConfig.LOGD ) Log.d(AppConfig.TAG,"onSingleTapUp()");

        float originX = e.getX();
        float originY = e.getY();
        float x = originX - (mRadius + mMargin.x ) * mScale + mScroller.getCurrX();
        float y = originY - (mRadius + mMargin.y ) * mScale + mScroller.getCurrY();
        if ( Math.pow((x), 2) + Math.pow((y), 2) <  Math.pow(mRadius * mScale, 2) ) {
            double angle = getAngle(x, y);
            if ( AppConfig.LOGD ) Log.d(AppConfig.TAG,"angle="+angle);
            int size = mPlan.size();
            mActiveArcIndex = ACTIVE_NONE;
            mStatus = STATUS_NO_ACTIVE;
            for (int i=0;i<size;i++) {
                Entry entry = mPlan.get(i);
                int time = (int)(angle * 4);
                if ( entry.isInTime(time) ) {
                    if ( AppConfig.LOGD ) Log.d(AppConfig.TAG," i = "+i);
                    mActiveArcIndex = i;
                    mStatus = STATUS_ACTIVE_INDEX;
                }
            }
            invalidate();
        }
        return false;
    }

    public void addDefaultPlan() {
        mPlan.addDefaultPlan();
        invalidate();
    }

    private void calcCoordinate() {
        mCircleRect 		= new ExRectF(0, 0, 2 * mRadius , 2 * mRadius );
        mCenter 			= new ExPoint(mRadius, mRadius);

        mCircleRect.offset(mMargin.x, mMargin.y);
        mCircleRect.magnify(mScale);
        mCircleRect.offset(-mScroller.getCurrX() , -mScroller.getCurrY());

        mCenter.offset(mMargin.x, mMargin.y);
        mCenter.magnify(mScale);
        mCenter.offset(-mScroller.getCurrX() , -mScroller.getCurrY());
    }

    private void setScrollMinMax() {
        int scrollX = (int)(2*(mRadius + mMargin.x)*mScale) - getWidth();
        int scrollY = (int)(2*(mRadius + mMargin.y)*mScale) - getHeight();
        if ( scrollX >= 0 ) {
            mScrollMin.x = 0;
            mScrollMax.x = scrollX;
        } else {
            mScrollMin.x = scrollX;
            mScrollMax.x = 0;
        }
        if ( scrollY >= 0 ) {
            mScrollMin.y = 0;
            mScrollMax.y = scrollY;
        } else {
            mScrollMin.y = scrollY;
            mScrollMax.y = 0;
        }
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

    private void TouchDownEvent(MotionEvent event) {
        float originX = event.getX();
        float originY = event.getY();

        if ( mActiveArcIndex != ACTIVE_NONE) {
            Entry entry = mPlan.get(mActiveArcIndex);

            Point startPoint = getPoint(entry.startTime);
            ExRectF startRect = new ExRectF(startPoint.x, startPoint.y, startPoint.x, startPoint.y);
            startRect.offset(mMargin.x, mMargin.y);
            startRect.magnify(mScale);
            startRect.offset(-mScroller.getCurrX(), -mScroller.getCurrY());
            startRect.inset(TOUCH_AREA, TOUCH_AREA);

            Point endPoint = getPoint(entry.endTime);
            ExRectF endRect = new ExRectF(endPoint.x, endPoint.y, endPoint.x, endPoint.y);
            endRect.offset(mMargin.x, mMargin.y);
            endRect.magnify(mScale);
            endRect.offset(-mScroller.getCurrX(), -mScroller.getCurrY());
            endRect.inset(TOUCH_AREA, TOUCH_AREA);

            if (startRect.contains(originX, originY)) {
                if ( AppConfig.LOGD ) Log.d(AppConfig.TAG," startRect touch down");
                mStatus = STATUS_ACTIVE_START;
                invalidate();
                return ;
            } else if (endRect.contains(originX, originY)) {
                if ( AppConfig.LOGD ) Log.d(AppConfig.TAG," endRect touch down");
                mStatus = STATUS_ACTIVE_END;
                invalidate();
                return ;
            }
        }
    }

    private void TouchMoveEvent(MotionEvent event) {
        float originX = event.getX();
        float originY = event.getY();
        float x = originX - (mRadius + mMargin.x ) * mScale + mScroller.getCurrX();
        float y = originY - (mRadius + mMargin.y ) * mScale + mScroller.getCurrY();
        if ( mStatus == STATUS_ACTIVE_START ) {
            double angle = getAngle(x, y);
            angle = ((int)((angle+3.5) / 7.5)) * 7.5;
            Entry entry = mPlan.get(mActiveArcIndex);
            entry.startTime = (int) (angle * 4);
            if ( AppConfig.LOGD ) Log.d(AppConfig.TAG," Move angle = "+angle);
            invalidate();
        } else if (mStatus == STATUS_ACTIVE_END) {
            double angle = getAngle(x, y);
            angle = ((int)((angle+3.5) / 7.5)) * 7.5;
            Entry entry = mPlan.get(mActiveArcIndex);
            entry.endTime = (int) (angle * 4);
            if ( AppConfig.LOGD ) Log.d(AppConfig.TAG," Move angle = "+angle);
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
        }
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
                    mScroller.computeScrollOffset();
                    calcCoordinate();
                    invalidate();
                    Message updateMsg = mFlingHandler.obtainMessage(MSG_UPDATE);
                    mFlingHandler.sendMessage(updateMsg);
                }
                break;
            }
            super.handleMessage(msg);
        }
    }
}

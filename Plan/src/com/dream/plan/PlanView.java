package com.dream.plan;

import com.dream.plan.Plan.Entry;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Point;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.util.Log;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Toast;
import com.lge.config.AppConfig;

public class PlanView extends View implements OnGestureListener {

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

    private static final int 		ACTIVE_NONE 			= -1;

    private static final int		STATUS_NO_ACTIVE 		= 0;
    private static final int		STATUS_ACTIVE_INDEX 	= 1;
    private static final int		STATUS_ACTIVE_START		= 2;
    private static final int		STATUS_ACTIVE_END		= 3;

    private static final int		POINT_SIZE = -8;
    private static final int		TOUCH_AREA = -30;
    private static final int		LINE_WIDTH = 2;

    private final GestureDetector 	mGestureDetector;
    private int						mStatus;
    private int 					mActiveArcIndex;
    private int 					mRadius;
    private Plan 					mPlan;
    private ExPoint 				mMargin;
    private ExRectF					mCircleRect;
    private ExPoint 				mCenter;
    private ExPoint 				mScroll;
    private Context					mContext;
    private float					mScale;
    private float					mOldScale;
    private float 					mCurDist;
    private float 					mOldDist;
    private boolean 				mMultiTouch;
    private Paint 					mTotalCirclePaint;

    public PlanView(Context context) {
        this(context,null,0);
    }

    public PlanView(Context context, AttributeSet attrs) {
        this(context, attrs,0);
    }

    public PlanView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        mContext 			= context;
        mActiveArcIndex 	= ACTIVE_NONE;
        mStatus 			= STATUS_NO_ACTIVE;
        mRadius 			= 300;
        mPlan 				= new Plan();
        mMargin 			= new ExPoint(30,30);
        mScroll				= new ExPoint(0,0);
        mGestureDetector 	= new GestureDetector(this);
        mScale				= 1;
        mOldScale			= mScale;
        mMargin.magnify(mScale);

        mOldDist 			= 0;
        mCurDist			= 0;
        mMultiTouch 		= false;

        mTotalCirclePaint = new Paint();
        mTotalCirclePaint.setARGB(255, 255, 225, 225);

        calcCoordinate();
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
            canvas.drawArc(mCircleRect, convertTime2Angle(entry.startTime), (float) convertTime2Angle(entry.endTime) - convertTime2Angle(entry.startTime), true, arcPaint);
        }

        if ( mActiveArcIndex != ACTIVE_NONE ) {
            Entry entry = mPlan.get(mActiveArcIndex);
            Paint curArcLinePaint = new Paint();
            Paint curArcPointPaint = new Paint();
            curArcLinePaint.setStyle(Paint.Style.STROKE);
            curArcLinePaint.setStrokeWidth(LINE_WIDTH);
            if ( 	mStatus == STATUS_ACTIVE_START ||
                    mStatus == STATUS_ACTIVE_END) {
                curArcLinePaint.setARGB(255, 0, 0, 0);
                curArcPointPaint.setARGB(255, 0, 0, 0);
            } else {
                curArcLinePaint.setARGB(255, 128, 128, 128);
                curArcPointPaint.setARGB(255, 128, 128, 128);
            }

            canvas.drawArc(mCircleRect, convertTime2Angle(entry.startTime), (float) convertTime2Angle(entry.endTime) - convertTime2Angle(entry.startTime), true, curArcLinePaint);
            RectF rect1 = new RectF(mCenter.x, mCenter.y, mCenter.x, mCenter.y);
            rect1.inset(mScale*POINT_SIZE, mScale*POINT_SIZE);
            canvas.drawRect(rect1, curArcPointPaint);

            Point point = getPoint(entry.startTime);
            RectF rect2 = new RectF(mScale*point.x, mScale*point.y, mScale*point.x, mScale*point.y);
            rect2.inset(mScale*POINT_SIZE, mScale*POINT_SIZE);
            rect2.offset(mScale*(mMargin.x) + mScroll.x, mScale*(mMargin.y) + mScroll.y);

            canvas.drawRect(rect2, curArcPointPaint);

            point = getPoint(entry.endTime);
            RectF rect3 = new RectF(mScale*point.x, mScale*point.y, mScale*point.x, mScale*point.y);
            rect3.inset(mScale*POINT_SIZE, mScale*POINT_SIZE);
            rect3.offset(mScale*(mMargin.x) + mScroll.x, mScale*(mMargin.y) + mScroll.y);
            canvas.drawRect(rect3, curArcPointPaint);
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
                    mScale = mOldScale + mCurDist / mOldDist - 1;
                    if ( AppConfig.LOGD ) Log.d(AppConfig.TAG, "onTouchEvent() scale="+mScale);

                    calcCoordinate();
                    invalidate();
                } else if (action == (MotionEvent.ACTION_POINTER_DOWN | 0x100)) {
                    mOldDist 	= mCurDist;
                    mOldScale 	= mScale;
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
        if ( AppConfig.LOGD ) Log.d(AppConfig.TAG,"onFling()"+velocityX);
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
        mScroll.x -= distanceX;
        mScroll.y -= distanceY;
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
        float x = originX - (mRadius + mMargin.x ) * mScale - mScroll.x;
        float y = originY - (mRadius + mMargin.y ) * mScale - mScroll.y;
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
        } else {
            Toast.makeText(mContext, "out of Circle", Toast.LENGTH_LONG).show();
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
        mCircleRect.offset(mScroll.x , mScroll.y);

        mCenter.offset(mMargin.x, mMargin.y);
        mCenter.magnify(mScale);
        mCenter.offset(mScroll.x , mScroll.y);
    }

    private int convertTime2Angle(int time) {
        return (360 / 24 * time / 60 - 90);
    }

    private double getAngle(float x, float y) {
        return (360 - (Math.atan2(x , y)/Math.PI*180+180));
    }

    private Point getPoint(int time) {
        Point point = new Point();
        int angle = convertTime2Angle(time);
        int x = (int) (Math.cos(angle * Math.PI/2/90)*mRadius) + mRadius;
        int y = (int) (Math.sin(angle * Math.PI/2/90)*mRadius) + mRadius;
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
            startRect.offset(mScroll.x, mScroll.y);
            startRect.inset(TOUCH_AREA, TOUCH_AREA);

            Point endPoint = getPoint(entry.endTime);
            ExRectF endRect = new ExRectF(endPoint.x, endPoint.y, endPoint.x, endPoint.y);
            endRect.offset(mMargin.x, mMargin.y);
            endRect.magnify(mScale);
            endRect.offset(mScroll.x, mScroll.y);
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
        float x = originX - (mRadius + mMargin.x ) * mScale - mScroll.x;
        float y = originY - (mRadius + mMargin.y ) * mScale - mScroll.y;
        if ( mStatus == STATUS_ACTIVE_START ) {
            double angle = getAngle(x, y);
            Entry entry = mPlan.get(mActiveArcIndex);
            entry.startTime = (int) (angle * 4);
            if ( AppConfig.LOGD ) Log.d(AppConfig.TAG," Move angle = "+angle);
            invalidate();
        } else if (mStatus == STATUS_ACTIVE_END) {
            double angle = getAngle(x, y);
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
}

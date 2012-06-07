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

public class PlanView extends View implements OnGestureListener {

    private static final int 		ACTIVE_NONE 			= -1;

    private static final int		STATUS_NO_ACTIVE 		= 0;
    private static final int		STATUS_ACTIVE_INDEX 	= 1;
    private static final int		STATUS_ACTIVE_START		= 2;
    private static final int		STATUS_ACTIVE_END		= 3;

    private static final int		POINT_SIZE = 8;
    private static final int		TOUCH_AREA = 20;
    private static final int		LINE_WIDTH = 2;
    private final GestureDetector 	m_GestureDetector;
    private int						mStatus;
    private int 					mActiveArcIndex;
    private int 					mRadius;
    private Plan 					mPlan;
    private Point 					mMargin;
    private RectF 					mCircleRect;
    private Point 					mCenter;
    private Point 					mScroll;
    private Context					mContext;


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
        mMargin 			= new Point(30,30);
        mScroll				= new Point(0,0);
        m_GestureDetector 	= new GestureDetector(this);
        calcCoordinate();
    }

    public void calcCoordinate() {
        mCircleRect 		= new RectF(mScroll.x + mMargin.x, mScroll.y + mMargin.y, mScroll.x + 2 * mRadius + mMargin.x, mScroll.y + 2 * mRadius + mMargin.y);
        mCenter 			= new Point(mScroll.x + mMargin.x + mRadius, mScroll.y + mMargin.y + mRadius);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        Paint totalCirclePaint = new Paint();
        totalCirclePaint.setARGB(255, 255, 225, 225);
        canvas.drawArc(mCircleRect, 0, (float) (360), true, totalCirclePaint);
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
            RectF rect1 = new RectF(mCenter.x-POINT_SIZE, mCenter.y-POINT_SIZE, mCenter.x+POINT_SIZE, mCenter.y+POINT_SIZE);
            canvas.drawRect(rect1, curArcPointPaint);

            Point point = getPoint(entry.startTime);
            RectF rect2 = new RectF(mScroll.x + point.x-POINT_SIZE, mScroll.y + point.y-POINT_SIZE, mScroll.x + point.x+POINT_SIZE, mScroll.y + point.y+POINT_SIZE);

            canvas.drawRect(rect2, curArcPointPaint);

            point = getPoint(entry.endTime);
            RectF rect3 = new RectF(mScroll.x + point.x-POINT_SIZE, mScroll.y + point.y-POINT_SIZE, mScroll.x + point.x+POINT_SIZE, mScroll.y + point.y+POINT_SIZE);
            canvas.drawRect(rect3, curArcPointPaint);
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (m_GestureDetector.onTouchEvent(event))
            return true;
        int action = event.getAction();
        Log.i("kihoon.kim"," onTouchEvent() event = " + event);
        if (action == MotionEvent.ACTION_DOWN) {
            TouchDownEvent(event);
        } else if (action == MotionEvent.ACTION_UP) {
            TouchUpEvent(event);
        } else if (action == MotionEvent.ACTION_MOVE) {
            TouchMoveEvent(event);
        }

        return super.onTouchEvent(event);
    }

    public void addDefaultPlan() {
        mPlan.addDefaultPlan();
        invalidate();
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
        int x = (int) (Math.cos(angle * Math.PI/2/90)*mRadius) + mRadius + mMargin.x;
        int y = (int) (Math.sin(angle * Math.PI/2/90)*mRadius) + mRadius + mMargin.y;
        point.x = x;
        point.y = y;
        return point;
    }

    private void TouchDownEvent(MotionEvent event) {
        float x = event.getX();
        float y = event.getY();

        if ( mActiveArcIndex != ACTIVE_NONE) {
            Entry entry = mPlan.get(mActiveArcIndex);
            Point startPoint = getPoint(entry.startTime);
            RectF startRect = new RectF(mScroll.x + startPoint.x-TOUCH_AREA, mScroll.y + startPoint.y-TOUCH_AREA, mScroll.x + startPoint.x+TOUCH_AREA, mScroll.y + startPoint.y+TOUCH_AREA);
            Point endPoint = getPoint(entry.endTime);
            RectF endRect = new RectF(mScroll.x + endPoint.x-TOUCH_AREA, mScroll.y + endPoint.y-TOUCH_AREA, mScroll.x + endPoint.x+TOUCH_AREA, mScroll.y + endPoint.y+TOUCH_AREA);
            if (startRect.contains(x, y)) {
                Log.i("kihoon.kim"," startRect touch down");
                mStatus = STATUS_ACTIVE_START;
                invalidate();
                return ;
            } else if (endRect.contains(x, y)) {
                Log.i("kihoon.kim"," endRect touch down");
                mStatus = STATUS_ACTIVE_END;
                invalidate();
                return ;
            }
        }
    }
    private void TouchMoveEvent(MotionEvent event) {
        float x = event.getX();
        float y = event.getY();
        float x1 = x-mMargin.x-mRadius- mScroll.x;
        float y1 = y-mMargin.y-mRadius- mScroll.y;
        if ( mStatus == STATUS_ACTIVE_START ) {

        } else if (mStatus == STATUS_ACTIVE_END) {
            double angle = getAngle(x1, y1);
            Entry entry = mPlan.get(mActiveArcIndex);
            entry.endTime = (int) (angle * 4);
            Log.i("kihoon.kim"," Move angle = "+angle);
            invalidate();
        } else if (mStatus == STATUS_ACTIVE_INDEX) {

        }
    }

    private void TouchUpEvent(MotionEvent event) {
        if (	mStatus == STATUS_ACTIVE_START ||
                mStatus == STATUS_ACTIVE_END ) {
            mStatus = STATUS_ACTIVE_INDEX;
        }
    }

    public boolean onDown(MotionEvent arg0) {
        Log.i("kihoon.kim","onDown()");
        return false;
    }

    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
        Log.i("kihoon.kim","onFling()");
        return false;
    }

    public void onLongPress(MotionEvent e) {
        Log.i("kihoon.kim","onLongPress()");
    }

    public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
        if (	mStatus == STATUS_ACTIVE_START ||
                mStatus == STATUS_ACTIVE_END ) {
            return false;
        }
        mScroll.x -= distanceX;
        mScroll.y -= distanceY;
        calcCoordinate();
        invalidate();
        return false;
    }

    public void onShowPress(MotionEvent e) {
        Log.i("kihoon.kim","onShowPress()");
    }

    public boolean onSingleTapUp(MotionEvent e) {
        Log.i("kihoon.kim","onSingleTapUp()");
        float x = e.getX();
        float y = e.getY();
        float x1 = x-mMargin.x-mRadius-mScroll.x;
        float y1 = y-mMargin.y-mRadius-mScroll.y;
        if ( Math.pow((x1), 2) + Math.pow((y1), 2) <  Math.pow(mRadius, 2) ) {
            double angle = getAngle(x1, y1);
            Log.i("kihoon.kim","angle="+angle);
            int size = mPlan.size();
            mActiveArcIndex = ACTIVE_NONE;
            mStatus = STATUS_NO_ACTIVE;
            for (int i=0;i<size;i++) {
                Entry entry = mPlan.get(i);
                int time = (int)(angle * 4);
                if ( entry.isInTime(time) ) {
                    Log.i("kihoon.kim"," i = "+i);
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
}

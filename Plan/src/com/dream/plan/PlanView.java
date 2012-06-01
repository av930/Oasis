package com.dream.plan;

import com.dream.plan.Plan.Entry;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Toast;



public class PlanView extends View {

    class Size {
        int x;
        int y;
        Size(int X, int Y) {
            x = X;
            y = Y;
        }
    };

    private static final int 	ACTIVE_NONE = -1;
    private int 				mActiveView;
    private int 				mRadius;
    private Plan 				mPlan;
    private Size 				mMargin;
    private RectF 				mCircleRect;
    private Context				mContext;

    public PlanView(Context context) {
        this(context,null,0);
    }

    public PlanView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public PlanView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        mContext 		= context;
        // �ʱⰪ
        mActiveView 	= ACTIVE_NONE;
        mRadius 		= 350;
        mPlan 			= new Plan();
        mMargin 		= new Size(10,10);
        mCircleRect 	= new RectF( mMargin.x, mMargin.y, 2*mRadius+mMargin.x, 2*mRadius+mMargin.y);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        Paint paint = new Paint();
        paint.setARGB(255, 255, 225, 225);
        canvas.drawArc(mCircleRect, 0, (float) (360), true, paint);
        int size = mPlan.size();
        for (int i=0;i<size;i++) {
            Entry entry = mPlan.get(i);
            Paint paint2 = new Paint();
            paint2.setColor(entry.bgcolor);
            canvas.drawArc(mCircleRect, convertTime2Angle(entry.startTime), (float) convertTime2Angle(entry.endTime) - convertTime2Angle(entry.startTime), true, paint2);
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int action = event.getAction();
        if (action == MotionEvent.ACTION_DOWN) {
            float x = event.getX();
            float y = event.getY();
            float x1 = x-mMargin.x-mRadius;
            float y1 = y-mMargin.y-mRadius;

            if ( Math.pow((x1), 2) + Math.pow((y1), 2) <  Math.pow(mRadius, 2) ) {
                //Toast.makeText(mContext, "in Circle", Toast.LENGTH_LONG).show();
                //Toast.makeText(mContext, "x = "+a, Toast.LENGTH_LONG).show();
                double angle = getAngle(x1, y1);
                Log.i("kihoon.kim","angle="+angle);
                int size = mPlan.size();
                for (int i=0;i<size;i++) {
                    Entry entry = mPlan.get(i);
                    int time = (int)(angle * 4);
                    if ( entry.isInTime(time) ) {
                        Log.i("kihoon.kim"," i = "+i);
                    }
                }

            } else {
                Toast.makeText(mContext, "out of Circle", Toast.LENGTH_LONG).show();
            }

            //double a = Math.sin(Math.PI/2);
            //Toast.makeText(mContext, "a = "+a, Toast.LENGTH_LONG).show();


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

}

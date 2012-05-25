package com.dream.plan;

import com.dream.plan.Plan.Entry;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.view.View;

public class PlanView extends View {
    Plan mPlan = new Plan();

    public PlanView(Context context) {
        this(context,null,0);
    }

    public PlanView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public PlanView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        RectF rect = new RectF( 10, 10, 700, 700);
        Paint paint = new Paint();
        paint.setARGB(255, 255, 225, 225);
        canvas.drawArc(rect, 0, (float) (360), true, paint);
        int size = mPlan.size();
        for(int i=0;i<size;i++) {
        	Entry entry = mPlan.get(i);
            Paint paint2 = new Paint();
            paint2.setColor(entry.bgcolor);
        	canvas.drawArc(rect, convertTime2Angle(entry.startTime), (float) convertTime2Angle(entry.endTime) - convertTime2Angle(entry.startTime), true, paint2);        	
        }
    }

    public void addDefaultPlan() {
        mPlan.addDefaultPlan();
        invalidate();
    }
    
    private int convertTime2Angle(int time) {
    	return (360 / 24 * time / 60 - 90);
    }    
}

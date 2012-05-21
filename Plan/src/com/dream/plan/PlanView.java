package com.dream.plan;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.util.Log;
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
		// TODO Auto-generated method stub
		super.onDraw(canvas);
		Paint paint = new Paint();
		paint.setARGB(255, 255, 225, 225);
		canvas.drawCircle(380,400,300, paint);
		
	}
	
	public void addDefaultPlan() {
		mPlan.addDefaultPlan();
	}
	
}

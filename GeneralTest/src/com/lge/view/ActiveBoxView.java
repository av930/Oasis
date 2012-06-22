package com.lge.view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.View;
import android.graphics.Region.Op;

public class ActiveBoxView extends View {
    public ActiveBoxView(Context context) {
        super(context);
    }

    public ActiveBoxView(Context context, AttributeSet attrs) {
        super(context,attrs);
    }

    public ActiveBoxView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }

    @Override
    protected void onDraw(Canvas canvas) {
//    	canvas.save();
//    	canvas.clipRect(10, 10, 20, 20);

        Rect rect = new Rect(0,0,getWidth(), getHeight());
        Paint   paintRect = new Paint();
        paintRect.setColor(Color.RED);
        paintRect.setStrokeWidth(0);
        canvas.drawRect(rect, paintRect);

//    	canvas.restore();
        super.onDraw(canvas);
    }
    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        setMeasuredDimension(30, 30);
    }
}

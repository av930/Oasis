/*========================================================================
Copyright (c) 2010 by LG Cappuccino Inc.  All Rights Reserved.

			EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when		who			what, where, why
----------	----------	---------------------------------------------------
2012/06/08	kihoon.kim	cappuccino Initial release.
=========================================================================*/
package com.lge.widget;

import com.lge.generaltest.R;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.RectF;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.view.View;

public class CustomView extends View {
    private Context mContext;
    public int mAngle;
    public int mX;
    public int mY;

    public CustomView(Context context) {
        this(context,null,0);
    }

    public CustomView(Context context, AttributeSet attrs) {
        this(context, attrs,0);
    }

    public CustomView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        mContext = context;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        Drawable drawable = mContext.getResources().getDrawable(R.drawable.album_view_image);
        drawable.setBounds(0, 0,  100, 100);
        final int restoreCount = canvas.save();
        //canvas.translate(100, 100);
        canvas.rotate(mAngle, mX, mY);
        drawable.draw(canvas);
        canvas.restoreToCount(restoreCount);
        Paint paint 	= new Paint();
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(2);
        paint.setARGB(255, 0, 0, 0);
        RectF rect = new RectF(100, 100, 200, 200);
        canvas.drawRect(rect, paint);
    }
}

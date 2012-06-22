/*========================================================================
Copyright (c) 2010 by LG Cappuccino Inc.  All Rights Reserved.

			EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when		who			what, where, why
----------	----------	---------------------------------------------------
2009/12/14	kihoon.kim	cappuccino Initial release.
=========================================================================*/
package com.lge.graphictest;

import com.lge.config.AppConfig;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.util.Log;
import android.widget.ImageView;
import android.graphics.LinearGradient;
import android.graphics.Matrix;
import android.graphics.PorterDuffXfermode;
import android.graphics.PorterDuff.Mode;
import android.graphics.Rect;
import android.graphics.Shader.TileMode;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;

//------------------------------------------------------------------------
/// ReflectView
//------------------------------------------------------------------------
public class ReflectView extends ImageView {
    private Rect			mRect = new Rect();
    public ReflectView(Context context) {
        super(context);
        Init();
    }

    public ReflectView(Context context, AttributeSet attrs) {
        super(context, attrs);
        Init();
    }

    private void Init() {
    }

    @Override
    protected void onLayout(boolean changed,
                            int l, int t, int r, int b) {
        if ( AppConfig.LOGD ) Log.d(AppConfig.TAG, "====onLayout() l = "+l+", t = "+t+", r = "+r+", b = "+b);
        mRect.left = l;
        mRect.right = r;
        mRect.top = t;
        mRect.bottom = b;

    }

    @Override
    public void setImageDrawable(Drawable drawable) {

        final Matrix m = new Matrix();
        final Paint paint = new Paint();
//		final PorterDuffXfermode mode = new PorterDuffXfermode(Mode.DST_IN);
        final PorterDuffXfermode mode = new PorterDuffXfermode(Mode.DST_IN);
        final Canvas c = new Canvas();

        //recycle old bitmap.
        Drawable old = this.getDrawable();
        if (old != null && old instanceof BitmapDrawable) {
            ((BitmapDrawable)old).getBitmap().recycle();
        }

        if (drawable instanceof BitmapDrawable) {
            Bitmap bm = ((BitmapDrawable)drawable).getBitmap();
            int w = bm.getWidth();
            int h = bm.getHeight();
            Bitmap temp = Bitmap.createBitmap(w, h * 2, bm.getConfig());
            c.setBitmap(temp);
            m.reset();
            c.drawBitmap(bm, m, null);
            m.postScale(1, -1);
            m.postTranslate(0, h * 2);
            c.drawBitmap(bm, m, null);

            LinearGradient gradient = new LinearGradient(0, h, 0, h + h / 2,
                    0x70ffffff, 0x00ffffff, TileMode.CLAMP);
            paint.setShader(gradient);
            paint.setXfermode(mode);
            c.drawRect(0, h, w, h * 2, paint);
            BitmapDrawable bd = new BitmapDrawable(getContext().getResources(), temp);
            super.setImageDrawable(bd);
        } else
            super.setImageDrawable(drawable);
    }

    @Override
    public void setImageResource(int resId) {

        Drawable d = getContext().getResources().getDrawable(resId);
        if (d instanceof BitmapDrawable)
            setImageDrawable(d);
        else
            super.setImageResource(resId);
    }

}



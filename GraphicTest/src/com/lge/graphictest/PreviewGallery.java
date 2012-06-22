package com.lge.graphictest;

import android.content.Context;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.widget.Gallery;

public class PreviewGallery extends Gallery {

    public PreviewGallery(Context context) {
        super(context, null);
    }

    public PreviewGallery(Context context, AttributeSet attrs) {
        super(context,attrs, 0);
    }

    public PreviewGallery(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }

    @Override
    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
        //UI20_TDR Launcher [kihoon.kim@lge.com 101020] 감도 둔하게 조정
        //크면 클수록 둔하게 움직입니다.
        int sensitityValue = 5;
        return super.onFling(e1, e2, velocityX/sensitityValue, velocityY/sensitityValue);
    }

    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b) {
        super.onLayout(changed, l, t, r, b);
    }
}

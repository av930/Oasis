package com.lge.generaltest;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.PropertyValuesHolder;
import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.HorizontalScrollView;
import android.widget.LinearLayout;
import android.animation.LayoutTransition;
import android.animation.ObjectAnimator;

public class LayoutAnimation_Act extends Activity {
    private Button 				mButton01;
    private Button 				mButton02;
    private Button 				mButton03;
    private Button 				mButton04;
    private LinearLayout		mLayout01;
    Animator defaultAppearingAnim;
    ViewGroup container = null;

    @Override
    public void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.layoutanimation_layout);

        mLayout01 = (LinearLayout) findViewById(R.id.layout01);
        container = mLayout01;
        //container.setClipChildren(false);

        final LayoutTransition transitioner = new LayoutTransition();
        defaultAppearingAnim = transitioner.getAnimator(LayoutTransition.APPEARING);
        transitioner.setAnimator(LayoutTransition.APPEARING,  defaultAppearingAnim);
        container.setLayoutTransition(transitioner);
        mButton01 = (Button) findViewById(R.id.button01);
        mButton01.setText("appear button");
        mButton01.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                HorizontalScrollView subLayout = (HorizontalScrollView)View.inflate(getBaseContext(),
                                                 R.layout.signaltest_layout, null);
                mLayout01.addView(subLayout);
            }
        });

        mButton02 = (Button) findViewById(R.id.button02);
        mButton02.setText("not yet");
        mButton02.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
            }
        });
        mButton03 = (Button) findViewById(R.id.button03);
        mButton03.setText("not yet");
        mButton03.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
            }
        });

        mButton04 = (Button) findViewById(R.id.button04);
        mButton04.setText("not yet");
        mButton04.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
            }
        });
    }

}
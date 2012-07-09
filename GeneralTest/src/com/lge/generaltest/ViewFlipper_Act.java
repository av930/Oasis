/*========================================================================
Copyright (c) 2010 by LG Cappuccino Inc.  All Rights Reserved.

			EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when		who			what, where, why
----------	----------	---------------------------------------------------
2010/04/29	kihoon.kim	cappuccino Initial release.
=========================================================================*/
package com.lge.generaltest;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.animation.AnimationUtils;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.TextView;
import android.widget.ViewFlipper;

public class ViewFlipper_Act extends Activity implements View.OnTouchListener,
            CompoundButton.OnCheckedChangeListener {
    CheckBox checkBox;
    ViewFlipper flipper;

    // ��ġ �̺�Ʈ �߻� ������ x��ǥ ����
    float xAtDown;
    float xAtUp;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.viewflipper_layout);

        checkBox = (CheckBox)findViewById(R.id.chkAuto);
        checkBox.setOnCheckedChangeListener(this);

        flipper = (ViewFlipper)findViewById(R.id.viewFlipper);
        flipper.setOnTouchListener(this);

        // ViewFlipper�� �������� child view �߰�
        TextView tv = new TextView(this);
        tv.setText("View 4\nDynamically added");
        tv.setTextColor(Color.CYAN);
        flipper.addView(tv);
    }

    // View.OnTouchListener�� abstract method
    // flipper ���� �̺�Ʈ ������
    @Override
    public boolean onTouch(View v, MotionEvent event) {
        // ��ġ �̺�Ʈ�� �Ͼ �䰡 ViewFlipper�� �ƴϸ� return
        if (v != flipper) return false;

        if (event.getAction() == MotionEvent.ACTION_DOWN) {
            xAtDown = event.getX(); // ��ġ �������� x��ǥ ����
        } else if (event.getAction() == MotionEvent.ACTION_UP) {
            xAtUp = event.getX(); 	// ��ġ �������� x��ǥ ����

            if ( xAtUp < xAtDown ) {
                // ���� ���� ���ϸ��̼� ����
                flipper.setInAnimation(AnimationUtils.loadAnimation(this,
                                       R.anim.push_left_in));
                flipper.setOutAnimation(AnimationUtils.loadAnimation(this,
                                        R.anim.push_left_out));

                // ���� view ������
                flipper.showNext();
            } else if (xAtUp > xAtDown) {
                // ������ ���� ���ϸ��̼� ����
                flipper.setInAnimation(AnimationUtils.loadAnimation(this,
                                       R.anim.push_right_in));
                flipper.setOutAnimation(AnimationUtils.loadAnimation(this,
                                        R.anim.push_right_out));
                // �� view ������
                flipper.showPrevious();
            }
        }
        return true;
    }

    // CompoundButton.OnCheckedChangeListener�� abstract method
    // åũ�ڽ� åũ �̺�Ʈ ������
    @Override
    public void onCheckedChanged(CompoundButton view, boolean isChecked) {

        Log.w("checked", Boolean.toString(isChecked));

        if (isChecked == true) {
            // ���� ���ϸ��̼� ����
            flipper.setInAnimation(AnimationUtils.loadAnimation(this,
                                   R.anim.push_left_in));
            flipper.setOutAnimation(AnimationUtils.loadAnimation(this,
                                    R.anim.push_left_out));

            // �ڵ� Flipping ���� (���� 3��)
            flipper.setFlipInterval(3000);
            flipper.startFlipping();
        } else {
            // �ڵ� Flipping ����
            flipper.stopFlipping();
        }
    }
}
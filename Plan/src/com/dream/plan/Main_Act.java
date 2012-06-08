/*========================================================================
Copyright (c) 2010 by LG Cappuccino Inc.  All Rights Reserved.

			EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when		who			what, where, why
----------	----------	---------------------------------------------------
2012/06/08	kihoon.kim	cappuccino Initial release.
=========================================================================*/
package com.dream.plan;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class Main_Act extends Activity {
    PlanView	mPlanView;
    Button 		mAddButton;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        mPlanView 	= (PlanView)findViewById(R.id.planview);
        mAddButton	= (Button)findViewById(R.id.addbutton);
        mAddButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                mPlanView.addDefaultPlan();
            }
        });
    }
}
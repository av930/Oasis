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
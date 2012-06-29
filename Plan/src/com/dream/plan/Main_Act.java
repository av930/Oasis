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
import android.view.Menu;
import android.view.MenuItem;
import android.widget.EditText;
import android.widget.Toast;

public class Main_Act extends Activity {
    PlanView	mPlanView;
    EditText	mEditText;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        mEditText 	= (EditText)findViewById(R.id.editText);
        mPlanView 	= (PlanView)findViewById(R.id.planview);
        mPlanView.registerEditText(mEditText);
        mPlanView.restore();
    }

    @Override
    protected void onPause() {
        // TODO Auto-generated method stub
        super.onPause();
        mPlanView.save();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        String text = null;
        switch (item.getItemId()) {
        case android.R.id.home:
            text = "Application icon";
            break;
        case R.id.add:
            text = "add";
            mPlanView.addDefaultPlan();
            break;
        case R.id.edit:
            text = "edit";
            break;
        case R.id.delete:
            text = "delete";
            break;
        default:
            return false;
        }
        Toast.makeText(this, text, Toast.LENGTH_SHORT).show();
        return true;
    }
}
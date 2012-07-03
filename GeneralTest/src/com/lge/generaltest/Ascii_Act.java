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

import java.util.ArrayList;
import com.lge.config.AppConfig;
import android.app.ListActivity;
import android.os.Bundle;
import android.widget.ArrayAdapter;

public class Ascii_Act extends ListActivity {
	private String []mstr = new String[0xFFFF];
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        AppConfig.init(getBaseContext());
        ArrayList<String> alList = new ArrayList<String>();
        for (int i=0xac00;i<0xFFFF;i++) {
        	mstr[i] = new String();
        	mstr[i] = String.format("0x%x   %c", i, i);
            alList.add(mstr[i]);
        }
        this.setListAdapter(new ArrayAdapter<String>(this,
                            android.R.layout.simple_list_item_1, alList));
    }
}
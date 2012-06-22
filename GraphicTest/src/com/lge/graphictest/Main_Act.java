/*========================================================================
Copyright (c) 2010 by LG Cappuccino Inc.  All Rights Reserved.

			EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when		who			what, where, why
----------	----------	---------------------------------------------------
2010/04/29	kihoon.kim	cappuccino Initial release.
=========================================================================*/
package com.lge.graphictest;

import java.util.ArrayList;

import com.lge.config.AppConfig;
import android.app.ListActivity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;

public class Main_Act extends ListActivity {
    public static final int REQUEST_WIDGET_PREVIEW                      = 20;

    String [] mstrList = new String[] {
        "Bin data paint",
        "Bitmap Test",
        "Reflect",
        "Rotate",
        "Wica Test",
        "Widget Preview1",
        "Widget Preview2",
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        AppConfig.init(getBaseContext());
        ArrayList<String> alList = new ArrayList<String>();
        for (int i=0;i<mstrList.length;i++) {
            alList.add(mstrList[i]);
        }
        this.setListAdapter(new ArrayAdapter<String>(this,
                            android.R.layout.simple_list_item_1, alList));
    }
    @Override
    protected void onListItemClick(ListView l, View v, int position, long id) {
        int nIndex = 0;
        String strItem = (String) l.getItemAtPosition(position);
        if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, Bin_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, BitmapTest_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, Reflect_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, Rotate_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, Wica_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, WidgetPreview1_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, WidgetPreview2_Act.class);
            intent.putExtra("widgetName", "Weather");
            startActivityForResult(intent, REQUEST_WIDGET_PREVIEW);
        }
    }
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (resultCode == RESULT_OK) {
            if (requestCode==REQUEST_WIDGET_PREVIEW) {
                final int selIndex = data.getIntExtra("SelIndex", -1);
                String strResult = new String("Result = "+selIndex);
                Toast toast =  Toast.makeText(getBaseContext(), strResult, Toast.LENGTH_LONG);
                toast.show();
            }
        }
    }
}


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
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.ListView;

public class Main_Act extends ListActivity {

    String [] mstrList = new String[] {
        "Action Bar",
        "Activity Life Cycle",
        "Alert Dialog",
        "Audio",
        "Access Other Application's Resource",
        "Bitmap Leak",
        "CallStack",
        "Cursor Leak",
        "Custom View",
        "Edit Text",
        "etc...",
        "FileManager",
        "Flex",
        "FormList",
        "Heap Leak",
        "Immersive",
        "LayoutAnimation",
        "ListView - BG color",
        "Memory",
        "MovableList",
        "MusicDB",
        "ProgressBar",
        "Roller",
        "Scrollable Button",
        "Service",
        "SignalClustorView",
        "StatusBar - General",
        "StatusBar - Call",
        "Wakeup Alarm",
        "WebView",
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
            Intent intent = new Intent(Main_Act.this, ActionBar_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, ActivityLifeCycle_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, AlertDlg_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, Audio_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, ResourceExam_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, BitmapLeak_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, CallStack_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, CursorLeak_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, CustomView_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, EditText_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, Etc_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, FileManager_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, Flex_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, FormList_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, HeapLeak_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, Immersive_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, LayoutAnimation_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, ListViewBG_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, Memory_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, MovableList_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, MusicDB_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, Progressbar_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, Roller_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, ScrollableButton_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, Service_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, SignalClusterView_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, StatusBar_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, StatusBarCall_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, WakeupAlarm_Act.class);
            startActivity(intent);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent(Main_Act.this, WebView_Act.class);
            startActivity(intent);
        }
    }
}
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
import android.app.ListActivity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;
import java.util.ArrayList;
import com.lge.config.AppConfig;

public class FileManager_Act extends ListActivity {
    private static final int REQUEST_1		 			= 1;
    private static final String [] mstrList = new String[] {
        "ACTION_SELECT_DIR_FILE",
        "ACTION_SELECT_FILE",
        "ACTION_SET_TARGET_DIR",
        "ACTION_ZIP_VIEWER",
        "ACTION_ZIP_EXTRACT",
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
            Intent intent = new Intent("com.lge.action.SELECT_DIR_FILE");
            intent.putExtra("com.lge.extra.TITLE", 				"Title - Select dir file");
            intent.putExtra("com.lge.extra.GOTO_MAINPATH", 		"/mnt/sdcard/_ExternalSD/_example");
            intent.putExtra("com.lge.extra.CMDBUTTON01", 		"Button - dir file");
            startActivityForResult(intent, REQUEST_1);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent("com.lge.action.SELECT_FILE");
            intent.putExtra("com.lge.extra.TITLE", 				"Title - Select dir file");
            intent.putExtra("com.lge.extra.GOTO_MAINPATH", 		"/mnt/sdcard/_ExternalSD/_example");
            intent.putExtra("com.lge.extra.CMDBUTTON01", 		"Button - dir file");
            startActivityForResult(intent, REQUEST_1);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent("com.lge.action.SET_TARGET_DIR");
            intent.putExtra("com.lge.extra.TITLE", 				"Title - Set directory");
            intent.putExtra("com.lge.extra.GOTO_MAINPATH", 		"/mnt/sdcard/_ExternalSD/_example");
            intent.putExtra("com.lge.extra.CMDBUTTON01", 		"Button - dir");
            startActivityForResult(intent, REQUEST_1);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent("com.lge.action.ZIP_VIEWER");
            intent.putExtra("com.lge.extra.TITLE", 				"_example.zip");
            intent.putExtra("com.lge.extra.GOTO_MAINPATH", 		"/mnt/sdcard/_ExternalSD/_example/_example.zip");
            intent.putExtra("com.lge.extra.GOTO_SUBPATH", 		"/");
            intent.putExtra("com.lge.extra.CMDBUTTON01", 		"Extract");
            startActivityForResult(intent, REQUEST_1);
        } else if (strItem.equals(mstrList[nIndex++])) {
            Intent intent = new Intent("com.lge.action.ZIP_EXTRACT");
            intent.putExtra("com.lge.extra.TITLE", 				"_example.zip");
            intent.putExtra("com.lge.extra.GOTO_MAINPATH", 		"/mnt/sdcard/_ExternalSD/_example/_example.zip");
            intent.putExtra("com.lge.extra.GOTO_SUBPATH", 		"/_Total/Images");
            intent.putExtra("com.lge.extra.CMDBUTTON01", 		"Extract");
            startActivityForResult(intent, REQUEST_1);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        switch (requestCode) {
        case REQUEST_1:
            if ( resultCode == RESULT_OK && data != null ) {
                String ResultText = "";
                ResultText = ResultText + "Result Path : " + data.getStringExtra("com.lge.extra.RESULT_PATH") + "\n";
                ArrayList<String> list = data.getStringArrayListExtra("com.lge.extra.RESULT_LIST");
                if (list != null) {
                    int nNumList = list.size();
                    for ( int i=0 ; i < nNumList ; i++ ) {
                        ResultText = ResultText + i + ":"+list.get(i)+ "\n";
                    }
                }
                Toast.makeText(this, ResultText, Toast.LENGTH_SHORT).show();
            } else {
                Toast.makeText(this, "Cancel", Toast.LENGTH_SHORT).show();
            }
            break;
        }
    }
}


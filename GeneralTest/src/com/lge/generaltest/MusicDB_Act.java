package com.lge.generaltest;

import android.app.Activity;
import android.content.ContentValues;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class MusicDB_Act extends Activity {
    private Button 				mButton01;
    private Button 				mButton02;
    private Button 				mButton03;
    private Button 				mButton04;
    private Button 				mButton05;
    private Button 				mButton06;
    private static final 	Uri 	VISIBIL_URI = Uri.parse("content://com.lge.music.Setting/infos/1");
    public static final 	Uri 	CONTENT_URI = Uri.parse("content://com.lge.music.Setting/infos");

    private static final 	String 	KEY_VISIBLE = "visible";
    private static final 	String 	KEY_DISPLAY = "display";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.music_layout);

        mButton01 = (Button) findViewById(R.id.button01);
        mButton01.setText("show");
        mButton01.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Cursor c = null;
                c = getBaseContext().getContentResolver().query(VISIBIL_URI, null, null,null,null);
                if (c.moveToFirst()) {
                    String visibleValue = c.getString(c.getColumnIndex(KEY_VISIBLE));
                    if (new Boolean(visibleValue)) {

                    } else {

                    }
                    int display = c.getInt(c.getColumnIndex(KEY_DISPLAY));
                    Toast.makeText(getBaseContext(), "visibleValue = "+visibleValue+", display = "+display, Toast.LENGTH_LONG).show();
                }
                c.close();
            }
        });

        mButton02 = (Button) findViewById(R.id.button02);
        mButton02.setText("set visibleValue true");
        mButton02.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                ContentValues values = new ContentValues();
                values.put(KEY_VISIBLE, "true");
                getContentResolver().update(CONTENT_URI, values, "_id=1", null);
                Toast.makeText(getBaseContext(), "visibleValue = true", Toast.LENGTH_LONG).show();
            }
        });

        mButton03 = (Button) findViewById(R.id.button03);
        mButton03.setText("set visibleValue false");
        mButton03.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                ContentValues values = new ContentValues();
                values.put(KEY_VISIBLE, "false");
                getContentResolver().update(CONTENT_URI, values, "_id=1", null);
                Toast.makeText(getBaseContext(), "visibleValue = false", Toast.LENGTH_LONG).show();
            }
        });

        mButton04 = (Button) findViewById(R.id.button04);
        mButton04.setText("set display 1");
        mButton04.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                ContentValues values = new ContentValues();
                values.put(KEY_DISPLAY, 1);
                getContentResolver().update(CONTENT_URI, values, "_id=1", null);
                Toast.makeText(getBaseContext(), "display = 1", Toast.LENGTH_LONG).show();
            }
        });

        mButton05 = (Button) findViewById(R.id.button05);
        mButton05.setText("set display 0");
        mButton05.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                ContentValues values = new ContentValues();
                values.put(KEY_DISPLAY, 0);
                getContentResolver().update(CONTENT_URI, values, "_id=1", null);
                Toast.makeText(getBaseContext(), "display = 0", Toast.LENGTH_LONG).show();
            }
        });

        mButton06 = (Button) findViewById(R.id.button06);
        mButton06.setText("insert");
        mButton06.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                ContentValues values = new ContentValues();
                values.put(KEY_VISIBLE, "true");
                values.put(KEY_DISPLAY, 0);
                getContentResolver().insert(CONTENT_URI, values);
                Toast.makeText(getBaseContext(), "inserted", Toast.LENGTH_LONG).show();
            }
        });
    }
}
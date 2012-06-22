package com.lge.generaltest;

import android.app.Activity;
import android.database.Cursor;
import android.os.Bundle;
import android.provider.Contacts.People;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class CursorLeak_Act extends Activity {

    private static final int	MAX_CURSOR1	= 100;
    private static final int	MAX_CURSOR2	= 2000;
    private static final int	MAX_CURSOR2_UNIT1	= 20;
    private static final int	MAX_CURSOR2_UNIT2	= 100;
    private static Cursor []	mCursor1;
    private static Cursor []	mCursor2;
    private static int 			mCount = 0;

    private Button 				mButton01;
    private Button 				mButton02;

    static {
        mCursor1 = new Cursor[MAX_CURSOR1];
        mCursor2 = new Cursor[MAX_CURSOR2];
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.cursorleak_layout);
        for (int i = 0;i<MAX_CURSOR1;i++) {
            mCursor1[i] = getContentResolver().query(People.CONTENT_URI, null, null, null, null);
            mCursor1[i].moveToFirst();
        }
        Toast.makeText(getBaseContext(), "Added Count="+MAX_CURSOR1, Toast.LENGTH_SHORT).show();

        mButton01 = (Button) findViewById(R.id.button01);
        mButton01.setText("Add "+MAX_CURSOR2_UNIT1);
        mButton01.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                for (int i = 0;i<MAX_CURSOR2_UNIT1;i++) {
                    mCursor2[mCount] = getContentResolver().query(People.CONTENT_URI, null, null, null, null);
                    mCursor2[mCount].moveToFirst();
                    mCount++;
                }
                Toast.makeText(getBaseContext(), "Added Count="+MAX_CURSOR2_UNIT1+"  Total = "+(MAX_CURSOR1+mCount), Toast.LENGTH_SHORT).show();
            }
        });
        mButton02 = (Button) findViewById(R.id.button02);
        mButton02.setText("Add "+MAX_CURSOR2_UNIT2);
        mButton02.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                for (int i = 0;i<MAX_CURSOR2_UNIT2;i++) {
                    try {
                        mCursor2[mCount] = getContentResolver().query(People.CONTENT_URI, null, null, null, null);
                        mCursor2[mCount].moveToFirst();
                    } catch (NullPointerException e) {
                        Toast.makeText(getBaseContext(), "NULL exception", Toast.LENGTH_SHORT).show();
                    }
                    mCount++;
                }
                Toast.makeText(getBaseContext(), "Added Count="+MAX_CURSOR2_UNIT1+"  Total = "+(MAX_CURSOR1+mCount), Toast.LENGTH_SHORT).show();
            }
        });
    }
}
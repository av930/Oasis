package com.lge.generaltest;

import android.accounts.Account;
import android.accounts.AccountManager;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.os.StrictMode;
import android.text.format.DateFormat;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.Toast;

import java.io.FileOutputStream;
import java.util.Calendar;
import java.util.TimeZone;

public class Etc_Act extends Activity {
    private Button 				mButton01;
    private Button 				mButton02;
    private Button 				mButton03;
    private Button 				mButton04;
    private Button 				mButton05;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        if (true) {
            StrictMode.setThreadPolicy(new StrictMode.ThreadPolicy.Builder()
                                       .detectDiskReads()
                                       .detectDiskWrites()
                                       .detectNetwork()
                                       .build());
            StrictMode.setVmPolicy(new StrictMode.VmPolicy.Builder()
                                   .detectLeakedSqlLiteObjects()
                                   .penaltyLog()
                                   .build());
        }

        super.onCreate(savedInstanceState);
        setContentView(R.layout.etc_layout);

        mButton01 = (Button) findViewById(R.id.button01);
        mButton01.setText("Ringtone Picker");
        mButton01.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Intent intent = new Intent("android.intent.action.RINGTONE_PICKER");
                startActivity(intent);
            }
        });

        mButton02 = (Button) findViewById(R.id.button02);
        mButton02.setText("google log in/off");
        mButton02.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Account[] googleAccounts = AccountManager.get(getBaseContext()).getAccountsByType("com.google");
                if (googleAccounts.length > 0) {
                    Toast.makeText(getBaseContext(), "google log in", Toast.LENGTH_LONG).show();
                } else {
                    Toast.makeText(getBaseContext(), "google log off", Toast.LENGTH_LONG).show();
                }

            }
        });
        mButton03 = (Button) findViewById(R.id.button03);
        mButton03.setText("file write test");
        mButton03.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                try {
                    FileOutputStream fos = openFileOutput("test.txt", Context.MODE_WORLD_WRITEABLE);
                    String str = "Android File IO Test";
                    fos.write(str.getBytes());
                    fos.close();
                } catch (Exception e) {
                }
            }
        });

        mButton04 = (Button) findViewById(R.id.button04);
        mButton04.setText("getTimeFormatString()");
        mButton04.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Toast.makeText(getBaseContext(), getTimeFormatString(), Toast.LENGTH_LONG).show();
            }
        });
        mButton05 = (Button) findViewById(R.id.button05);
        mButton05.setText("case sensitive");
        mButton05.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                String aa = "aa";
                if (aa.equalsIgnoreCase("AA") == true) {
                    Toast.makeText(getBaseContext(), "Same", Toast.LENGTH_LONG).show();
                } else {
                    Toast.makeText(getBaseContext(), "Different", Toast.LENGTH_LONG).show();
                }

            }
        });
        
        LinearLayout linearLayout = (LinearLayout)findViewById(R.id.etc_linearlayout);
        if (true) {
            BubbleTextView btv = new BubbleTextView(getBaseContext());
            btv.setLayoutParams(new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));
            btv.setClickable(true);
            btv.setFocusable(false);

            btv.setPadding(0, 8, 0, 0);
            btv.setTextSize(15);
            btv.setTextColor(0xFFFFFFFF);
            btv.setCompoundDrawablePadding(0);
            btv.setGravity(Gravity.TOP | Gravity.CENTER_HORIZONTAL);
            btv.setMaxLines(1);

            btv.setCompoundDrawablesWithIntrinsicBounds(null, getResources().getDrawable(R.drawable.icon), null, null);

            btv.setText("test");
            linearLayout.addView(btv);
        }
    }

    public String getTimeFormatString() {
        Calendar mCalendar;
        mCalendar = Calendar.getInstance(TimeZone.getDefault());
        String result2 = DateFormat.getTimeFormat(getBaseContext()).format(mCalendar.getTime());
        int indexStart = result2.indexOf(":") - 2;
        int indexEnd = result2.indexOf(":") + 3;

        if (indexStart < 0)
            indexStart = 0;
        if (indexEnd > result2.length())
            indexEnd = result2.length() - 1;
        String result = result2.substring(indexStart, indexEnd);
        return result;
    }
}
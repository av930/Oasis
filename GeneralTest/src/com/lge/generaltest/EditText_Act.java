package com.lge.generaltest;

import android.app.Activity;
import android.graphics.Typeface;
import android.os.Bundle;
import android.widget.EditText;

import com.lge.generaltest.R;

public class EditText_Act extends Activity {
    private EditText mEditText;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.edittext_layout);
        mEditText = (EditText) findViewById(R.id.editText01);
        mEditText.setText("Hi Test");
        Typeface tf = Typeface.createFromFile("/data/data/com.jungle.app.fonts/tmp/SJplayful.ttf");
        mEditText.setTypeface(tf);
    }
}
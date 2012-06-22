package com.lge.generaltest;

import java.util.ArrayList;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Parcelable;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class Share_Act extends Activity {
    private Button 		mButton01;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.share_layout);

        Intent intent = getIntent();
        String subject = intent.getStringExtra(Intent.EXTRA_SUBJECT);
        String text = intent.getStringExtra(Intent.EXTRA_TEXT);
        ArrayList<Parcelable> attchList = intent.getParcelableArrayListExtra(Intent.EXTRA_STREAM);
        String type = intent.getType();

        Toast.makeText(getBaseContext(), "subject = "+subject+" ,text = "+text+" ,type = "+type+" ,list = ", Toast.LENGTH_LONG).show();
        String result = "";
        for (int i=0;i<attchList.size();i++) {
            String str = attchList.get(i).toString();
            result = result + str +"\n";
        }

        Toast.makeText(getBaseContext(), "result = "+result, Toast.LENGTH_LONG).show();


        mButton01 = (Button) findViewById(R.id.button01);

        mButton01.setText("test");
        mButton01.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
            }
        });

    }
}
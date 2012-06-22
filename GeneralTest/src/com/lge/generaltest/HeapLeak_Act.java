package com.lge.generaltest;

import java.util.ArrayList;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.lge.generaltest.R;
class Dummy {
    private static final int	MAX_BUFFER_SIZE	= 1024*1024;
    public byte 				mArrByte[];
    public Dummy() {
        mArrByte = new byte[MAX_BUFFER_SIZE];
    }
}
public class HeapLeak_Act extends Activity {
    private static int 				mCount = 0;
    private Button 					mButton01;
    private ArrayList<Dummy>		mList;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mList = new ArrayList<Dummy>();
        setContentView(R.layout.heapleak_layout);
        mButton01 = (Button) findViewById(R.id.button01);
        mButton01.setText("Alloc Heap");
        mButton01.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Dummy dummy = new Dummy();
                mList.add(dummy);
                mCount++;
                Toast.makeText(getBaseContext(), "alloc heap"+mCount, Toast.LENGTH_SHORT).show();
            }
        });
    }
}
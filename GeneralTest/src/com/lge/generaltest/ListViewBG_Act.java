package com.lge.generaltest;

import java.util.ArrayList;
import java.util.HashMap;

import android.app.Activity;
import android.os.Bundle;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.SimpleAdapter;

public class ListViewBG_Act extends Activity {

    private ListView mList;
    private ListAdapter mAdapter;
    private ArrayList<HashMap<String, String>>	mArrayList;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.listviewbg_layout);
        mArrayList = new ArrayList<HashMap<String, String>>();

        HashMap<String, String> m = new HashMap <String, String>();
        m.put("name", "kihoon.kim");
        m.put("job", "mobile developer");
        m.put("telephone", "010-8327-2859");
        m.put("house", "APT");
        mArrayList.add(m);

        m = new HashMap <String, String>();
        m.put("name", "bonsam.kim");
        m.put("job", "daddy");
        m.put("house", "Zxi");
        mArrayList.add(m);

        m = new HashMap <String, String>();
        m.put("name", "kuicheon.park");
        m.put("job", "mom");
        m.put("house", "Zxi");
        mArrayList.add(m);

        m = new HashMap <String, String>();
        m.put("name", "chanho.park");
        m.put("job", "baseball player");
        m.put("house", "Big house");
        mArrayList.add(m);

        m = new HashMap <String, String>();
        m.put("name", "terran");
        m.put("job", "game character");
        mArrayList.add(m);

        m = new HashMap <String, String>();
        m.put("name", "protoss");
        m.put("job", "game character");
        mArrayList.add(m);

        m = new HashMap <String, String>();
        m.put("name", "zerg");
        m.put("job", "game character");
        mArrayList.add(m);

        m = new HashMap <String, String>();
        m.put("name", "hosung.kim");
        m.put("job", "kihoon's son");
        m.put("house", "APT");
        mArrayList.add(m);

        m = new HashMap <String, String>();
        m.put("name", "nayong.kim");
        m.put("job", "kihoon's daughter");
        m.put("house", "APT");
        mArrayList.add(m);

        m = new HashMap <String, String>();
        m.put("name", "ran.jeong");
        m.put("job", "kihoon's wife");
        m.put("house", "APT");
        mArrayList.add(m);

        mList = (ListView)findViewById(R.id.list);
        mAdapter = new SimpleAdapter(this, mArrayList, R.layout.listviewbg_item, new String[] {"name","job","house"}, new int[] {R.id.text1, R.id.text2, R.id.text3});
        mList.setAdapter(mAdapter);

    }
}
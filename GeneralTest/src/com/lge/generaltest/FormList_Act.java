package com.lge.generaltest;

import com.lge.view.FormListView;

import android.app.Activity;
import android.os.Bundle;

public class FormList_Act extends Activity {
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.formlist_layout);
        FormListView listview = (FormListView)findViewById(R.id.formlist);
        //listview.setCustAdapter(getBaseContext(), FormListView.FORMLISTVIEW_STYLE1);
        //listview.add(new ListType1("Terran"));
        //listview.add(new ListType1("Protoss"));
        //listview.add(new ListType1("Zerg"));

        listview.setCustAdapter(getBaseContext(), FormListView.FORMLISTVIEW_STYLE2);
        listview.add(new ListType2("Terran", "지구인"));
        listview.add(new ListType2("Protoss", "우주인"));
        listview.add(new ListType2("Zerg", "벌레족"));
    }
}
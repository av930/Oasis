package com.lge.view;

import java.util.ArrayList;
import java.util.List;

import com.lge.generaltest.ListType1;
import com.lge.generaltest.ListType2;
import com.lge.generaltest.R;
import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

public class FormListView extends ListView {
    public static final int 		FORMLISTVIEW_NO_STYLE 	= 0;
    public static final int 		FORMLISTVIEW_STYLE1 	= 1;
    public static final int 		FORMLISTVIEW_STYLE2 	= 2;
    public static final int 		FORMLISTVIEW_STYLE3 	= 3;
    public int						mListStyle				= FORMLISTVIEW_NO_STYLE;

    private FormListViewAdapter1 	mAdapter1;
    private FormListViewAdapter2 	mAdapter2;
    private ArrayList<ListType1> 	mArrayList1;
    private ArrayList<ListType2> 	mArrayList2;

    public FormListView(Context context) {
        super(context);
    }

    public FormListView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public FormListView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }

    public void setCustAdapter(Context context, int nStyle) {
        mListStyle = nStyle;
        switch (mListStyle) {
        case FORMLISTVIEW_STYLE1:
            mArrayList1 = new ArrayList<ListType1>();
            mAdapter1 = new FormListViewAdapter1(context, R.layout.list_item_1, mArrayList1);
            setAdapter(mAdapter1);
            break;
        case FORMLISTVIEW_STYLE2:
            mArrayList2 = new ArrayList<ListType2>();
            mAdapter2 = new FormListViewAdapter2(context, R.layout.list_item_2, mArrayList2);
            setAdapter(mAdapter2);
            break;
        }
    }

    public void add(ListType1 item) {
        mArrayList1.add(item);
    }

    public void add(ListType2 item) {
        mArrayList2.add(item);
    }

    public class FormListViewAdapter1 extends ArrayAdapter<ListType1> {
        private final LayoutInflater 	mInflater;
        private final int 				mResourceId;
        public FormListViewAdapter1(Context context, int resourceId, List<ListType1> list) {
            super(context, resourceId, list);
            mInflater = LayoutInflater.from(context);
            mResourceId = resourceId;
        }

        public View getView(int position, View convertView, ViewGroup parent) {
            if (convertView == null) {
                convertView = mInflater.inflate(mResourceId, parent, false);
            }
            final ListType1 item = getItem(position);
            final TextView text = (TextView) convertView.findViewById(R.id.text1);
            text.setText(item.mString1);
            return convertView;
        }
    }

    public class FormListViewAdapter2 extends ArrayAdapter<ListType2> {
        private final LayoutInflater 	mInflater;
        private final int 				mResourceId;

        public FormListViewAdapter2(Context context, int resourceId, List<ListType2> list) {
            super(context, resourceId, list);
            mInflater = LayoutInflater.from(context);
            mResourceId = resourceId;
        }

        public View getView(int position, View convertView, ViewGroup parent) {
            if (convertView == null) {
                convertView = mInflater.inflate(mResourceId, parent, false);
            }
            final ListType2 item = getItem(position);
            final TextView text1 = (TextView) convertView.findViewById(R.id.text1);
            final TextView text2 = (TextView) convertView.findViewById(R.id.text2);
            text1.setText(item.mString1);
            text2.setText(item.mString2);
            return convertView;
        }
    }
}
package com.lge.generaltest;


import java.util.ArrayList;

import android.app.ListActivity;
import android.content.Context;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.lge.widget.QuickSettingsDragNDropView;

public class MovableList_Act extends ListActivity implements 	QuickSettingsDragNDropView.DragListener,
            QuickSettingsDragNDropView.DropListener,
            QuickSettingsDragNDropView.DoExpansionListener,
            QuickSettingsDragNDropView.MoveBlankListener {
    String [] mstrList = new String[] {
        "Terran",
        "Protoss",
        "Zerg",
        "Random",
        "Chanho Park",
        "Kihoon Kim",
        "Wonder girls",
        "LG Electrics",
        "SAMSUNG",
        "HYUNDAI",
        "BIG BANG",
        "SISTAR",
        "Secret"
    };
    QuickSettingsDragNDropView mList;
    private SettingAdapter mSettingAdapter;
    private ArrayList<Item> mItems;
    private Item moveItem = null;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.movablelist_layout);
        mItems = new ArrayList<Item>();

        for (int i = 0; i < mstrList.length; i++) {
            Item item = new Item();
            item.name = mstrList[i];
            mItems.add(item);
        }

        mList = (QuickSettingsDragNDropView)findViewById(android.R.id.list);
        mSettingAdapter = new SettingAdapter(this, R.layout.quicksettings_order_item, mItems);
        this.setListAdapter(mSettingAdapter);
        mList.setDragListener(this);
        mList.setDropListener(this);
        mList.setDoExpansionListener(this);
        mList.setMoveBlankListener(this);
        mList.setSelected(false);
        mList.setOnItemClickListener(null);
        Drawable drawable = getBaseContext().getResources().getDrawable(R.drawable.tp_select);
        mList.setSelector(drawable);
    }

    @Override
    public void moveBlank(int dragPos) {
        if (mItems.contains(null)) {
            mItems.remove(null);
            if (dragPos >= 0 && dragPos <= mItems.size()) {
                mItems.add(dragPos, null);
            }
            mSettingAdapter.notifyDataSetChanged();
        }
    }

    @Override
    public void doExpansion(int firstDragPos) {
        if (moveItem == null) {
            if (firstDragPos >= 0 && firstDragPos <= mItems.size()) {
                moveItem = mItems.remove(firstDragPos);
                mItems.add(firstDragPos, null);
                mSettingAdapter.notifyDataSetChanged();
            }
        }
    }

    @Override
    public void drop(int from, int to) {
        if (moveItem != null) {
            mItems.remove(null);
            if (to <= mItems.size()) mItems.add(to, moveItem);
            else mItems.add(mItems.size(), moveItem);
            moveItem = null;
        } else {
            if (from == to)
                return;

            Item item = mItems.remove(from);
            mItems.add(to, item);
        }
        mSettingAdapter.notifyDataSetChanged();
    }

    @Override
    public void drag(int from, int to) {
    }

    private class Item {
        public String name;
    }

    private class SettingAdapter extends ArrayAdapter<Item> {
        private ArrayList<Item> mItems;
        private Context mContext;

        public SettingAdapter(Context context, int resourceId, ArrayList<Item> items) {
            super(context, resourceId, items);
            mContext = context;
            mItems = items;
        }

        public View getView(int position, View convertView, ViewGroup parent) {
            LinearLayout layout = null;

            if ( convertView != null && convertView instanceof LinearLayout ) {
                layout = (LinearLayout)convertView;
            } else {
                layout = (LinearLayout)View.inflate(mContext, R.layout.quicksettings_order_item, null);
            }

            Item item = mItems.get(position);
            if (item != null) {
                ((TextView) layout.findViewById(R.id.quicksettings_setting_item_name)).setVisibility(View.VISIBLE);
                ((ImageView) layout.findViewById(R.id.quicksettings_setting_item_move)).setVisibility(View.VISIBLE);

                TextView name = (TextView)layout.findViewById(R.id.quicksettings_setting_item_name);
                name.setText(item.name);
            } else {
                ((TextView) layout.findViewById(R.id.quicksettings_setting_item_name)).setVisibility(View.INVISIBLE);
                ((ImageView) layout.findViewById(R.id.quicksettings_setting_item_move)).setVisibility(View.INVISIBLE);
            }

            return layout;
        }
    }
}
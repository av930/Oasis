/*========================================================================
Copyright (c) 2010 by LG Cappuccino Inc.  All Rights Reserved.

			EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when		who			what, where, why
----------	----------	---------------------------------------------------
2012/06/08	kihoon.kim	cappuccino Initial release.
=========================================================================*/
package com.dream.plan;

import java.io.File;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.Iterator;
import java.util.Random;
import java.util.StringTokenizer;

import android.content.Context;
import android.content.SharedPreferences;
import android.graphics.Color;

public class Plan {

    public class Entry {
        public int 		startTime;
        public int 		endTime;
        public String 	title;
        public int		bgcolor;
        public Entry() {
        }

        public Entry(int s, int e, String t, int b) {
            startTime 	= s;
            endTime   	= e;
            title     	= t;
            bgcolor 	= b;
        }

        public boolean isInTime(int time) {
            if ( startTime < endTime) {
                if ( startTime < time && time < endTime) {
                    return true;
                }
            } else {
                if ( startTime <= time || time <= endTime) {
                    return true;
                }
            }
            return false;
        }

        @Override
        public String toString() {
            return "startTime = " + startTime + ", endTime = "+endTime+", title ="+title;
        }
    }
    
    private final String 	SHARED_PREFS_NAME 	= "plan";
    private final String 	PLAN_VERSION_STRING = "version";
    private final String 	PLAN_LIST_STRING 	= "list";
    private final int 		PLAN_VERSION_NONE 	= 0;
    private final int 		PLAN_VERSION 		= 1;

    private final int 		OBJECT_NUMBER		= 7;
    private int[] 			mArrBlockColor;

    public Random 					mRnd 		= new Random();
    private final ArrayList<Entry> 	mEntries 	= new ArrayList<Entry>();
    private Context 				mContext;

    private final Comparator<Entry> mEntryCmp 	= new Comparator<Entry>() {
        public int compare(Entry a, Entry b) {
            final int na = a.startTime;
            final int nb = b.startTime;
            return (na-nb);
        }
    };

    public Plan(Context context) {
        mContext = context;
        init();
    }

    public void init() {
        mArrBlockColor 		= new int[OBJECT_NUMBER];
        mArrBlockColor[0] 	= Color.rgb(255, 230, 230);
        mArrBlockColor[1] 	= Color.rgb(230, 255, 230);
        mArrBlockColor[2] 	= Color.rgb(230, 230, 255);
        mArrBlockColor[3] 	= Color.rgb(255, 255, 230);
        mArrBlockColor[4] 	= Color.rgb(255, 230, 255);
        mArrBlockColor[5] 	= Color.rgb(230, 255, 255);
        mArrBlockColor[6] 	= Color.rgb(230, 230, 230);
    }

    public int getLastTime() {
        int N = mEntries.size();
        if ( N > 0 )
            return mEntries.get(N-1).endTime;
        return 0;
    }

    public void addDefaultPlan() {
        int lastTime = getLastTime();
        int c = mRnd.nextInt(OBJECT_NUMBER);

        Entry entry = new Entry(lastTime, lastTime+60, "", mArrBlockColor[c]);
        int i;
        int N = mEntries.size();
        for (i=0; i<N; i++) {
            if (mEntryCmp.compare(mEntries.get(i), entry) > 0) {
                break;
            }
        }

        mEntries.add(i, entry);
    }

    public int size() {
        return mEntries.size();
    }

    public Entry get(int i) {
        return mEntries.get(i);
    }

    public void save() {
        String data = "";
        SharedPreferences.Editor editor = mContext.getSharedPreferences(SHARED_PREFS_NAME, Context.MODE_PRIVATE).edit();
        Iterator<Entry> iterator = mEntries.iterator();
        while (iterator.hasNext() ) {
            Entry entry = iterator.next();
            String items = "";
            items += entry.startTime 	+ ":";
            items += entry.endTime 		+ ":";
            if(entry.title.equals("")) {
            	items += "@" 			+ ":";
            } else {
            	items += entry.title 	+ ":";
            }
            items += entry.bgcolor 		+ "";
            items += "|";
            data += items;
        }
        editor.putInt(PLAN_VERSION_STRING, PLAN_VERSION);
        editor.putString(PLAN_LIST_STRING, data);
        editor.commit();

    }

    public void restore() {
        // preference °ª ÀÐ¾î¿È
        SharedPreferences preferences = mContext.getSharedPreferences(SHARED_PREFS_NAME, Context.MODE_PRIVATE);
        File preferencesFile;

        try {
            preferencesFile = new File("/data/data/com.dream.plan/shared_prefs/"+SHARED_PREFS_NAME+".xml");
            if (!preferencesFile.exists())
                return;
            int version = preferences.getInt(PLAN_VERSION_STRING, PLAN_VERSION_NONE);
            if (version != PLAN_VERSION) {
                preferencesFile.delete();
                return ;
            }
        } catch (Exception e) {
        }

        String data = preferences.getString(PLAN_LIST_STRING, "");
        StringTokenizer tokenizer = new StringTokenizer(data, "|");

        while ( tokenizer.hasMoreElements() ) {
            updateItem(tokenizer.nextToken());
        }
    }

    private void updateItem(String data) {
        if (data == null) return;

        StringTokenizer tokenizer = new StringTokenizer(data, ":");

        int startTime = Integer.parseInt(tokenizer.nextToken());
        int endTime = Integer.parseInt(tokenizer.nextToken());
        String title = tokenizer.nextToken();
        if(title.equals("@")) {
        	title = "";
        }
        int bgcolor = Integer.parseInt(tokenizer.nextToken());

        Entry entry = new Entry(startTime, endTime, title, bgcolor);
        mEntries.add(entry);
    }
}

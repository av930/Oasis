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

import java.util.ArrayList;
import java.util.Comparator;
import java.util.Random;

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
            if ( startTime < time && time < endTime) {
                return true;
            }
            return false;
        }

        @Override
        public String toString() {
            return "startTime = " + startTime + ", endTime = "+endTime+", title ="+title;
        }
    }
    private final int 	OBJECT_NUMBER			= 7;
    private int[] 		mArrBlockColor;

    public Random mRnd 			= new Random();
    private final ArrayList<Entry> mEntries = new ArrayList<Entry>();
    private final Comparator<Entry> mEntryCmp = new Comparator<Entry>() {
        public int compare(Entry a, Entry b) {
            final int na = a.startTime;
            final int nb = b.startTime;
            return (na-nb);
        }
    };

    public Plan() {
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

        Entry entry = new Entry(lastTime, lastTime+60, "default", mArrBlockColor[c]);
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
}

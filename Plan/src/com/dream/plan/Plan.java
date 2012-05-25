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

        @Override
        public String toString() {
            return "startTime = " + startTime + ", endTime = "+endTime+", title ="+title;
        }
    }
    private final int OBJECT_NUMBER			= 7;
    private int[] 			mArrBlockColor;

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
        mArrBlockColor[0] 	= Color.BLUE;
        mArrBlockColor[1] 	= Color.GREEN;
        mArrBlockColor[2] 	= Color.RED;
        mArrBlockColor[3] 	= Color.GRAY;
        mArrBlockColor[4] 	= Color.rgb(255, 255, 0);
        mArrBlockColor[5] 	= Color.rgb(255, 0, 255);
        mArrBlockColor[6] 	= Color.rgb(0, 255, 255);
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
}

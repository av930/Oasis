package com.dream.plan;

import java.util.ArrayList;
import java.util.Comparator;

import android.util.Log;

public class Plan {

	public class Entry {
		public int 		startTime;
		public int 		endTime;
		public String 	title;		

		public Entry() {			
		}
		
		public Entry(int s, int e, String t) {
			startTime = s;
			endTime   = e;
			title     = t;			
		}

		@Override
		public String toString() {
			return "startTime = " + startTime + ", endTime = "+endTime+", title ="+title;
		}	
	}
	private final ArrayList<Entry> mEntries = new ArrayList<Entry>();
    private final Comparator<Entry> mEntryCmp = new Comparator<Entry>() {
        public int compare(Entry a, Entry b) {
            final int na = a.startTime;
            final int nb = b.startTime;            
            return (na-nb);
        }
    };

	public int getLastTime() {
		int N = mEntries.size();
		if( N > 0 )			
			return mEntries.get(N-1).endTime;
		return 0;
	}
	
	public void addDefaultPlan() {
		int lastTime = getLastTime();		
		Entry entry = new Entry(lastTime, lastTime+60, "default");
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

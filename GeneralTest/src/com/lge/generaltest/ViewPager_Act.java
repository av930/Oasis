/*========================================================================
Copyright (c) 2010 by LG Cappuccino Inc.  All Rights Reserved.

			EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when		who			what, where, why
----------	----------	---------------------------------------------------
2010/04/29	kihoon.kim	cappuccino Initial release.
=========================================================================*/
package com.lge.generaltest;


import android.app.Activity;
import android.content.res.Resources;
import android.os.Bundle;
import android.os.Parcelable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.lge.view.PagerAdapter;
import com.lge.view.ViewPager;

public class ViewPager_Act extends Activity {
    private ViewPager mPager;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.viewpager_layout);
        mPager = (ViewPager) findViewById(R.id.panelswitch);
        if (mPager != null) {
            mPager.setAdapter(new PageAdapter(mPager));
        }
    }

    class PageAdapter extends PagerAdapter {
        private View mSimplePage;
        private View mAdvancedPage;
        public PageAdapter(ViewPager parent) {
            final LayoutInflater inflater = LayoutInflater.from(parent.getContext());
            final View simplePage = inflater.inflate(R.layout.actionbar_layout, parent, false);
            final View advancedPage = inflater.inflate(R.layout.roller_layout, parent, false);
            mSimplePage = simplePage;
            mAdvancedPage = advancedPage;
        }

        @Override
        public int getCount() {
            return 2;
        }

        @Override
        public void startUpdate(View container) {
        }

        @Override
        public Object instantiateItem(ViewGroup container, int position) {
            final View page = position == 0 ? mSimplePage : mAdvancedPage;
            (container).addView(page);
            return page;
        }

        @Override
        public void destroyItem(ViewGroup container, int position, Object object) {
            ( container).removeView((View) object);
        }

        @Override
        public void finishUpdate(View container) {
        }

        @Override
        public boolean isViewFromObject(View view, Object object) {
            return view == object;
        }

        @Override
        public Parcelable saveState() {
            return null;
        }

        @Override
        public void restoreState(Parcelable state, ClassLoader loader) {
        }
    }
}
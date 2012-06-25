package com.lge.widget;

import com.lge.config.AppConfig;
import com.lge.generaltest.R;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewConfiguration;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.os.Handler;
import android.widget.AbsListView.OnScrollListener;

public class QuickSettingsDragNDropView extends ListView {
    private Context mContext;
    private ImageView mDragView;
    private WindowManager mWindowManager;
    private WindowManager.LayoutParams mWindowParams;
    private int mDragPos;
    private int mFirstDragPos;
    private int mDragPoint;
    private int mCoordOffset;
    private DragListener mDragListener;
    private DropListener mDropListener;
    private DoExpansionListener mDoExpansionListener;
    private MoveBlankListener mMoveBlankListener;
    private int mUpperBound;
    private int mLowerBound;
    private int mHeight;
    private Rect mTempRect = new Rect();
    private Bitmap mDragBitmap;
    private final int mTouchSlop;
    private int mItemHeightNormal;
    private int mItemHeightExpanded;
    private int dndViewId;
    private int dragImageX;

    private int mScrollState = OnScrollListener.SCROLL_STATE_IDLE;
    private Handler mScrollHander = new Handler();
    private ScrollRunnable mScrollRunnable;

    private int mDiff = 0;

    class ScrollRunnable implements Runnable {
        private int y;

        public ScrollRunnable(int y) {
            this.y = y;
        }

        @Override
        public void run() {
            if (scroll(y)) {
                mScrollHander.removeCallbacks(mScrollRunnable);
                int itemnum = pointToPosition(0, y);
                if (itemnum >= 0) {
                    mDragPos = itemnum;
                } else {
                    if (y < mUpperBound) {
                        View view = getChildAt(0);
                        if (view != null) {
                            mDragPos = getPositionForView(view);
                            Log.v("QuickSettingsDragNDropView","ScrollRunnable:run : mDragPos = " + mDragPos);
                        }
                    }
                }
                if (mMoveBlankListener != null) {
                    mMoveBlankListener.moveBlank(mDragPos);
                }
                mScrollRunnable = new ScrollRunnable(y);
                mScrollHander.postDelayed(mScrollRunnable, 30);
            } else {
                mScrollHander.removeCallbacks(mScrollRunnable);
            }
        }
    }

    public QuickSettingsDragNDropView(Context context, AttributeSet attrs) {
        super(context, attrs);
        mContext = context;
        mTouchSlop = ViewConfiguration.get(context).getScaledTouchSlop();
    }

    @Override
    public boolean onInterceptTouchEvent(MotionEvent ev) {
        if (mDragListener != null || mDropListener != null) {
            switch (ev.getAction()) {
            case MotionEvent.ACTION_DOWN:
                int x = (int) ev.getX();
                int y = (int) ev.getY();
                int itemnum = pointToPosition(x, y);

                if (itemnum == AdapterView.INVALID_POSITION)
                    break;

                ViewGroup item = (ViewGroup) getChildAt(itemnum - getFirstVisiblePosition());

                if (item == null)
                    break;

                mItemHeightNormal = item.getHeight();
                mItemHeightExpanded = mItemHeightNormal * 2;

                mDragPoint = y - item.getTop();
                mCoordOffset = ((int)ev.getRawY()) - y;
                View dragger = item.findViewById(dndViewId);

                if (dragger == null)
                    dragger = item;

                Rect r = mTempRect;
                dragger.getDrawingRect(r);

                mDiff = (r.bottom + r.top)/2 - mDragPoint;

                //CAPP_RTL
                boolean isDragDrop  = (x > item.getRight() - 90);
                if (isDragDrop) {
                    //CAPP_RTL_END
                    item.setDrawingCacheEnabled(true);

                    try {
                        {
                            Bitmap bitmap = Bitmap.createBitmap(item.getDrawingCache());
                            startDragging(bitmap, y);
                        }
                    } catch (Exception ex) {
                        Log.e(AppConfig.TAG, ex.toString());
                    }

                    mDragPos = itemnum;
                    mFirstDragPos = mDragPos;
                    mHeight = getHeight();
                    int touchSlop = mTouchSlop;
                    mUpperBound = Math.min(y - touchSlop, mHeight / 3);
                    mLowerBound = Math.max(y + touchSlop, mHeight * 2 /3);
                    return false;
                }

                mDragView = null;
                stopDragging();
                //return true;
                break;
            }
        }

        return super.onInterceptTouchEvent(ev);
    }

    @Override
    public boolean onTouchEvent(MotionEvent ev) {
        if ((mDragListener != null || mDropListener != null) && mDragView != null) {
            if (mScrollRunnable != null) {
                mScrollHander.removeCallbacks(mScrollRunnable);
                mScrollRunnable = null;
            }

            int action = ev.getAction();

            switch (action) {
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_CANCEL:
                Rect r = mTempRect;
                mDragView.getDrawingRect(r);
                stopDragging();

                if (mDropListener != null && mDragPos >= 0 && mDragPos < getCount()) {
                    mDropListener.drop(mFirstDragPos, mDragPos);
                }

                unExpandViews(true);
                break;

            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_MOVE:
                int x = (int) ev.getX();
                int y = (int) ev.getY();

                dragView(x, y);

                int itemnum = myPointToPosition(0, y+mDiff);

                if (itemnum >= 0) {
                    if (action == MotionEvent.ACTION_DOWN || itemnum != mDragPos) {
                        if (mDragListener != null) {
                            mDragListener.drag(mDragPos, itemnum);
                        }

                        mDragPos = itemnum;
                        if (action == MotionEvent.ACTION_DOWN) {
                            if (mDoExpansionListener != null) {
                                mDoExpansionListener.doExpansion(mFirstDragPos);
                            }
                        }
                    }

                    if (mMoveBlankListener != null) {
                        mMoveBlankListener.moveBlank(mDragPos);
                    }
                } else {
                    if (y < mUpperBound) {
                        View view = getChildAt(0);
                        if (view != null) {
                            mDragPos = getPositionForView(view);
                            Log.v("QuickSettingsDragNDropView","ScrollRunnable:run : mDragPos = " + mDragPos);
                        }
                    }
                    if (mMoveBlankListener != null) {
                        mMoveBlankListener.moveBlank(mDragPos);
                    }
                }

                if (scroll(y)) {
                    mScrollRunnable = new ScrollRunnable(y);
                    mScrollHander.postDelayed(mScrollRunnable, 30);
                }
                break;
            }

            return true;
        }

        return super.onTouchEvent(ev);
    }

    private int myPointToPosition(int x, int y) {
        Rect frame = mTempRect;
        final int count = getChildCount();

        for (int i = count - 1; i >= 0; i--) {
            final View child = getChildAt(i);

            if (child != null) {
                child.getHitRect(frame);

                if (frame.contains(x, y)) {
                    return getFirstVisiblePosition() + i;
                }
            }
        }

        return INVALID_POSITION;
    }

    private void adjustScrollBounds(int y) {
        if (y >= mHeight / 3) {
            mUpperBound = mHeight / 3;
        }

        if (y <= mHeight * 2 / 3) {
            mLowerBound = mHeight * 2 / 3;
        }
    }

    private boolean scroll(int y) {
        int speed = 0;
        adjustScrollBounds(y);

        if (y > mLowerBound) {
            speed = y > (mHeight + mLowerBound) / 2 ? 30 : 4;

            if (getLastVisiblePosition() == (getCount() -1)) {
                int adjustedy = y - mDragPoint;
                View view = getChildAt(getChildCount() -1);
                if (view != null) {
                    if (view.getTop() < adjustedy) {
                        speed = 0;
                    } else if (view.getTop() < (adjustedy + 10)) {
                        speed = 4;
                    }
                }
            }
        } else if (y < mUpperBound) {
            speed = y < mUpperBound / 2 ? -30 : -4;
            View view = getChildAt(0);

            if (getFirstVisiblePosition() == 0 && view != null && view.getTop() >= getPaddingTop()) {
                speed = 0;
            }
        }

        if (speed != 0) {
            if (mDragPos >= (getCount() - 2)) {
                smoothScrollBy(speed, 350);
            } else {
                smoothScrollBy(speed, 100);
            }
            return true;
        }
        return false;
    }

    public void setScrollState(int state) {
        mScrollState = state;
    }

    private void unExpandViews(boolean deletion) {
        for (int i = 0;; i++) {
            View v = getChildAt(i);

            if (v == null) {
                if (deletion) {
                    int position = getFirstVisiblePosition();
                    int y = 0;
                    View temp = getChildAt(0);

                    if (temp != null)
                        y = temp.getTop();

                    setAdapter(getAdapter());
                    setSelectionFromTop(position, y);
                }

                layoutChildren();
                v = getChildAt(i);

                if (v == null) {
                    //break;
                    return;
                }
            }

            ViewGroup.LayoutParams params = v.getLayoutParams();
            params.height = mItemHeightNormal;
            v.setLayoutParams(params);
            v.setVisibility(View.VISIBLE);
        }
    }

    private void startDragging(Bitmap bm, int y) {
        stopDragging();

        mWindowParams = new WindowManager.LayoutParams();
        mWindowParams.gravity = Gravity.TOP;
        mWindowParams.x = dragImageX;
        mWindowParams.y = y - mDragPoint + mCoordOffset;

        mWindowParams.height = (int)((69.33f * mContext.getResources().getDisplayMetrics().density) + 0.5f);
        mWindowParams.width = WindowManager.LayoutParams.WRAP_CONTENT;
        mWindowParams.flags = WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE
                              | WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE
                              | WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON
                              | WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN
                              | WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS;
        mWindowParams.format = PixelFormat.TRANSLUCENT;
        mWindowParams.windowAnimations = 0;

        if ( mContext != null ) {
            ImageView v = new ImageView(mContext);
            v.setBackgroundResource(R.drawable.list_movable);
            v.setPadding(0, 0, 0, 0);
            v.setImageBitmap(bm);
            mDragBitmap = bm;

            mWindowManager = (WindowManager)mContext.getSystemService(Context.WINDOW_SERVICE);
            mWindowManager.addView(v, mWindowParams);
            mDragView = v;
        }
    }

    private void dragView(int x, int y) {
        mWindowParams.y = y - mDragPoint + mCoordOffset;
        int bottonOffset = getHeight() + mCoordOffset - mDragView.getHeight()/2;
        if (mWindowParams.y > bottonOffset) {
            mWindowParams.y = bottonOffset;
        }
        mWindowManager.updateViewLayout(mDragView, mWindowParams);
    }

    private void stopDragging() {
        if (mDragView != null) {
            mDragView.setVisibility(GONE);
            WindowManager wm = (WindowManager)mContext.getSystemService(Context.WINDOW_SERVICE);
            wm.removeView(mDragView);
            mDragView.setImageDrawable(null);
            mDragView = null;
        }

        if (mDragBitmap != null) {
            mDragBitmap.recycle();
            mDragBitmap = null;
        }
    }

    public void setDragListener(DragListener l) {
        mDragListener = l;
    }

    public void setDropListener(DropListener l) {
        mDropListener = l;
    }

    public void setDndView(int id) {
        dndViewId = id;
    }

    public void setDragImageX(int x) {
        dragImageX = x;
    }

    public void setDoExpansionListener(DoExpansionListener l) {
        mDoExpansionListener = l;
    }

    public void setMoveBlankListener(MoveBlankListener l) {
        mMoveBlankListener = l;
    }

    public interface DragListener {
        void drag(int from, int to);
    }

    public interface DropListener {
        void drop(int from, int to);
    }

    public interface DoExpansionListener {
        public void doExpansion(int firstDragPos);
    }

    public interface MoveBlankListener {
        public void moveBlank(int dragPos);
    }
}

package com.lge.wica;
import com.lge.config.AppConfig;

import android.util.Log;
public class Wica {
    private static final String TAG = "WICA";
    private int mHandle;
    static {
        if ( AppConfig.LOGD ) Log.i(TAG, "loadLibrary Wica");
        System.loadLibrary("wica");
    }

    public Wica() {
        if ( AppConfig.LOGD ) Log.i(TAG, "Wica()");
        mHandle = 0;
    }

    public boolean create(String strFileName) {
        if ( AppConfig.LOGD ) Log.i(TAG, "create()");
        mHandle = create_jni(strFileName);
        if ( AppConfig.LOGD ) Log.i(TAG, "create() mHandle = " + mHandle);
        if (mHandle == 0)
            return false;
        return true;
    }

    public int getFrameNum() {
        return getFrameNum_jni(mHandle);
    }

    public int getWidth() {
        return getWidth_jni(mHandle);
    }

    public int getHeight() {
        return getHeight_jni(mHandle);
    }

    public int[] decode(int numFrame) {
        if ( AppConfig.LOGD ) Log.i(TAG, "decode() numFrame = " + numFrame);
        return decode_jni(mHandle, numFrame);
    }

    public boolean setAlpha(int alpha) {
        return setAlpha_jni(mHandle, alpha);
    }

    public boolean destroy() {
        return destroy_jni(mHandle);
    }

    private static native int		create_jni(String fileName);
    private static native int       getFrameNum_jni(int handle);
    private static native int       getWidth_jni(int handle);
    private static native int       getHeight_jni(int handle);
    private static native int[]     decode_jni(int handle, int numFrame);
    private static native boolean   setAlpha_jni(int handle, int alpha);
    private static native boolean   destroy_jni(int handle);
}

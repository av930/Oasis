package com.lge.generaltest;
import android.util.Log;
public class MemoryNative {
    private static final String TAG = "MemoryNative";
    static {
        Log.i(TAG, "loadLibrary MemoryNative");
        System.loadLibrary("memorynative");
    }

    public MemoryNative() {
        Log.i(TAG, "MemoryNative()");
    }

    public static void test1() {
        test1_jni();
    }
    public static void test2() {
        test2_jni();
    }

    private static native void test1_jni();
    private static native void test2_jni();
}

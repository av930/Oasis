/*========================================================================
Copyright (c) 2010 by LG Cappuccino Inc.  All Rights Reserved.

			EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when		who			what, where, why
----------	----------	---------------------------------------------------
2010/09/13	kihoon.kim	cappuccino Initial release.
=========================================================================*/
package com.lge.config;

import android.content.Context;
import android.graphics.Point;
import android.view.Display;
import android.view.WindowManager;

//------------------------------------------------------------------------
/// AppConfig
//------------------------------------------------------------------------
public class AppConfig {
    //-------------------------------------------------------------------------
    // Member Variables
	public 	static final String		TAG				= "kihoon.kim";
	public 	static final boolean	LOGD			= true;
	public 	static final boolean 	DEBUG 			= true;

    public 	static int 				mLongAxis;
    public 	static int 				mShortAxis;
    public 	static int 				mWidth;
    public 	static int 				mHeight;
    public 	static Point			mCenter 		= new Point();    
     
    public static void init(Context context) {
        Display d 	= ((WindowManager)context.getApplicationContext().getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        int nWidth 	= d.getWidth();
        int nHeight = d.getHeight();
         
        if (nHeight >  nWidth) {
            mLongAxis 	= nHeight;
            mShortAxis 	= nWidth;
        } else {
            mLongAxis 	= nWidth;
            mShortAxis 	= nHeight;
        }
        	
        mCenter.x 	= nWidth / 2;
        mCenter.y 	= nHeight / 2;
        mWidth 		= nWidth;
        mHeight 	= nHeight;    
    }
}
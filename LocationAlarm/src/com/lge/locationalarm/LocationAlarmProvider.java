/*========================================================================
Copyright (c) 2010 by LG Cappuccino Inc.  All Rights Reserved.

			EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when		who			what, where, why
----------	----------	---------------------------------------------------
2011/04/19	kihoon.kim	cappuccino Initial release.
=========================================================================*/
package com.lge.locationalarm;

import android.content.*;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.net.Uri;
import android.util.Log;
import com.lge.config.AppConfig;

public class LocationAlarmProvider extends ContentProvider {
    private static final String 		DATABASE_NAME 		= "locationalarm.db";
    private LocationAlarmOpenHelper 	mOpenHelper;
    private static SQLiteDatabase 		mDB;

    @Override
    public boolean onCreate() {
        mOpenHelper 	= new LocationAlarmOpenHelper(getContext());
        return true;
    }

    @Override
    public Cursor query(Uri uri, String[] projection, String selection,
                        String[] selectionArgs, String sort) {
        String 	strTable = uri.getPathSegments().get(0);
        mDB = mOpenHelper.getReadableDatabase();
        Cursor c = mDB.query(strTable, projection, selection, selectionArgs, null, null, null);
        c.setNotificationUri(getContext().getContentResolver(), uri);
        return c;
    }

    @Override
    public int delete(Uri uri, String arg1, String[] arg2) {
        String 	strTable = uri.getPathSegments().get(0);
        mDB = mOpenHelper.getWritableDatabase();
        return mDB.delete(strTable, arg1, arg2);
    }

    @Override
    public String getType(Uri uri) {
        return null;
    }

    @Override
    public Uri insert(Uri uri, ContentValues values) {
        long 	rowID;
        String 	strTable = uri.getPathSegments().get(0);
        mDB 	= mOpenHelper.getWritableDatabase();
        rowID	= mDB.insert(strTable, null, values);
        if (rowID <= 0)
            return null;

        Uri returi=ContentUris.withAppendedId(uri, rowID);
        getContext().getContentResolver().notifyChange(uri, null);
        return returi;
    }

    @Override
    public int update(Uri uri, ContentValues values, String selection,
                      String[] selectionArgs) {
        String 	strTable = uri.getPathSegments().get(0);
        mDB = mOpenHelper.getWritableDatabase();
        return mDB.update(strTable, values, selection,selectionArgs);
    }

    class LocationAlarmOpenHelper extends SQLiteOpenHelper {
        private static final int	 	VERSION 			= 1;
        private static final String	 	TABLE_LOCATIONALARMINFO	= "locationalarminfo";

        public LocationAlarmOpenHelper(Context context) {
            super(context, DATABASE_NAME, null, VERSION);
            Log.w(AppConfig.TAG, "LocationAlarmOpenHelper");
        }

        @Override
        public void onCreate(SQLiteDatabase db) {
            Log.w(AppConfig.TAG, "onCreate DB Version = "+db.getVersion());
            Cursor c1=db.rawQuery("SELECT name FROM sqlite_master WHERE type='table' AND name='"+TABLE_LOCATIONALARMINFO+"'", null);
            try {
                if (c1.getCount()==0) {
                    db.execSQL("CREATE TABLE "+TABLE_LOCATIONALARMINFO+"(_id INTEGER PRIMARY KEY AUTOINCREMENT, locationname TEXT, enable int);");
                    db.execSQL("INSERT INTO " + TABLE_LOCATIONALARMINFO + " VALUES(null,'대륭 6차',0);");
                    db.execSQL("INSERT INTO " + TABLE_LOCATIONALARMINFO + " VALUES(null,'우리집',0);");
                }
            } finally {
                c1.close();
            }
        }

        @Override
        public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
            Log.w(AppConfig.TAG, "Upgrading database - current DB Version = "+db.getVersion()+" , oldVersion = "+oldVersion+" , newVersion = "+newVersion);
        }
    }
}
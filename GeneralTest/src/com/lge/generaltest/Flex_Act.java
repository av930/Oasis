package com.lge.generaltest;

import java.util.ArrayList;
import com.lge.config.AppConfig;
import android.app.ListActivity;
import android.content.ContentResolver;
import android.content.ContentValues;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;

public class Flex_Act extends ListActivity {
    String [] mstrList = new String[] {
        "getDefaultValues(EMAILPRECONFIGDB_TITLE_I_0)",
        "getUserValues(EMAILPRECONFIGDB_TITLE_I_0)",
        "setUserValues(EMAILPRECONFIGDB_TITLE_I_0,±è³ª¿µ)",
        "setUserValues(EMAILPRECONFIGDB_TITLE_I_0,±èÈ£¼º)",
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        ArrayList<String> alList = new ArrayList<String>();
        for (int i=0;i<mstrList.length;i++) {
            alList.add(mstrList[i]);
        }
        this.setListAdapter(new ArrayAdapter<String>(this,
                            android.R.layout.simple_list_item_1, alList));
    }

    @Override
    protected void onListItemClick(ListView l, View v, int position, long id) {
        int nIndex = 0;
        String strItem = (String) l.getItemAtPosition(position);
        String 	AUTHORITY 	= "flex";
        Uri 	CONTENT_URI = Uri.parse("content://" + AUTHORITY + "/flexinfo");
        ContentResolver cr 	= getBaseContext().getContentResolver();
        Cursor c = null;
        if (strItem.equals(mstrList[nIndex++])) {
            try {
                c = cr.query(CONTENT_URI,  new String[] {"default_values"},
                             "flex_name=?", new String[] {"EMAILPRECONFIGDB_TITLE_I_0"}, null);
                if ( c.moveToNext() ) {
                    Toast.makeText(getBaseContext(), c.getString(0), Toast.LENGTH_LONG).show();
                }
            } catch (RuntimeException e) {
                Log.e(AppConfig.TAG, "getFlexValue() :>>"+ e );
            } finally {
                if ( c != null ) {
                    c.close();
                }
            }
        } else if (strItem.equals(mstrList[nIndex++])) {
            try {
                c = cr.query(CONTENT_URI,  new String[] {"user_values"},
                             "flex_name=?", new String[] {"EMAILPRECONFIGDB_TITLE_I_0"}, null);
                if ( c.moveToNext() ) {
                    Toast.makeText(getBaseContext(), c.getString(0), Toast.LENGTH_LONG).show();
                }
            } catch (RuntimeException e) {
                Log.e(AppConfig.TAG, "getFlexValue() :>>"+ e );
            } finally {
                if ( c != null ) {
                    c.close();
                }
            }
        } else if (strItem.equals(mstrList[nIndex++])) {
            try {
                ContentValues values = new ContentValues();
                c = cr.query(CONTENT_URI,  new String[] {"user_values"},
                             "flex_name=?", new String[] {"EMAILPRECONFIGDB_TITLE_I_0"}, null);
                if ( c.moveToNext() ) {
                    values.put("user_values", "±è³ª¿µ\n±èÈ£¼º");
                    cr.update(CONTENT_URI, values, "flex_name=?", new String[] {"EMAILPRECONFIGDB_TITLE_I_0"});
                }
            } catch (RuntimeException e) {
                Log.e(AppConfig.TAG, "getFlexValue() :>>"+ e );
            } finally {
                if ( c != null ) {
                    c.close();
                }
            }
        } else if (strItem.equals(mstrList[nIndex++])) {
            try {
                ContentValues values = new ContentValues();
                c = cr.query(CONTENT_URI,  new String[] {"user_values"},
                             "flex_name=?", new String[] {"EMAILPRECONFIGDB_TITLE_I_0"}, null);
                if ( c.moveToNext() ) {
                    values.put("user_values", "±èÈ£¼º");
                    cr.update(CONTENT_URI, values, "flex_name=?", new String[] {"EMAILPRECONFIGDB_TITLE_I_0"});
                }
            } catch (RuntimeException e) {
                Log.e(AppConfig.TAG, "getFlexValue() :>>"+ e );
            } finally {
                if ( c != null ) {
                    c.close();
                }
            }
        }
    }
}
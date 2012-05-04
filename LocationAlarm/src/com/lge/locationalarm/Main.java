package com.lge.locationalarm;

import android.app.ListActivity;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ListView;
import android.widget.ResourceCursorAdapter;
import android.widget.TextView;

public class Main extends ListActivity {
    static final int 	SUMMARY_LOCATIONALARM_COLUMN_INDEX 	= 1;
    static final String AUTHORITY 	= "locationalarm";
    static final Uri 	CONTENT_URI = Uri.parse("content://" + AUTHORITY + "/locationalarminfo");
    ListView 	mList;
    Button 		mAddButton;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_layout);
        mList = (ListView) findViewById(android.R.id.list);

//        mList = (ListView)findViewById(android.R.id.list);
        mAddButton = (Button) findViewById(R.id.add);
        Cursor c = getContentResolver().query(CONTENT_URI,  new String[] {"_id", "locationname"}, null, null, "locationname");
		startManagingCursor(c);        
        PackageConfigListItemAdapter adapter = new PackageConfigListItemAdapter(this, R.layout.alarmlist_item, c);
        setListAdapter(adapter);   
        mAddButton.setOnClickListener(new View.OnClickListener() {
        	public void onClick(View v){
        		//Intent intent = new Intent(Main.this, LocationAlarmEdit.class);
        		Intent intent = new Intent(Main.this, SetLocation.class);
        		startActivity(intent);
        		
        	}
        });
        
    }
	private final class PackageConfigListItemAdapter extends ResourceCursorAdapter {
        public PackageConfigListItemAdapter(Context context, int layout, Cursor c) {
            super(context, layout, c);
        }

        @Override
        public void bindView(View view, Context context, Cursor cursor) {
            final PackageConfigListItemCache cache = (PackageConfigListItemCache) view.getTag();
            // Set the name
            cache.locationAlarmText.setText(cache.locationAlarmString);            
        }

        @Override
        public View newView(Context context, Cursor cursor, ViewGroup parent) {
            View view = super.newView(context, cursor, parent);
            PackageConfigListItemCache cache = new PackageConfigListItemCache();
            cache.locationAlarmString= cursor.getString(SUMMARY_LOCATIONALARM_COLUMN_INDEX);
            cache.locationAlarmText = (TextView) view.findViewById(R.id.location);
/*            view.setOnClickListener(new OnClickListener(){ 
            	public void onClick(View v){
            		final PackageConfigListItemCache cache = (PackageConfigListItemCache) v.getTag();
            	}
            });
*/            
            view.setTag(cache);
            return view;
        } 
    }

    final static class PackageConfigListItemCache {
        public TextView locationAlarmText;
        public String 	locationAlarmString;
    }	    
}
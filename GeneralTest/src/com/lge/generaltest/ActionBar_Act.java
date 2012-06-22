package com.lge.generaltest;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

public class ActionBar_Act extends Activity {
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.actionbar_layout);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        String text = null;
        switch (item.getItemId()) {
        case android.R.id.home:
            text = "Application icon";
            break;
        case R.id.item1:
            text = "Action item, with text, displayed if room exists";
            break;
        case R.id.item2:
            text = "Action item, icon only, always displayed";
            break;
        case R.id.item3:
            text = "Normal menu item";
            break;
        default:
            return false;
        }
        Toast.makeText(this, text, Toast.LENGTH_SHORT).show();
        return true;
    }
}

package com.lge.generaltest;

import com.lge.config.AppConfig;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

public class ActivityLifeCycle_Act extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if ( AppConfig.LOGD ) Log.i(AppConfig.TAG, "onCreate");
        if (savedInstanceState != null) {
            if ( AppConfig.LOGD ) Log.i(AppConfig.TAG, "savedInstanceState = "+savedInstanceState);
        }
        setContentView(R.layout.activitylifecycle_layout);
        Button button 	= (Button) findViewById(R.id.Button01);
        button.setText("Resource Exam");
        button.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Intent intent = new Intent(ActivityLifeCycle_Act.this, ResourceExam_Act.class);
                startActivity(intent);
            }
        });
    }

    @Override
    protected void onRestart() {
        super.onRestart();
        if ( AppConfig.LOGD ) Log.i(AppConfig.TAG, "onRestart");
    }


    @Override
    protected void onStart() {
        super.onStart();
        if ( AppConfig.LOGD ) Log.i(AppConfig.TAG, "onStart");
    }

    @Override
    protected void onResume() {
        super.onResume();
        if ( AppConfig.LOGD ) Log.i(AppConfig.TAG, "onResume");
    }

    @Override
    protected void onPause() {
        super.onPause();
        if ( AppConfig.LOGD ) Log.i(AppConfig.TAG, "onPause");
    }

    @Override
    protected void onStop() {
        super.onStop();
        if ( AppConfig.LOGD ) Log.i(AppConfig.TAG, "onStop");
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if ( AppConfig.LOGD ) Log.i(AppConfig.TAG, "onDestroy");
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);
        String restoredString = savedInstanceState.getString("Restore String");
        if ( AppConfig.LOGD ) Log.i(AppConfig.TAG, "onCreate restoredString = "+restoredString);
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        outState.putString("Restore String", "I am saving");
        if ( AppConfig.LOGD ) Log.i(AppConfig.TAG, "onSaveInstanceState saving outState");
    }
}

package com.lge.generaltest;

import android.app.Activity;
import android.app.NotificationManager;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;
import android.widget.ImageButton;
import android.widget.RelativeLayout;


/**
 * Activity used by StatusBarNotification to show the notification to the user.
 */
public class NotificationDisplay extends Activity implements View.OnClickListener {
    /**
     * Initialization of the Activity after it is first created.  Must at least
     * call {@link android.app.Activity#setContentView setContentView()} to
     * describe what is to be displayed in the screen.
     */
    @Override
    protected void onCreate(Bundle icicle) {
        // Be sure to call the super class.
        super.onCreate(icicle);

        // Have the system blur any windows behind this one.
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_BLUR_BEHIND,
                             WindowManager.LayoutParams.FLAG_BLUR_BEHIND);

        RelativeLayout container = new RelativeLayout(this);

        ImageButton button = new ImageButton(this);
        button.setImageResource(getIntent().getIntExtra("moodimg", 0));
        button.setOnClickListener(this);

        RelativeLayout.LayoutParams lp = new RelativeLayout.LayoutParams(
            RelativeLayout.LayoutParams.WRAP_CONTENT,
            RelativeLayout.LayoutParams.WRAP_CONTENT);
        lp.addRule(RelativeLayout.CENTER_IN_PARENT);

        container.addView(button, lp);

        setContentView(container);
    }

    public void onClick(View v) {
        // The user has confirmed this notification, so remove it.
        ((NotificationManager) getSystemService(NOTIFICATION_SERVICE))
        .cancel(R.layout.statusbar_layout);

        // Pressing on the button brings the user back to our mood ring,
        // as part of the api demos app.  Note the use of NEW_TASK here,
        // since the notification display activity is run as a separate task.
        Intent intent = new Intent(this, StatusBar_Act.class);
        intent.setAction(Intent.ACTION_MAIN);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        startActivity(intent);

        // We're done.
        finish();
    }
}

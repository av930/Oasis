package com.lge.generaltest;

import android.app.Activity;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.RemoteViews;

//IntruderView ø° ¥Î«— activity
public class Immersive_Act extends Activity {
    private Button 				mButton01;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.immersive_layout);

        mButton01 = (Button) findViewById(R.id.button01);
        mButton01.setText("button 01");
        mButton01.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                NotificationManager mNotificationManager;
                // Get the notification manager serivce.
                mNotificationManager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);

                RemoteViews contentView = new RemoteViews(getPackageName(), R.layout.status_bar_custom);
//            	contentView.setImageViewResource(R.id.button01, R.drawable.btn_bt);
                contentView.setTextViewText(R.id.text, "KIHOON.KIM");
                // Set the icon, scrolling text and timestamp
                Notification notification = new Notification(R.drawable.stat_sad,"This is Test!!!", System.currentTimeMillis());
                notification.contentView = contentView;
                notification.flags = Notification.FLAG_HIGH_PRIORITY;
                Intent notificationIntent = new Intent(Immersive_Act.this, NotificationDisplay.class);
                PendingIntent contentIntent = PendingIntent.getActivity(Immersive_Act.this, 0, notificationIntent,0);
                notification.contentIntent = contentIntent;

                // Send the notification.
                // We use a layout id because it is a unique number.  We use it later to cancel.
                mNotificationManager.notify(R.layout.status_bar_custom, notification);
            }
        });
    }
}
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

/**
 * Demonstrates adding notifications to the status bar
 */
public class StatusBar_Act extends Activity {
    private NotificationManager mNotificationManager;
    private int mCount = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.statusbar_layout);
        Button button;

        // Get the notification manager serivce.
        mNotificationManager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);

        // Icon Only
        button = (Button) findViewById(R.id.happy);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setIconOnly(R.drawable.stat_happy, R.string.status_bar_notifications_happy_message, R.layout.statusbar_layout);
            }
        });

        button = (Button) findViewById(R.id.neutral);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setIconOnly(R.drawable.stat_neutral, R.string.status_bar_notifications_ok_message, R.layout.statusbar_layout);
            }
        });

        button = (Button) findViewById(R.id.sad);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setIconOnly(R.drawable.stat_sad, R.string.status_bar_notifications_sad_message, R.layout.statusbar_layout);
            }
        });

        final int layout = R.layout.activitylifecycle_layout;
        button = (Button) findViewById(R.id.buttonexam01);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setIconOnly(R.drawable.stat_exam01, R.string.exam01, layout+1);
            }
        });
        button = (Button) findViewById(R.id.buttonexam02);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setIconOnly(R.drawable.stat_exam02, R.string.exam02, layout+2);
            }
        });
        button = (Button) findViewById(R.id.buttonexam03);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setIconOnly(R.drawable.stat_exam03, R.string.exam03, layout+3);
            }
        });
        button = (Button) findViewById(R.id.buttonexam04);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setIconOnly(R.drawable.stat_exam04, R.string.exam04, layout+4);
            }
        });
        button = (Button) findViewById(R.id.buttonexam05);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setIconOnly(R.drawable.stat_exam05, R.string.exam05, layout+5);
            }
        });
        button = (Button) findViewById(R.id.buttonexam06);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setIconOnly(R.drawable.stat_exam06, R.string.exam06, layout+6);
            }
        });
        button = (Button) findViewById(R.id.buttonexam07);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setIconOnly(R.drawable.stat_exam07, R.string.exam07, layout+7);
            }
        });
        button = (Button) findViewById(R.id.buttonexam08);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setIconOnly(R.drawable.stat_exam08, R.string.exam08, layout+8);
            }
        });
        button = (Button) findViewById(R.id.buttonexam09);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setIconOnly(R.drawable.stat_exam09, R.string.exam09, layout+9);
            }
        });

        button = (Button) findViewById(R.id.buttonexam10);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setIconOnly(R.drawable.stat_exam10, R.string.exam10, layout+10);
            }
        });

        // Icon and Marquee
        button = (Button) findViewById(R.id.happyMarquee);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setIconMarquee(R.drawable.stat_happy, R.string.status_bar_notifications_happy_message, R.layout.statusbar_layout);
            }
        });

        button = (Button) findViewById(R.id.neutralMarquee);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setIconMarquee(R.drawable.stat_neutral, R.string.status_bar_notifications_ok_message, R.layout.statusbar_layout);
            }
        });

        button = (Button) findViewById(R.id.sadMarquee);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setIconMarquee(R.drawable.stat_sad, R.string.status_bar_notifications_sad_message, R.layout.statusbar_layout);
            }
        });

        //use remote views in ballon
        button = (Button) findViewById(R.id.happyViews);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setRemoteView(R.drawable.stat_happy, R.string.status_bar_notifications_happy_message);
            }
        });

        button = (Button) findViewById(R.id.neutralViews);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setRemoteView(R.drawable.stat_neutral, R.string.status_bar_notifications_ok_message);
            }
        });

        button = (Button) findViewById(R.id.sadViews);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setRemoteView(R.drawable.stat_sad, R.string.status_bar_notifications_sad_message);
            }
        });

        button = (Button) findViewById(R.id.defaultSound);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setDefault(Notification.DEFAULT_SOUND);
            }
        });

        button = (Button) findViewById(R.id.defaultVibrate);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setDefault(Notification.DEFAULT_VIBRATE);
            }
        });

        button = (Button) findViewById(R.id.defaultAll);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setDefault(Notification.DEFAULT_ALL);
            }
        });


        button = (Button) findViewById(R.id.button01);
        button.setText("1");
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
//                StatusBarManager mService;
//                mService = (StatusBarManager)getSystemService(Context.STATUS_BAR_SERVICE);
//                mService.setIcon("test", R.drawable.star_big_on, 0);
//                mService.setIconVisibility("test", true);
            }
        });

        button = (Button) findViewById(R.id.button02);
        button.setText("increase");
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                RemoteViews contentView = new RemoteViews(getPackageName(), R.layout.status_bar_custom);
//            	contentView.setImageViewResource(R.id.button01, R.drawable.btn_bt);
                mCount++;
                contentView.setTextViewText(R.id.text, "KIHOON.KIM"+mCount);
                // Set the icon, scrolling text and timestamp
                Notification notification = new Notification(R.drawable.stat_sys_gps_acquiring_anim,"DDD", System.currentTimeMillis());
                notification.contentView = contentView;
                Intent notificationIntent = new Intent(StatusBar_Act.this, NotificationDisplay.class);
                PendingIntent contentIntent = PendingIntent.getActivity(StatusBar_Act.this, 0, notificationIntent,0);
                notification.contentIntent = contentIntent;

                // Send the notification.
                // We use a layout id because it is a unique number.  We use it later to cancel.
                mNotificationManager.notify(R.layout.status_bar_custom, notification);
            }
        });

        button = (Button) findViewById(R.id.button03);
        button.setText("9");
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setMood(R.drawable.stat_happy, R.string.status_bar_notifications_happy_message, false, R.layout.statusbar_layout+1, 9);
            }
        });
        button = (Button) findViewById(R.id.button04);
        button.setText("22");
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setMood(R.drawable.stat_neutral, R.string.status_bar_notifications_ok_message, false, R.layout.statusbar_layout+2, 22);
            }
        });
        button = (Button) findViewById(R.id.button05);
        button.setText("200");
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setMood(R.drawable.stat_sad , R.string.status_bar_notifications_sad_message, false, R.layout.statusbar_layout+3, 200);
            }
        });
        button = (Button) findViewById(R.id.button06);
        button.setText("1020");
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                setMood(R.drawable.stat_sad , R.string.status_bar_notifications_sad_message, false, R.layout.statusbar_layout+4, 1020);
            }
        });

        button = (Button) findViewById(R.id.button07);
        button.setText("long");
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                CharSequence text = "This is kihoon";

                // choose the ticker text
                String tickerText = "Ticker Text";

                // Set the icon, scrolling text and timestamp
                Notification notification = new Notification(R.drawable.stat_sad, tickerText,
                        System.currentTimeMillis());

                // Set the info for the views that show in the notification panel.
                notification.setLatestEventInfo(StatusBar_Act.this, getText(R.string.status_bar_notifications_mood_title),
                                                "this is test [2012-02-23 10:37:59 - GeneralTest] Performing com.lge.generaltest.Main_Act activity launch[2012-02-23 10:37:59 - GeneralTest] Automatic Target Mode: using device 'LG-F160L-1ad2bf'[2012-02-23 10:37:59 - GeneralTest] Uploading GeneralTest.apk onto device 'LG-F160L-1ad2bf'[2012-02-23 10:37:59 - GeneralTest] Installing GeneralTest.apk...[2012-02-23 10:38:01 - GeneralTest] Success!", makeMoodIntent(0));

                notification.flags |= Notification.FLAG_ONGOING_EVENT;

                // Send the notification.
                // We use a layout id because it is a unique number.  We use it later to cancel.
                mNotificationManager.notify(100, notification);


            }
        });
        button = (Button) findViewById(R.id.button08);
        button.setText("going");
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                CharSequence text = "This is kihoon";

                // choose the ticker text
                String tickerText = "Ticker Text";

                // Set the icon, scrolling text and timestamp
                Notification notification = new Notification(0, tickerText,
                        System.currentTimeMillis());

                // Set the info for the views that show in the notification panel.
                notification.setLatestEventInfo(StatusBar_Act.this, getText(R.string.status_bar_notifications_mood_title),
                                                "this is test", makeMoodIntent(0));

                notification.flags |= Notification.FLAG_ONGOING_EVENT;

                // Send the notification.
                // We use a layout id because it is a unique number.  We use it later to cancel.
                mNotificationManager.notify(100, notification);


            }
        });
        button = (Button) findViewById(R.id.button09);
        button.setText("Music");
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                String tickerText = "Music";

                Notification notif = new Notification(R.drawable.stat_sad, tickerText,
                                                      System.currentTimeMillis());
                CharSequence text = "Music";
                notif.tickerText = "this is test";

                RemoteViews views = new RemoteViews(getPackageName(), R.layout.ongoing_notification);
                views.setImageViewResource(R.id.icon, R.drawable.stat_notify_musicplayer);
                views.setTextViewText(R.id.trackname, "Track Name");
                views.setTextViewText(R.id.artistalbum, "Artist Album");
                views.setTextViewText(R.id.trackname2, "Track Name");
                views.setTextViewText(R.id.info, "info");
                notif.contentView = views;

                mNotificationManager.notify(R.drawable.stat_notify_musicplayer, notif);
            }
        });
        button = (Button) findViewById(R.id.button10);
        button.setText("long2");
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                String tickerText = "long2";

                Notification notif = new Notification(R.drawable.stat_sad, tickerText,
                                                      System.currentTimeMillis());
                CharSequence text = "long2";
                notif.tickerText = "this is test";

                RemoteViews views = new RemoteViews(getPackageName(), R.layout.ongoing_notification);
                views.setImageViewResource(R.id.icon, R.drawable.stat_notify_musicplayer);
                views.setTextViewText(R.id.trackname, "Track Name");
                views.setTextViewText(R.id.artistalbum, "Licensed under the Apache License, Version 2.0 (the License) you may not use this file except in compliance with the License.You may obtain a copy of the License at");
                notif.contentView = views;

                mNotificationManager.notify(R.drawable.stat_sad, notif);
            }
        });
        button = (Button) findViewById(R.id.button11);
        button.setText("voice recorder");
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                CharSequence text = "This is kihoon";

                // choose the ticker text
                String tickerText = "Ticker Text";

                // Set the icon, scrolling text and timestamp
                Notification notification = new Notification(R.drawable.stat_sad, tickerText,
                        System.currentTimeMillis());

                // Set the info for the views that show in the notification panel.
                notification.setLatestEventInfo(StatusBar_Act.this, getText(R.string.status_bar_notifications_mood_title),
                                                "this is test [2012-02-23 10:37:59 - GeneralTest] Performing com.lge.generaltest.Main_Act activity launch[2012-02-23 10:37:59 - GeneralTest] Automatic Target Mode: using device 'LG-F160L-1ad2bf'[2012-02-23 10:37:59 - GeneralTest] Uploading GeneralTest.apk onto device 'LG-F160L-1ad2bf'[2012-02-23 10:37:59 - GeneralTest] Installing GeneralTest.apk...[2012-02-23 10:38:01 - GeneralTest] Success!", makeMoodIntent(0));

                notification.flags |= 0x400;

                // Send the notification.
                // We use a layout id because it is a unique number.  We use it later to cancel.
                mNotificationManager.notify(100, notification);

            }
        });
        button = (Button) findViewById(R.id.button12);
        button.setText("voice cancel");
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                mNotificationManager.cancel(100);
            }
        });
        button = (Button) findViewById(R.id.clear);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                mNotificationManager.cancel(R.layout.statusbar_layout);
            }
        });
    }

    private PendingIntent makeMoodIntent(int moodId) {
        // The PendingIntent to launch our activity if the user selects this
        // notification.  Note the use of FLAG_UPDATE_CURRENT so that if there
        // is already an active matching pending intent, we will update its
        // extras to be the ones passed in here.
        PendingIntent contentIntent = PendingIntent.getActivity(this, 0,
                                      new Intent(this, NotificationDisplay.class)
                                      .setFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
                                      .putExtra("moodimg", moodId),
                                      PendingIntent.FLAG_UPDATE_CURRENT);
        return contentIntent;
    }
    private void setIconOnly(int moodId, int textId, int layoutID) {
        setMood(moodId, textId, false, layoutID, 0);
    }

    private void setIconMarquee(int moodId, int textId, int layoutID) {
        setMood(moodId, textId, true, layoutID, 0);
    }

    private void setMood(int moodId, int textId, boolean showTicker, int layoutID, int number) {
        // In this sample, we'll use the same text for the ticker and the expanded notification
        CharSequence text = getText(textId);

        // choose the ticker text
        String tickerText = showTicker ? getString(textId) : null;

        // Set the icon, scrolling text and timestamp
        Notification notification = new Notification(moodId, tickerText,
                System.currentTimeMillis());

        // Set the info for the views that show in the notification panel.
        notification.setLatestEventInfo(this, getText(R.string.status_bar_notifications_mood_title),
                                        text, makeMoodIntent(moodId));

        notification.number = number;
        notification.flags |= Notification.FLAG_ONGOING_EVENT;

        // Send the notification.
        // We use a layout id because it is a unique number.  We use it later to cancel.
        mNotificationManager.notify(layoutID, notification);
    }

    private void setRemoteView(int moodId, int textId) {
        // Instead of the normal constructor, we're going to use the one with no args and fill
        // in all of the data ourselves.  The normal one uses the default layout for notifications.
        // You probably want that in most cases, but if you want to do something custom, you
        // can set the contentView field to your own RemoteViews object.
        Notification notif = new Notification();

        // This is who should be launched if the user selects our notification.
        notif.contentIntent = makeMoodIntent(moodId);

        // In this sample, we'll use the same text for the ticker and the expanded notification
        CharSequence text = getText(textId);
        notif.tickerText = text;

        // the icon for the status bar
        notif.icon = moodId;

        // our custom view
        RemoteViews contentView = new RemoteViews(getPackageName(), R.layout.status_bar_balloon);
        contentView.setTextViewText(R.id.text, text);
        contentView.setImageViewResource(R.id.icon, moodId);
        notif.contentView = contentView;

        // we use a string id because is a unique number.  we use it later to cancel the
        // notification
        mNotificationManager.notify(R.layout.statusbar_layout, notif);
    }

    private void setDefault(int defaults) {
        // This method sets the defaults on the notification before posting it.

        // This is who should be launched if the user selects our notification.
        PendingIntent contentIntent = PendingIntent.getActivity(this, 0,
                                      new Intent(this, StatusBar_Act.class), 0);

        // In this sample, we'll use the same text for the ticker and the expanded notification
        CharSequence text = getText(R.string.status_bar_notifications_happy_message);

        final Notification notification = new Notification(
            R.drawable.stat_happy,       // the icon for the status bar
            text,                        // the text to display in the ticker
            System.currentTimeMillis()); // the timestamp for the notification

        notification.setLatestEventInfo(
            this,                        // the context to use
            getText(R.string.status_bar_notifications_mood_title),
            // the title for the notification
            text,                        // the details to display in the notification
            contentIntent);              // the contentIntent (see above)

        notification.defaults = defaults;
        //notification.flags = Notification.

        mNotificationManager.notify(
            R.layout.statusbar_layout, // we use a string id because it is a unique
            // number.  we use it later to cancel the
            notification);                     // notification
    }
}

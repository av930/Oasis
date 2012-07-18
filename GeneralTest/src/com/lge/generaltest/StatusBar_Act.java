package com.lge.generaltest;

import android.app.Activity;
import android.app.Notification;
import android.app.Notification.Builder;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.RemoteViews;

public class StatusBar_Act extends Activity {
    private NotificationManager mNotificationManager;
    private int mCount = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.statusbar_layout);
        Button button;

        mNotificationManager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);

        // Icon Only
        button = (Button) findViewById(R.id.happy);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                makeNotification(R.drawable.stat_happy, R.string.status_bar_notifications_happy_message, R.layout.statusbar_layout, false, 0, 0);
            }
        });

        button = (Button) findViewById(R.id.neutral);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                makeNotification(R.drawable.stat_neutral, R.string.status_bar_notifications_ok_message, R.layout.statusbar_layout, false, 0, 0);
            }
        });

        button = (Button) findViewById(R.id.sad);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                makeNotification(R.drawable.stat_sad, R.string.status_bar_notifications_sad_message, R.layout.statusbar_layout, false, 0, 0);
            }
        });

        final int layout = R.layout.activitylifecycle_layout;
        button = (Button) findViewById(R.id.buttonexam01);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                makeNotification(R.drawable.stat_exam01, R.string.exam01, layout+1, false, 0, 0);
            }
        });
        button = (Button) findViewById(R.id.buttonexam02);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                makeNotification(R.drawable.stat_exam02, R.string.exam02, layout+2, false, 0, 0);
            }
        });
        button = (Button) findViewById(R.id.buttonexam03);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                makeNotification(R.drawable.stat_exam03, R.string.exam03, layout+3, false, 0, 0);
            }
        });
        button = (Button) findViewById(R.id.buttonexam04);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                makeNotification(R.drawable.stat_exam04, R.string.exam04, layout+4, false, 0, 0);
            }
        });
        button = (Button) findViewById(R.id.buttonexam05);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                makeNotification(R.drawable.stat_exam05, R.string.exam05, layout+5, false, 0, 0);
            }
        });
        button = (Button) findViewById(R.id.buttonexam06);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                makeNotification(R.drawable.stat_exam06, R.string.exam06, layout+6, false, 0, 0);
            }
        });
        button = (Button) findViewById(R.id.buttonexam07);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                makeNotification(R.drawable.stat_exam07, R.string.exam07, layout+7, false, 0, 0);
            }
        });
        button = (Button) findViewById(R.id.buttonexam08);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                makeNotification(R.drawable.stat_exam08, R.string.exam08, layout+8, false, 0, 0);
            }
        });
        button = (Button) findViewById(R.id.buttonexam09);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                makeNotification(R.drawable.stat_exam09, R.string.exam09, layout+9, false, 0, 0);
            }
        });

        button = (Button) findViewById(R.id.buttonexam10);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                makeNotification(R.drawable.stat_exam10, R.string.exam10, layout+10, false, 0, 0);
            }
        });

        // Icon and Marquee
        button = (Button) findViewById(R.id.happyMarquee);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
            	makeNotification(R.drawable.stat_happy, R.string.status_bar_notifications_happy_message, R.layout.statusbar_layout, true, 0, 0);
            }
        });

        button = (Button) findViewById(R.id.neutralMarquee);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
            	makeNotification(R.drawable.stat_neutral, R.string.status_bar_notifications_ok_message, R.layout.statusbar_layout, true, 0, 0);
            }
        });

        button = (Button) findViewById(R.id.sadMarquee);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
            	makeNotification(R.drawable.stat_sad, R.string.status_bar_notifications_sad_message, R.layout.statusbar_layout, true, 0, 0);
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
                makeNotification(R.drawable.stat_sad, R.string.status_bar_notifications_sad_message, R.layout.statusbar_layout, true, 0, Notification.DEFAULT_SOUND);
            }
        });

        button = (Button) findViewById(R.id.defaultVibrate);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                makeNotification(R.drawable.stat_sad, R.string.status_bar_notifications_sad_message, R.layout.statusbar_layout, true, 0, Notification.DEFAULT_VIBRATE);
            }
        });

        button = (Button) findViewById(R.id.defaultAll);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                makeNotification(R.drawable.stat_sad, R.string.status_bar_notifications_sad_message, R.layout.statusbar_layout, true, 0, Notification.DEFAULT_ALL);
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
                RemoteViews contentView = new RemoteViews(getPackageName(), R.layout.status_bar_call);
//            	contentView.setImageViewResource(R.id.button01, R.drawable.btn_bt);
                mCount++;
                contentView.setTextViewText(R.id.info, "KIHOON.KIM"+mCount);
                Notification notification = new Notification(R.drawable.stat_sys_gps_acquiring_anim,"DDD", System.currentTimeMillis());
                notification.contentView = contentView;
                Intent notificationIntent = new Intent(StatusBar_Act.this, NotificationDisplay.class);
                PendingIntent contentIntent = PendingIntent.getActivity(StatusBar_Act.this, 0, notificationIntent,0);
                notification.contentIntent = contentIntent;
                mNotificationManager.notify(R.layout.status_bar_call, notification);
            }
        });

        button = (Button) findViewById(R.id.button03);
        button.setText("9");
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                makeNotification(R.drawable.stat_happy, R.string.status_bar_notifications_happy_message, R.layout.statusbar_layout+1, false, 9, 0);
            }
        });
        button = (Button) findViewById(R.id.button04);
        button.setText("22");
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                makeNotification(R.drawable.stat_neutral, R.string.status_bar_notifications_ok_message, R.layout.statusbar_layout+2, false, 22, 0);
            }
        });
        button = (Button) findViewById(R.id.button05);
        button.setText("200");
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                makeNotification(R.drawable.stat_sad , R.string.status_bar_notifications_sad_message, R.layout.statusbar_layout+3, false, 200, 0);
            }
        });
        button = (Button) findViewById(R.id.button06);
        button.setText("1020");
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                makeNotification(R.drawable.stat_sad , R.string.status_bar_notifications_sad_message, R.layout.statusbar_layout+4, false, 1020, 0);
            }
        });
        button = (Button) findViewById(R.id.button07);
        button.setText("long");
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                CharSequence text = "This is kihoon";
                String tickerText = "Ticker Text";
                Notification notification = new Notification(R.drawable.stat_sad, tickerText, System.currentTimeMillis());
                notification.setLatestEventInfo(StatusBar_Act.this, getText(R.string.status_bar_notifications_mood_title),
                                                "this is test [2012-02-23 10:37:59 - GeneralTest] Performing com.lge.generaltest.Main_Act activity launch[2012-02-23 10:37:59 - GeneralTest] Automatic Target Mode: using device 'LG-F160L-1ad2bf'[2012-02-23 10:37:59 - GeneralTest] Uploading GeneralTest.apk onto device 'LG-F160L-1ad2bf'[2012-02-23 10:37:59 - GeneralTest] Installing GeneralTest.apk...[2012-02-23 10:38:01 - GeneralTest] Success!", makePendingIntent(0));
                notification.flags |= Notification.FLAG_ONGOING_EVENT;
                mNotificationManager.notify(100, notification);
            }
        });
        button = (Button) findViewById(R.id.button08);
        button.setText("going");
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                CharSequence text = "This is kihoon";
                String tickerText = "Ticker Text";
                Notification notification = new Notification(0, tickerText, System.currentTimeMillis());
                notification.setLatestEventInfo(StatusBar_Act.this, getText(R.string.status_bar_notifications_mood_title), "this is test", makePendingIntent(0));
                notification.flags |= Notification.FLAG_ONGOING_EVENT;
                mNotificationManager.notify(100, notification);
            }
        });
        button = (Button) findViewById(R.id.button09);
        button.setText("Music");
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                String tickerText = "Music";
                Notification notif = new Notification(R.drawable.stat_sad, tickerText, System.currentTimeMillis());
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
                Notification notif = new Notification(R.drawable.stat_sad, tickerText, System.currentTimeMillis());
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
                String tickerText = "Ticker Text";
                Notification notification = new Notification(R.drawable.stat_sad, tickerText, System.currentTimeMillis());
                notification.setLatestEventInfo(StatusBar_Act.this, getText(R.string.status_bar_notifications_mood_title),
                                                "this is test [2012-02-23 10:37:59 - GeneralTest] Performing com.lge.generaltest.Main_Act activity launch[2012-02-23 10:37:59 - GeneralTest] Automatic Target Mode: using device 'LG-F160L-1ad2bf'[2012-02-23 10:37:59 - GeneralTest] Uploading GeneralTest.apk onto device 'LG-F160L-1ad2bf'[2012-02-23 10:37:59 - GeneralTest] Installing GeneralTest.apk...[2012-02-23 10:38:01 - GeneralTest] Success!", makePendingIntent(0));
                notification.flags |= 0x400;
                mNotificationManager.notify(100, notification);
            }
        });
        button = (Button) findViewById(R.id.button12);
        button.setText("voice cancel");
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                mNotificationManager.cancel(100);            	
                //Notification notification = new Notification(R.drawable.stat_happy, "This is test", System.currentTimeMillis());                
                //notification.setLatestEventInfo(mContext, "title", "test", PendingIntent.getActivity(mContext, 0,
                //                new Intent(mContext, NotificationDisplay.class),
                //                PendingIntent.FLAG_UPDATE_CURRENT));
                //mNotificationManager.notify(100, notification);   
            }
        });
        button = (Button) findViewById(R.id.button13);
        button.setText("big");
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
            	Notification notification;            	
            	PendingIntent contentIntent;        		
        		contentIntent = makePendingIntent(0);
        		
        		// 아래 Builder의 setLargeIcon 함수에서 사용할 Bitmap 생성
        		Bitmap bm = BitmapFactory.decodeResource(getResources(), R.drawable.icon02);
        		
        		// Builder 생성
        		Builder builder = new Notification.Builder(StatusBar_Act.this);		
        		builder.setContentTitle("ContentTitle");
        		builder.setContentText("ContentText"); 
        		builder.setTicker("Jelly Bean Notification");
        		builder.setSmallIcon(R.drawable.icon01);
        		builder.setLargeIcon(bm);
        		builder.setContentIntent(contentIntent);
        		builder.setNumber(100);
        		builder.setContentInfo("hello");
        		builder.setPriority(0); // notification priority(몇 번째 위치에 표시 될 것인지..)
        		builder.addAction(R.drawable.icon01, "icon1", contentIntent);
        		builder.addAction(R.drawable.icon02, "icon2", contentIntent);
        		builder.addAction(R.drawable.icon03, "icon3", contentIntent);        		        		
        			
        		notification = builder.build();
        		mNotificationManager.notify(0, notification);
            }
        });        
        button = (Button) findViewById(R.id.clear);
        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                mNotificationManager.cancel(R.layout.statusbar_layout);
            }
        });
    }

    private PendingIntent makePendingIntent(int moodId) {
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

    private void makeNotification(int moodId, int textId, int layoutID, boolean showTicker, int number, int defaults) {
        CharSequence text = getText(textId);
        String tickerText = showTicker ? getString(textId) : null;
        Notification notification = new Notification(moodId, tickerText, System.currentTimeMillis());
        notification.setLatestEventInfo(this, getText(R.string.status_bar_notifications_mood_title), text, makePendingIntent(moodId));
        notification.number = number;
        notification.flags |= Notification.FLAG_ONGOING_EVENT;
        notification.defaults = defaults;
        mNotificationManager.notify(layoutID, notification);
    }

    private void setRemoteView(int moodId, int textId) {
        Notification notif = new Notification();
        notif.contentIntent = makePendingIntent(moodId);
        CharSequence text = getText(textId);
        notif.tickerText = text;
        notif.icon = moodId;
        RemoteViews contentView = new RemoteViews(getPackageName(), R.layout.status_bar_call);
        contentView.setTextViewText(R.id.info, text);
        contentView.setImageViewResource(R.id.icon, moodId);
        notif.contentView = contentView;
        mNotificationManager.notify(R.layout.statusbar_layout, notif);
    }
}

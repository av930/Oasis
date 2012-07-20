package com.lge.generaltest;

import android.app.Activity;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.SystemClock;

import android.view.View;
import android.widget.Button;
import android.widget.RemoteViews;
import android.widget.TextView;
import android.widget.Toast;

public class StatusBarCall_Act extends Activity {
    static final int NOTIFICATION_ID 		= 1;
    static final int IN_CALL_NOTIFICATION 	= 2;

    private Context mContext;
    private NotificationManager mNotificationMgr;

    private long chronometerBaseTime;
    private long chronometerBaseTime2;

    private int mInCallResId;

    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Toast.makeText(getBaseContext(), "End Call", Toast.LENGTH_LONG).show();
            finish();
        }
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        IntentFilter filter = new IntentFilter();
        filter.addAction("com.lge.test");
        getBaseContext().registerReceiver(mReceiver, filter);

        setContentView(R.layout.statusbarcall_layout);

        mContext = getBaseContext();
        mInCallResId = R.drawable.stat_sys_phone_call;
        mNotificationMgr = (NotificationManager)getSystemService(NOTIFICATION_SERVICE);

        final Button btnStart = (Button)findViewById(R.id.btnStartCall);
        final Button btnEnd = (Button)findViewById(R.id.btnEndCall);
        final TextView textCallStatus = (TextView)findViewById(R.id.textCallStatus);

        btnStart.setOnClickListener(new View.OnClickListener() {

            public void onClick(View v) {
                chronometerBaseTime2 = SystemClock.elapsedRealtime();
                textCallStatus.setText("Call Status : True");
                mNotificationMgr.notify("CALLING", IN_CALL_NOTIFICATION, getCallingNotification());
            }
        });

        btnEnd.setOnClickListener(new View.OnClickListener() {

            public void onClick(View v) {
                // TODO Auto-generated method stub
                chronometerBaseTime2 = 0;
                textCallStatus.setText("Call Status : False");
                mNotificationMgr.cancel("CALLING", IN_CALL_NOTIFICATION);
            }
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mNotificationMgr.cancel("CALLING", IN_CALL_NOTIFICATION);
        unregisterReceiver(mReceiver);
    }

    // 종료했을 시 다시 앱으로 돌아오는 인텐트 반환
    private Intent getReturnToCallActivity() {
        final Intent notificationIntent = new Intent(mContext, StatusBarCall_Act.class);
        notificationIntent.setAction(Intent.ACTION_MAIN);
        notificationIntent.addCategory(Intent.CATEGORY_LAUNCHER);
        return notificationIntent;
    }

    private Notification getCallingNotification() {
        // 노티피케이션 생성하기
        Notification notification = new Notification();
        notification.icon = mInCallResId;
        notification.flags |= Notification.FLAG_ONGOING_EVENT;

        // 종료했을 시 다시 앱으로 돌아오는 펜딩 인텐트 생성하기
        PendingIntent returnToCallPendingIntent =
            PendingIntent.getActivity(mContext, 0, getReturnToCallActivity(), 0);

        // 노티피케이션 내 위치한 버튼에 펜딩 인텐트 생성하기
        PendingIntent endCallPendingIntent =
            PendingIntent.getBroadcast(mContext, 0, new Intent("com.lge.test"), 0);

        // 리모트 뷰 생성 하기
        RemoteViews contentView =
            new RemoteViews(mContext.getPackageName(), R.layout.status_bar_call);

        contentView.setImageViewResource(R.id.icon, mInCallResId);
        contentView.setOnClickPendingIntent(R.id.button1, endCallPendingIntent);

        // 시간 계산하기
        chronometerBaseTime = (chronometerBaseTime2 == 0) ?
                              SystemClock.elapsedRealtime() :
                              SystemClock.elapsedRealtime() + (chronometerBaseTime2 - SystemClock.elapsedRealtime());

        // 노티피케이션에 보일 시간, 전화번호 설정
//		contentView.setChronometer(R.id.text1, chronometerBaseTime, mContext.getString(R.string.notification_ongoing_call_format), true);
        contentView.setChronometer(R.id.text1, chronometerBaseTime, "Current call (%s)", true);
        contentView.setTextViewText(R.id.title, "010-5096-7523");

        notification.contentView = contentView;
        notification.contentIntent = returnToCallPendingIntent;

        return notification;
    }
}
package com.lge.generaltest;

import android.app.Activity;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.Button;

public class Progressbar_Act extends Activity {
    private Button 				mButton01;
    private Button 				mButton02;

    private static final int DIALOG_PROGRESS = 4;
    private static final int MAX_PROGRESS = 53;
    private ProgressDialog 	mProgressDialog;
    private int 			mProgress;
    private Handler 		mProgressHandler;

    private static final int DIALOG_PROGRESS_THREAD = 5;
    private static final int MAX_PROGRESS_THREAD = 100;
    private ProgressDialog 	mProgressThreadDialog;
    private int 			mProgressThread;
    private Handler 		mProgressThreadHandler;
    private FileCopy 		mFileCopy;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.progressbar_layout);

        mButton01 = (Button) findViewById(R.id.button01);
        mButton01.setText("progress bar style 1");
        mButton01.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                showDialog(DIALOG_PROGRESS);
                mProgress = 0;
                mProgressDialog.setProgress(0);
                mProgressHandler.sendEmptyMessage(0);
            }
        });

        mButton02 = (Button) findViewById(R.id.button02);
        mButton02.setText("progress bar style 2");
        mButton02.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                showDialog(DIALOG_PROGRESS_THREAD);
                mFileCopy = new FileCopy();
                mFileCopy.start();

            }
        });

        mProgressHandler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                if (mProgress >= MAX_PROGRESS) {
                    mProgressDialog.dismiss();
                } else {
                    mProgress++;
                    mProgressDialog.incrementProgressBy(1);
                    mProgressHandler.sendEmptyMessageDelayed(0, 100);
                }
            }
        };

        mProgressThreadHandler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                if (mProgressThread >= MAX_PROGRESS_THREAD) {
                    mProgressThreadDialog.dismiss();
                }
            }
        };
    }

    @Override
    protected Dialog onCreateDialog(int id) {
        switch (id) {
        case DIALOG_PROGRESS:
            mProgressDialog = new ProgressDialog(this);
//	        mProgressDialog.setIcon(R.drawable.alert_dialog_icon);
            mProgressDialog.setTitle("파일 다운로드 중");
            mProgressDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
            mProgressDialog.setMax(MAX_PROGRESS);
//	        mProgressDialog.setProgressNumberFormat("");
            mProgressDialog.setButton(DialogInterface.BUTTON_NEGATIVE, "cancel", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int whichButton) {
                }
            });
            return mProgressDialog;
        case DIALOG_PROGRESS_THREAD:
            mProgressThreadDialog = new ProgressDialog(this);
//	        mProgressThreadDialog(R.drawable.alert_dialog_icon);
            mProgressThreadDialog.setTitle("파일 다운로드 중");
            mProgressThreadDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
            mProgressThreadDialog.setMax(MAX_PROGRESS_THREAD);
            mProgressThreadDialog.setButton(DialogInterface.BUTTON_NEGATIVE, "cancel", new DialogInterface.OnClickListener() {
                public void onClick(final DialogInterface dialog, final int whichButton) {
                }
            });
            return mProgressThreadDialog;
        }
        return null;
    }

    class FileCopy extends Thread {
        public FileCopy() {

        }
        public void run() {
            mProgressThread = 0;
            mProgressThreadDialog.setProgress(0);
            mProgressThreadHandler.sendEmptyMessage(0);
            for (int i = 0;i<100;i++) {
                try {
                    sleep(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                mProgressThread++;
                mProgressThreadDialog.setProgress(i);
                mProgressThreadHandler.sendEmptyMessage(0);
            }
        }
    }
}
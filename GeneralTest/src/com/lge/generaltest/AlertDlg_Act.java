package com.lge.generaltest;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class AlertDlg_Act extends Activity implements DialogInterface.OnClickListener {
    private static final int	DIALOG01 = 0;
    private static final int	DIALOG02 = 1;
    private Button 				mButton01;
    private Button 				mButton02;
    private Button 				mButton03;
    private int					mDlgMode = 0;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.alertdlg_layout);

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setMessage("do you want to download again?")
        .setCancelable(false)
        .setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                Toast toast =  Toast.makeText(getBaseContext(), android.R.string.ok, Toast.LENGTH_LONG);
                toast.show();
            }
        })
        .setNegativeButton(android.R.string.cancel, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                Toast toast =  Toast.makeText(getBaseContext(), android.R.string.cancel, Toast.LENGTH_LONG);
                toast.show();
                dialog.cancel();
            }
        });
        AlertDialog alertDialog = builder.create();
        alertDialog.show();

        mButton01 = (Button) findViewById(R.id.button01);
        mButton01.setText("button01");
        mButton01.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                mDlgMode = 0;
                showDialog(DIALOG01);
            }
        });
        mButton02 = (Button) findViewById(R.id.button02);
        mButton02.setText("button02");
        mButton02.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                mDlgMode = 1;
                showDialog(DIALOG01);
            }
        });
        mButton03 = (Button) findViewById(R.id.button03);
        mButton03.setText("button03");
        mButton03.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                showDialog(DIALOG02);
            }
        });
    }

    @Override
    protected Dialog onCreateDialog(int id) {
        switch (id) {
        case DIALOG01:
            return new AlertDialog.Builder(this).setMessage("1111").setTitle(R.string.shorttext)
                   .setPositiveButton(android.R.string.ok, this)
                   .setNegativeButton(android.R.string.cancel, this).create();
        case DIALOG02:
            return new AlertDialog.Builder(this).setMessage("2222").setTitle(R.string.shorttext)
                   .setPositiveButton(android.R.string.ok, this)
                   .setNegativeButton(android.R.string.cancel, this).create();
        }
        return null;
    }

    @Override
    protected void onPrepareDialog(int id, Dialog dialog, Bundle args) {
        super.onPrepareDialog(id, dialog, args);
        switch (id) {
        case DIALOG01:
            switch (mDlgMode) {
            case 0:
                dialog.setTitle(R.string.longtext);
                break;
            case 1:
                dialog.setTitle(R.string.shorttext);
                break;
            }
            break;
        case DIALOG02:
            dialog.setTitle(R.string.shorttext);
            break;
        }
    }

    public void onClick(DialogInterface dialog, int which) {
        dialog.dismiss();
    }
}

package com.lge.generaltest;

import android.app.Activity;
import android.os.Bundle;
import android.webkit.WebView;

import com.lge.generaltest.R;

public class WebView_Act extends Activity {
    private WebView mWebView;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.webview_layout);
        mWebView = (WebView) findViewById(R.id.webview);
        //mWebView.loadDataWithBaseURL("http://naver.com");
        mWebView.loadData("<html><head><title> Untitled1</title></head><body>"+
                          "<font color=\"red\"><i>"+"test hi"+" </i></font><br>"+
                          "<span style=\"font-family:\'Clockopia\';\">Clockopia</span><br>"+
                          "<span style=\"font-family:\'Droid Sans Thai\';\">Droid Sans Thai</span><br>"+
                          "<span style=\"font-family:\'Droid Sans Arabic\';\">Droid Sans Arabic</span><br>"+
                          "<span style=\"font-family:\'Droid Sans Fallback\';\">Droid Sans Fallback</span><br>"+
                          "<span style=\"font-family:\'Droid Sans Hebrew\';\">Droid Sans Hebrew</span><br>"+
                          "<span style=\"font-family:\'Droid Serif\';\">Droid Serif</span><br>"+
                          "<span style=\"font-family:\'Droid Sans\';\">Droid Sans</span><br>"+
                          "<span style=\"font-family:\'SJplayful\';\">SJplayful</span><br>"+
                          "</body></html>"
                          ,  "text/html", "utf-8");
    }
}
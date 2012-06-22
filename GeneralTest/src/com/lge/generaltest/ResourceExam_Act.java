package com.lge.generaltest;

import android.app.Activity;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.widget.ImageView;

public class ResourceExam_Act extends Activity {
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.resourceexam_layout);
        ImageView preview01 = (ImageView)findViewById(R.id.widgetpreviewview);
        PackageManager mPackageManager = getPackageManager();

        try {
            String packagename = "com.lge.graphictest";
            Resources res = mPackageManager.getResourcesForApplication(packagename);
            int id = res.getIdentifier( packagename+":array/widgetPreview", null, null);
            TypedArray drawableArray = res.obtainTypedArray(id);
            Drawable drawable01 = drawableArray.getDrawable(0);
            preview01.setImageDrawable(drawable01);
        } catch (NameNotFoundException e) {
            e.printStackTrace();
        }
        //다음과 같이 다른 application의 리소스를 액세스 할수 있다.
        //Context ctx = createPackageContext(“com.lge.util”, ...);
        //Resources res = ctx.getResources();
        //Drawable icon = res.getDrawable(      res.getIdentifier(“icon”, “drawable”, “com.lge.util”));
        //String text = res.getString(      res.getIdentifier(“text”, “string”, “com.lge.util”));
    }
}
package com.mqbest.myopencv;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;

public class MainActivity extends AppCompatActivity implements View.OnClickListener{
    private static final String TAG = "MainActivity";
    private static Button btnColorDetection = null;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initView();
    }

    @Override
    public void onClick(View view) {
        int id = view.getId();
        if(R.id.btn_start_color_detection == id){
            startActivity(new Intent(this, ObjectTrackerActivity.class));
        }
    }

    private void initView(){
        btnColorDetection = (Button)findViewById(R.id.btn_start_color_detection);
        btnColorDetection.setOnClickListener(this);
    }


}
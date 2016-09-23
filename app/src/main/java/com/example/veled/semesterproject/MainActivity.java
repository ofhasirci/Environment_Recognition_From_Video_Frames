package com.example.veled.semesterproject;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    Button save, compare, getFrames;
    ProgressBar progressBar;
    int count;
    TextView tv;
    String path;
    boolean getff = false;
    SharedPreferences preferences;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        tv = (TextView) findViewById(R.id.textView);
        save = (Button) findViewById(R.id.button);
        compare = (Button) findViewById(R.id.button2);
        getFrames = (Button) findViewById(R.id.button3);
        progressBar = (ProgressBar) findViewById(R.id.progressBar);
        progressBar.setProgress(0);

        File file = new File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM), "descriptors.xml");
        path = file.getPath();

        isFileExist(file);

        save.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                Intent intent = new Intent(getApplicationContext(), CameraActivity.class);
                intent.putExtra("save", true);
                intent.putExtra("count", count);
                startActivity(intent);
            }
        });


        compare.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                if (getff) {
                    Intent intent = new Intent(getApplicationContext(), CameraActivity.class);
                    intent.putExtra("save", false);
                    intent.putExtra("count", count);
                    progressBar.setProgress(0);
                    getff = false;
                    startActivity(intent);
                }else {
                    Toast.makeText(getApplicationContext(), "First tap Get Frames button", Toast.LENGTH_SHORT).show();
                }
            }
        });

        getFrames.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                progressBar.setVisibility(View.VISIBLE);
                getff = getFromFile(path, count);
                if (getff){
                    tv.append(" getFromFile is working ");
                    progressBar.setProgress(100);
                    Toast.makeText(getApplicationContext(), "Ready to compare", Toast.LENGTH_SHORT).show();
                }
            }
        });
    }

    private void isFileExist(File file) {
        if (file.exists()){
            preferences = getSharedPreferences("myPreferences", MODE_PRIVATE);
            count = preferences.getInt("count", 0);
            tv.setText(String.valueOf(count));
            Log.e("TAG", "COUNT: " +count);
        }else {
            count = 0;
        }
    }

    public native boolean getFromFile(String path, int cnt);


    static {
        System.loadLibrary("myModule");
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onStart() {
        super.onStart();
        Log.e("TAG", "On START");
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.e("TAG", "On DESTROY");
    }

    @Override
    protected void onStop() {
        super.onStop();
        Log.e("TAG", "On STOP");
    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.e("TAG", "On PAUSE");

    }

}

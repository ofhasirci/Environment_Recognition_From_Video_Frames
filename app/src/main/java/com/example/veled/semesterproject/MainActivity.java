package com.example.veled.semesterproject;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    Button save, compare;

    TextView tv;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        tv = (TextView) findViewById(R.id.textView);
        save = (Button) findViewById(R.id.button);
        compare = (Button) findViewById(R.id.button2);

        save.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                Intent intent = new Intent(getApplicationContext(), CameraActivity.class);
                intent.putExtra("save", true);
                startActivity(intent);
            }
        });


        compare.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {


                Intent intent = new Intent(getApplicationContext(), CameraActivity.class);
                intent.putExtra("save", false);
                startActivity(intent);
            }
        });
    }


    static {
        System.loadLibrary("myModule");
    }



}

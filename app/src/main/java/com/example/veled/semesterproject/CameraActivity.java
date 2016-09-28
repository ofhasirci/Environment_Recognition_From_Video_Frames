package com.example.veled.semesterproject;

import android.Manifest;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.SurfaceView;
import android.view.WindowManager;
import android.widget.TextView;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Mat;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;

import java.io.File;
import java.util.LinkedList;

public class CameraActivity extends AppCompatActivity implements CameraBridgeViewBase.CvCameraViewListener2{

    private AsynchronousCompare task=null;
    LinkedList<AsynchronousCompare> tasks = new LinkedList<>();
    public Mat matFrame, matCloneFrame;
    int result;
    int index = 1;
    private int PER_REQ = 1;
    private CameraBridgeViewBase openCvCameraView;
    private BaseLoaderCallback mLoaderCallBack = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status){
                case LoaderCallbackInterface.SUCCESS: {
                    System.loadLibrary("myModule");
                    Log.e("TAG", "OpenCv succesfully loaded");
                    openCvCameraView.enableView();
                    break;
                }
                default:{
                    super.onManagerConnected(status);
                }
            }
        }
    };
    TextView tv;
    Size size = new Size(160,120);
    boolean saveOrNot = true;
    String path;
    int count;

    public native int compareFrames(long addr1, int i);
    public native void saveToFile(long addr, String path, String descName, String frmName);
    public native void clearMap();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_get_frames);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        openCvCameraView = (CameraBridgeViewBase) findViewById(R.id.surface_view);
        openCvCameraView.setMaxFrameSize(320, 240);
        openCvCameraView.setVisibility(SurfaceView.VISIBLE);
        openCvCameraView.setCvCameraViewListener(this);

        File file = new File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM), "descriptors.xml");
        path = file.getPath();

        tv = (TextView) findViewById(R.id.textView2);
        tv.setText("beyylee");
        Bundle data = getIntent().getExtras();
        saveOrNot = data.getBoolean("save");
        count = data.getInt("count");


        if (ContextCompat.checkSelfPermission(getApplicationContext(), Manifest.permission.CAMERA)!= PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(getApplicationContext(), Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED){

            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.CAMERA, Manifest.permission.WRITE_EXTERNAL_STORAGE}, PER_REQ);
        }

    }

    @Override
    protected void onResume() {
        super.onResume();
        if (!OpenCVLoader.initDebug()) {
            OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_11, getApplicationContext(), mLoaderCallBack);
        }else {
            mLoaderCallBack.onManagerConnected(LoaderCallbackInterface.SUCCESS);
            Log.d("TAG", "initdebug true");
        }
    }

    @Override
    protected void onPause() {
        super.onPause();

        SharedPreferences.Editor editor = getSharedPreferences("myPreferences", MODE_PRIVATE).edit();
        editor.putInt("count", count);
        editor.apply();
        for (AsynchronousCompare asy: tasks){
            if (!asy.isCancelled()){
                asy.cancel(true);
            }
        }

        tasks.clear();
        clearMap();

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        if (openCvCameraView != null){
            openCvCameraView.disableView();
        }

        Log.e("TAG", "ONDESTROY CAMACT..");

    }

    public void compareOrSave(final int frameIndex){

        runOnUiThread(new Runnable() {
            @Override
            public void run() {

                if (!saveOrNot) {
                    task = new AsynchronousCompare(matCloneFrame);
                    tasks.add(task);
                    Log.e("TAG", "Tasks size---> "+ tasks.size());
                    task.execute();
                }else if (frameIndex==15){
                    task = new AsynchronousCompare(matCloneFrame, count);
                    count++;
                    Log.e("TAG", "COUNT: "+count);
                    Log.e("TAG", "FRAMEINDEX: "+frameIndex);
                    task.execute();
                }
            }
        });

    }

    private class AsynchronousCompare extends AsyncTask<Void, Void, Integer> {
        private Mat mat;
        private String name;
        private String frameName;
        private int mCount;

        public AsynchronousCompare(Mat mat, int mCount){
            this.mat = mat;
            this.mCount = mCount;
            name = pullName(mCount);
            frameName = pullFrameName(mCount);
        }

        public AsynchronousCompare(Mat mat){
            this.mat = mat;
        }

        @Override
        protected Integer doInBackground(Void... params) {
            if (!saveOrNot) {
                for (int i=0;i<count;i++){
                    result = compareFrames(mat.getNativeObjAddr(), i);
                    if (result>=80){
                        return i;
                    }
                    if (isCancelled()){
                        task.cancel(true);
                        break;
                    }
               }
                return -1;
            } else if (!isCancelled()) {
                saveToFile(mat.getNativeObjAddr(), path, name, frameName);
                return -1;
                }else {
                    task.cancel(true);
                    return -1;
                }
        }

        @Override
        protected void onPostExecute(Integer aVoid) {

            /*if (aVoid<0){
                Toast.makeText(CameraActivity.this, "Saved", Toast.LENGTH_SHORT).show();
            }else{
                tv.append(String.valueOf(aVoid)+" ");
            }*/
            tv.append(String.valueOf(aVoid)+" ");
            //tasks.getLast().cancel(true);
            //task.cancel(true);
        }
    }


    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        switch (requestCode){
            case 1:{
                for (int i = 0; i<permissions.length;i++){
                    if (grantResults[i] == PackageManager.PERMISSION_GRANTED){
                        Log.d("TAG", "Permission Granted");
                    }else {
                        Log.d("TAG", "Permission Denied");
                    }
                }
            }
            break;
            default:{
                super.onRequestPermissionsResult(requestCode, permissions, grantResults);

            }
        }
    }

    @Override
    public void onCameraViewStarted(int width, int height) {
        matFrame = new Mat();
        matCloneFrame = new Mat();
    }

    @Override
    public void onCameraViewStopped() {

    }

    @Override
    public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {
        index++;
        matFrame = inputFrame.rgba();
        matCloneFrame = matFrame.clone();
        Imgproc.resize(matCloneFrame, matCloneFrame, size);

        switch (index){
            case 1:{
                compareOrSave(index);
                break;
            }
            case 8:{
                compareOrSave(index);
                break;
            }
            case 15:{
                compareOrSave(index);
                break;
            }
            case 22:{
                compareOrSave(index);
                break;
            }
            case 29:{
                compareOrSave(index);
                break;
            }
            case 30:{
                index=1;
                break;
            }
        }
        Log.d("Index", String.valueOf(index));

        return matFrame;
    }

    public String pullName(int nCount){
        return "descriptor"+String.valueOf(nCount);
    }

    public String pullFrameName(int nCount){
        return "frame"+String.valueOf(nCount);
    }
}

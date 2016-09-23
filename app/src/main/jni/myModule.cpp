#include <jni.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <vector>
#include <android/log.h>
#include <iostream>
#include <fstream>
#include <string>


cv::Mat getDescriptor(cv::Mat image){

    cv::OrbFeatureDetector detector;
    std::vector<cv::KeyPoint> keypoints;

    cv::cvtColor(image, image, CV_RGBA2GRAY);

    detector.detect(image, keypoints);

    cv::OrbDescriptorExtractor extractor;
    cv::Mat descriptor;

    extractor.compute(image, keypoints, descriptor);

    return descriptor;
}



using namespace std;
using namespace cv;


std::map<string, Mat> descriptorMap;

extern "C" {

JNIEXPORT jboolean JNICALL
Java_com_example_veled_semesterproject_MainActivity_getFromFile(JNIEnv *env, jobject instance,
                                                                jstring path_, jint cnt) {
    const char *path = env->GetStringUTFChars(path_, 0);
    Mat img;

    cv::FileStorage fs(path, FileStorage::READ);
    if (fs.isOpened()){
        __android_log_print(ANDROID_LOG_ERROR, "TAG", "FS IS OPEN --> FROMFILE");
        for (int i=0; i<cnt; i++){
            fs["descriptor"+i] >> img;
            descriptorMap.insert(std::map<string, Mat>::value_type("descriptor"+i, img));
            __android_log_print(ANDROID_LOG_ERROR, "TAG", "INSERTING TO MAP--- %d", i);
        }
    }

    fs.release();

    env->ReleaseStringUTFChars(path_, path);
    return true;
}

JNIEXPORT void JNICALL
Java_com_example_veled_semesterproject_CameraActivity_clearMap(JNIEnv *env, jobject instance) {

    descriptorMap.clear();
    __android_log_print(ANDROID_LOG_ERROR, "TAG", "CLEAR MAP SIZE--- %d", descriptorMap.size());

}

JNIEXPORT void JNICALL
Java_com_example_veled_semesterproject_CameraActivity_saveToFile(JNIEnv *env,
                                                                 jobject instance,
                                                                 jlong addr, jstring filepath, jstring descName, jstring frmName) {
    Mat & image = *(Mat *) addr;
    const char *path = env->GetStringUTFChars(filepath, NULL);
    const char *name = env->GetStringUTFChars(descName, NULL);
    const char *fname = env->GetStringUTFChars(frmName, NULL);


    cv::FileStorage fs(path, FileStorage::APPEND);

    Mat descriptor = getDescriptor(image);

    if (fs.isOpened()) {
        __android_log_print(ANDROID_LOG_ERROR, "TAG", "fs write desc");
        fs << name << descriptor;
    } else __android_log_print(ANDROID_LOG_ERROR, "TAG", "fs is NOT opened");

    if (fs.isOpened()){
        __android_log_print(ANDROID_LOG_ERROR, "TAG", "fs write frame");
        fs << fname << image;
    }

    fs.release();

    env->ReleaseStringUTFChars(filepath, path);
    env->ReleaseStringUTFChars(descName, name);
    env->ReleaseStringUTFChars(frmName, fname);

}

JNIEXPORT jint JNICALL
Java_com_example_veled_semesterproject_CameraActivity_compareFrames(JNIEnv *env,
                                                                    jobject instance,
                                                                    jlong addr1) {


    Mat & image = *(Mat *) addr1;
    Mat descriptor2;
    Mat descriptor1;

    descriptor1 = getDescriptor(image);


    std::vector<DMatch> matches12, matches21;
    std::vector<DMatch> goodMatches;
    cv::BFMatcher matcher;


    /*for (std::map<string, Mat>::iterator it = descriptorMap.begin(); it != descriptorMap.end(); ++it){
        //descriptor2 = it->second;

        __android_log_print(ANDROID_LOG_ERROR, "TAG", "COLS ARE NOT EQUAL -- %d", it->second.cols);

        /*if (descriptor1.type()==it->second.type() && descriptor1.cols == it->second.cols) {
            __android_log_print(ANDROID_LOG_ERROR, "TAG", "COLS ARE EQUAL");

            matcher.match(descriptor1, it->second, matches12);
            matcher.match(it->second, descriptor1, matches21);


            for (int i = 0; i < matches12.size(); i++) {
                DMatch forward = matches12[i];
                DMatch backward = matches21[forward.trainIdx];
                if (backward.trainIdx == forward.queryIdx) {
                    goodMatches.push_back(forward);
                }
            }

        } else{
            __android_log_print(ANDROID_LOG_ERROR, "TAG", "COLS ARE NOT EQUAL -- %d", it->second.cols);
        }

        if (goodMatches.size() >= 250){
            break;
        }
    }*/

    return descriptor2.cols ;

}


}
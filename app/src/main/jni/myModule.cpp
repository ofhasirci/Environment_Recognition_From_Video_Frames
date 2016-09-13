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


extern "C" {

JNIEXPORT jint JNICALL
Java_com_example_veled_semesterproject_CameraActivity_getFromFile(JNIEnv *env, jobject instance,
                                                                  jlong addr, jstring path_,
                                                                  jstring desName_) {
    const char *path = env->GetStringUTFChars(path_, 0);
    const char *desName = env->GetStringUTFChars(desName_, 0);
    Mat &img = *(Mat*) addr;

    cv::FileStorage fs(path, FileStorage::READ);
    if (fs.isOpened()){
        fs[desName] >> img;
        __android_log_print(ANDROID_LOG_ERROR, "TAG", "FS IS OPEN --> FROMFILE");
    }

    fs.release();

    env->ReleaseStringUTFChars(path_, path);
    env->ReleaseStringUTFChars(desName_, desName);
    return img.cols;
}

JNIEXPORT void JNICALL
Java_com_example_veled_semesterproject_CameraActivity_saveToFile(JNIEnv *env,
                                                                 jobject instance,
                                                                 jlong addr, jstring filepath, jstring descName, jstring frmName) {
    Mat & image = *(Mat *) addr;
    const char *path = env->GetStringUTFChars(filepath, NULL);
    const char *name = env->GetStringUTFChars(descName, NULL);
    const char *fname = env->GetStringUTFChars(frmName, NULL);


    cv::FileStorage fs(path, FileStorage::WRITE);
    if (fs.isOpened()){
        __android_log_print(ANDROID_LOG_ERROR, "TAG", "fs write frame");
        fs << fname << image;
    }

    Mat descriptor = getDescriptor(image);

    if (fs.isOpened()) {
        __android_log_print(ANDROID_LOG_ERROR, "TAG", "fs write desc");
        fs << name << descriptor;
    } else __android_log_print(ANDROID_LOG_ERROR, "TAG", "fs is NOT opened");
    fs.release();

    env->ReleaseStringUTFChars(filepath, path);
    env->ReleaseStringUTFChars(descName, name);
    env->ReleaseStringUTFChars(frmName, fname);

}

JNIEXPORT jint JNICALL
Java_com_example_veled_semesterproject_CameraActivity_compareFrames(JNIEnv *env,
                                                                    jobject instance,
                                                                    jlong addr1,jlong addr2,
                                                                    jint count, jstring filepath) {

    const char *path = env->GetStringUTFChars(filepath, NULL);

    Mat & image = *(Mat *) addr1;
    Mat & descriptor2 = *(Mat *) addr2;
    Mat descriptor1;

    descriptor1 = getDescriptor(image);


    std::vector<DMatch> matches12, matches21;
    std::vector<DMatch> goodMatches;
    cv::BFMatcher matcher;

    /*FileStorage fs(path, FileStorage::READ);
    if (fs.isOpened()){
        __android_log_print(ANDROID_LOG_ERROR, "TAG", "fs is opened -- COMPARE");
    }

    fs["descriptor"+count] >> descriptor2;*/

    if (descriptor1.type()==descriptor2.type() && descriptor1.cols == descriptor2.cols) {
        __android_log_print(ANDROID_LOG_ERROR, "TAG", "COLS ARE EQUAL");

        matcher.match(descriptor1, descriptor2, matches12);
        matcher.match(descriptor2, descriptor1, matches21);


        for (int i = 0; i < matches12.size(); i++) {
            DMatch forward = matches12[i];
            DMatch backward = matches21[forward.trainIdx];
            if (backward.trainIdx == forward.queryIdx) {
                goodMatches.push_back(forward);
            }
        }

    } else{
        __android_log_print(ANDROID_LOG_ERROR, "TAG", "COLS ARE NOT EQUAL");
    }


   // fs.release();

    env->ReleaseStringUTFChars(filepath, path);

    return goodMatches.size();

}


}
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
#include <sstream>
#include <future>
#include <thread>
#include <opencv2/highgui/highgui_c.h>


using namespace std;
using namespace cv;



std::map<string, Mat> descriptorMap;


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

template <typename T>
std::string to_string(T value){
    std::ostringstream os;
    os << value;
    return os.str();
}




extern "C" {

JNIEXPORT jboolean JNICALL
Java_com_example_veled_semesterproject_MainActivity_getFromFile(JNIEnv *env, jobject instance,
                                                                jstring path_, jint cnt) {
    const char *path = env->GetStringUTFChars(path_, 0);
    Mat img;

    cv::FileStorage fs(path, FileStorage::READ);
    if (fs.isOpened()) {
        __android_log_print(ANDROID_LOG_ERROR, "TAG", "FS IS OPEN --> FROMFILE");
        for (int i = 0; i < cnt; i++) {
            std::string desc = "descriptor";
            fs[desc+to_string(i)] >> img;
            descriptorMap.insert(std::map<string, Mat>::value_type(desc+to_string(i), img));
            __android_log_print(ANDROID_LOG_ERROR, "TAG", "INSERTING TO MAP--- %d", i);
            __android_log_print(ANDROID_LOG_ERROR, "TAG", "descriptor%d cols: %d", i, img.cols);
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
                                                                 jlong addr, jstring filepath,
                                                                 jstring descName,
                                                                 jstring frmName) {
    Mat &image = *(Mat *) addr;
    const char *path = env->GetStringUTFChars(filepath, NULL);
    const char *name = env->GetStringUTFChars(descName, NULL);
    const char *fname = env->GetStringUTFChars(frmName, NULL);


    cv::FileStorage fs(path, FileStorage::APPEND);

    Mat descriptor = getDescriptor(image);

    if (fs.isOpened()) {
        __android_log_print(ANDROID_LOG_ERROR, "TAG", "fs write desc");
        fs << name << descriptor;
    } else __android_log_print(ANDROID_LOG_ERROR, "TAG", "fs is NOT opened");

    if (fs.isOpened()) {
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
                                                                    jlong addr1, jint index) {


    Mat &image = *(Mat *) addr1;
    Mat descriptor2;
    Mat descriptor1;

    descriptor1 = getDescriptor(image);
    std::vector<DMatch> matches12, matches21;
    std::vector<DMatch> goodMatches;
    cv::BFMatcher matcher;


        descriptor2 = descriptorMap.at("descriptor"+to_string(index));
    __android_log_print(ANDROID_LOG_ERROR, "TAG", "descriptor1 size:  %d", descriptor1.rows);
    __android_log_print(ANDROID_LOG_ERROR, "TAG", "descriptor2 size:  %d", descriptor2.rows);

    if (descriptor1.type() == descriptor2.type() && descriptor1.cols == descriptor2.cols) {
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
            __android_log_print(ANDROID_LOG_ERROR, "TAG", "GoodMatch Size: -- %d, k value: %d", goodMatches.size(), index);
        } else __android_log_print(ANDROID_LOG_ERROR, "TAG", "COLS ARE NOT EQUAL -- %d", descriptor2.cols);

    return goodMatches.size();

    }
}
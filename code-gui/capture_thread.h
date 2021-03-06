// mode: c++
#ifndef CAPTURE_THREAD_H
#define CAPTURE_THREAD_H

#include <QString>
#include <QThread>
#include <QMutex>
#include <QElapsedTimer>

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/video/background_segm.hpp>

using namespace std;

class CaptureThread : public QThread
{
    Q_OBJECT
public:
    CaptureThread(int camera, QMutex *lock);
    CaptureThread(QString videoPath, QMutex *lock);
    ~CaptureThread();
    void setRunning(bool run) {running = run; }
    void startCalcFPS() {fps_calculating = true; }
    enum VideoSavingStatus {
                            STARTING,
                            STARTED,
                            STOPPING,
                            STOPPED
    };

    void setVideoSavingStatus(VideoSavingStatus status) {video_saving_status = status; }
    void setImageProcessingStatus(bool status) {
        image_processing_status = status;
        target_detected = false;
        if(video_saving_status != STOPPED) video_saving_status = STOPPING;
    }

protected:
    void run() override;

signals:
    void frameCaptured(cv::Mat *data);
    void fpsChanged(float fps);
    void videoSaved(QString name);

private:
    void calculateFPS(cv::VideoCapture &cap);
    void startSavingVideo(cv::Mat &firstFrame);
    void stopSavingVideo();
    void imageProcessing(cv::Mat &frame);
    void getRandomColors(vector<cv::Scalar>& colors, int numColors);

private:
    bool running;
    int cameraID;
    QString videoPath;
    QMutex *data_lock;
    cv::Mat frame;

    // FPS calculating
    bool fps_calculating;
    float fps;

    // video saving
    int frame_width, frame_height;
    VideoSavingStatus video_saving_status;
    QString saved_video_name;
    cv::VideoWriter *video_writer;

    // image analysis
    bool image_processing_status;
    bool target_detected;
};

#endif // CAPTURE_THREAD_H

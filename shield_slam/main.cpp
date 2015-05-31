#include "VSlam.hpp"
#include <opencv2/opencv.hpp>

#include "UpdateListener.hpp"
#include "Visualizer.hpp"
#include "MapPoint.hpp"

using namespace cv;
using namespace std;
using namespace vslam;

class VisualizerListener : public UpdateListener {
    
public:
	void update(vector<MapPoint> global_map, vector<MapPoint> local_map, Mat camera_rot, Mat camera_pos) {
        
        vector<Point3d> init_pc;
        vector<Point3d> kf_pc;
        
        for (int i=0; i<global_map.size(); i++)
        {
            init_pc.push_back(global_map.at(i).GetPoint3D());
        }
        
        for (int i=0; i<local_map.size(); i++)
        {
            kf_pc.push_back(local_map.at(i).GetPoint3D());
        }
        
// GLOBAL MAP:
       UpdateCloud(init_pc, 0, 255, 0, true);
        
// LOCAL MAP:
//       UpdateCloud(kf_pc, 0, 0, 255, true); 
        
// GLOBAL MAP & LOCAL MAP:
//      UpdateCloud(init_pc, 0, 255, 0, true);
//		UpdateCloud(kf_pc, 0, 0, 255, false);
        
        AddCamera(camera_rot, camera_pos);
	}
};

int main(int argc, char** argv)
{
//    VideoCapture cap("/Users/MohitSridhar/NCSV/Stanford/CS231M/projects/shield_slam/indoor.avi");
//    VideoCapture cap("/Users/MohitSridhar/Downloads/kitti_youtube.avi");
    VideoCapture cap("/Users/MohitSridhar/Downloads/VID_20150530_120719.mp4");
    
    if (!cap.isOpened())
    {
        cout << "failed to open video file" << endl;
        return -1;
    }

    Ptr<VisualizerListener> visualizerListener = new VisualizerListener;
    InitializeVisualizer();
    
    vslam::VSlam slam = vslam::VSlam();
    
    // Initialize:
    Mat frame;
    cap >> frame;
    
    Size size(640, 480);
    resize(frame, frame, size);
    
    // Load Initialized Map:
    RunVisualizationOnly();
    
    slam.ProcessFrame(frame);
    
    for (int i=0; i<1; i++)
    {
        cap >> frame;
    }
    
    for ( ; ; )
    {
        cap >> frame;
        if (frame.empty())
            break;
        
        
        resize(frame, frame, size);
        slam.ProcessFrame(frame);
        
        if (waitKey(30) == 27)
        {
            break;
        }
        
        visualizerListener->update(slam.GetGlobalMap(), slam.GetCurrKeyFrame().GetMap(), slam.GetCameraRot().back(), slam.GetCameraPose().back());
        
        RunVisualizationOnly();
        
        Mat tracked_features;
        drawKeypoints(frame, slam.GetCurrKeyFrame().GetTrackedKeypoints(), tracked_features, Scalar(255, 0, 0));
        imshow("Tracked Features", tracked_features);
        
        waitKey(0);
    }
    
    waitKey(0);

//    WaitForVisualizationThread();
    return 0;
}
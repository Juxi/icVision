#define TRAINING 1 //0 FOR TESTING
#define ASK_USER 0
#define DURATION_RUN 100 //in seconds

//Segmenting objects
#define POSES_PER_OBJECT_STATIC 1   // When ASK_USER is 0, you just leave the icub running and allow it to focuss objects in the scene and segment and save the results
#define POSES_PER_OBJECT_DYNAMIC 15 //When ASK_USER is 1, when icub focesses on an object, you rotate the object to acquire many poses
//NOTE: Images will be saved in a directory indicated by 'pathResults' variable in main.cpp. You have to set the path before running the code
#define FEATURE_COUNT_TRESHOLD 5
#define OBJECT_LIMIT 30

//Saliency
#define FOVEA 1
#define FOVEA_BOUNDARY 120.0 //elliptic boundary condition for attention in the scene.
#define SALIENCY_INHIBIT_CENTER 4 //How much you want to suppress the saliency in the center to move to a new location. Higher the number higher suppression and longer time taken. Note: You need not change this unless you really feel so.

//Features
#define SELECT_FEATURE 1
/* Select the following number for features
HARRIS_CORNER 1
FAST_FEATURE 2
STAR_FEATURE 3
MSER_FEATURE 4
SIFT_FEATURE 5
SURF_FEATURE 6 */

//matching features between left and right images to use it later in segmentation.
#define SIM_THRESHOLD 0.95 // 1.0 for best match(no false alarms+few correct matches) and 0.0 for worst match(lot of false alarms+all correct matches)
#define HOR_DISP_THRESHOLD 100.0//threshold in x axis to remove false alarms
#define VER_DISP_THRESHOLD 10.0//threshold in y axis to remove false alarms NOTE: vertical disparity in cameras due to manufacturing defect is already corrected (in the code)

//Segmentation
#define CLUSTERXY 15.0 //clustering threshold in pixels in XY plane to pool the features into one group to declare as belonging to one object.
#define CLUSTERDEPTH 5.1 // clustering threshold in pixels in Depth (the way it is done is bit different from the above. It is the relative measure of two close by points on left and right images)
#define WIDTHSEGMENTATION 20.0 //consider the segment within this distance from the center of the image after robot focusses on the object to the center
#define TOPDOWNTHRESHOLD 50.0 //inhibit locations less than this distance from visited locations. This will avoid the robot learning the same object again.
#define INHIBIT_VISITED_LOCATIONS 1

//In the motion code you can play with the following numbers
/*#define AZIMUTH_RANGE 80 //when robot moves outside this angle it comes back. This is to avoid objects out of the view you wanted.
#define ELEVATION_RANGE 50 //same thing as above but in vertical direction
#define BOUNDARY_CHECK 1 // do you want to do the boundary check?
#define DISPLAY_VALUES 1 //Display motor values
#define RECOIL_FACTOR 0.66 //When it goes outside the boundary, by howmuch amount you want to come back into your field of view.
#define BIND_NECK_ROLL 0 // If you don't want neck roll
*/


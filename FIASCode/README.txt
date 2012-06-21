Hi all, 

Code is organized as follows:

'scripts' folder contains .xml to run camera and stuffs. ikingazecontrol is essential.

src folder contains 3 folders: MainCode, Motion and yarpImageToHTTP.

MainCode has 4 modules(segmentation, saliency, featureprocessing and camera) and a main file.
All the necessary parameters one can play with for the code are in the file /src/MainCode/include/iCub/variables.h

MotionCode is for controlling and moving the head. you can take out the part of the code in LookAtLocation::run() (along with the variables and functions) and add it your motion code with torso. Parameters for the motion code are in main.h. 

Execute the code in the following order:
Run the Motion code first. If icub was not in the home position it comes to the home position. Type 'track' once it comes to the home position and prompts you to type 'track'. Then run the Maincode. Maincode runs for a certain amount of time(what you feed in variables.h) and then exits. You can also exit the code with 'ctrl C'(but results will be incomplete). After closing Maincode go the terminal of Motion code and type 'home' to go to home position or 'end' to safely exit. It sometimes gives a segmentation fault in the end. Never mind. You can run the code again without facing any problems. You will be anyway taking only the necessary parts of that code and put into yours.

Output:
When the code completes running you will find the folder /Images created that contains 4 kind of files: original image with object in the center objectN_imageM.png(where N is the object index and M is the pose index), image with features objectN_featuresM.png, objectN_fileFeaturesInM.txt containing the feature locations on the object, objectN_fileFeaturesInM.txt containing feature locations on the background(Simon has the piece of code for getting the binary segmentation from these locations). Don't forget to set the path 'pathResults' in main.cpp

What does the code do:
with #define ASK_USER 0 (default) in variables.h, code does the following:
icub looks for salient objects in the seen, changes its gaze, segment the object on which salient point falls, moves its gaze again to the center of the object, obtain the features and save them onto files. Then it looks for the next salient point in the scene and the process repeats. Whenever icub shifts its attention you can change the object pose or remove the object or replace it by some other object. Keep doing it until you are done with all the objects and their poses. 

Watch teh video attached and get a feel:
The sample video with my email explains the above better. On the left hand side from top to bottom you see respectively the original images, feature images and the feature matching. On the right hand side top you see saliency images at course scale. You also see an ellipse that tells you the attention area. It is done so that icub doesn't jump its attention to outside crazily. You can also set AZIMUTH_RANGE and ELEVATION_RANGE in Motion code for restricting your view to locations of your interest(it depends on where the table and objects are located). What you in the middle of the screen is segmented images. white dots are those on the current object (the other dots also you can use. Just look for them in the segmentation.cpp if you want). What you see on right hand side bottom is visited locations of the robot that are masked for further attention in near future. parameters for all these things can be manipulated in variables.h file. 

Through the course of the video you see objects being segmented one after the other and a sample of the object pic is shown at the bottom for your reference to know what all objects are segmented already. I also keep changeing the objects pose when the code is running. When the code finishes running it display each object and asks the user for a number. Just type on the image 1 or 2 or 3 or whatever. Whenever a different pose of the same object appears you should give the appropriate number. In the end the renamed files are saved in /Images/Renamed

Code can also do:
You can also leave the code running for 10 minutes or so keeping all the objects in the view and allow robot to segment them all one by one. Meanwhile go have coffee come back and type the object numbers!

With #define ASK_USER 1, the things are done in the slightly different manner. When icub looks at an object, press 's' or otherwise to skip or use the object. Once the object is chosen it gives you some time to show the various poses of the object. Then it moves its attention to the other object. 

With #define TRAINING 0, you can run the code in testing mode. icub just keeping looking at objects in some order. You can check if your recognition algorithm is working well. 

NOTE:
1. images from teh camera are 320X240. It is hard coded everywhere because we never used bigger size for our algorithms. It is quite some work for you if you want to change. I advice to use the code as it is.
2. The way outputs are saved etc can be changed in segmentObject(), saveFeatures() in segmentation.cpp
3. If you don't like to see the displays and stuffs you have swith them off in .h files. There are many #define variables
4. You can obtain the object 3D locations in Motion code in the variable ' locations3DSave'. Every alternate location it saves is the object location. You can use it for grasping the object. It probably helps for a demo. My colleague has checked our code on full icub at IIT. Grasping works very fine using object location from our code.
5. You can feed the .ini file in Motion code with calibrated values for your icub. Currently it is empty because i don't need any .ini for my algorithm.

Thanks much. Feel free to get back to me if you want any other info. 

Have fun! 
Cheers :)
Pramod

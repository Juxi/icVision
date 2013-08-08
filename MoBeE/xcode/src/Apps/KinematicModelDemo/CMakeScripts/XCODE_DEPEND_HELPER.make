# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# For each target create a dummy rule so the target does not have to exist
/Users/kailfrank/imClever/dev/MoBeE/xcode/src/LibKinematicModel/Debug/libKinematicModel.a:
/usr/local/lib/libsolid.dylib:
/Users/kailfrank/imClever/dev/MoBeE/xcode/src/LibKinematicModel/MinSizeRel/libKinematicModel.a:
/Users/kailfrank/imClever/dev/MoBeE/xcode/src/LibKinematicModel/RelWithDebInfo/libKinematicModel.a:
/Users/kailfrank/imClever/dev/MoBeE/xcode/src/LibKinematicModel/Release/libKinematicModel.a:


# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.KinematicModelDemo.Debug:
PostBuild.KinematicModel.Debug: /Users/kailfrank/imClever/dev/MoBeE/xcode/src/Apps/KinematicModelDemo/Debug/KinematicModelDemo
/Users/kailfrank/imClever/dev/MoBeE/xcode/src/Apps/KinematicModelDemo/Debug/KinematicModelDemo:\
	/Users/kailfrank/imClever/dev/MoBeE/xcode/src/LibKinematicModel/Debug/libKinematicModel.a\
	/usr/local/lib/libsolid.dylib
	/bin/rm -f /Users/kailfrank/imClever/dev/MoBeE/xcode/src/Apps/KinematicModelDemo/Debug/KinematicModelDemo


PostBuild.KinematicModelDemo.Release:
PostBuild.KinematicModel.Release: /Users/kailfrank/imClever/dev/MoBeE/xcode/src/Apps/KinematicModelDemo/Release/KinematicModelDemo
/Users/kailfrank/imClever/dev/MoBeE/xcode/src/Apps/KinematicModelDemo/Release/KinematicModelDemo:\
	/Users/kailfrank/imClever/dev/MoBeE/xcode/src/LibKinematicModel/Release/libKinematicModel.a\
	/usr/local/lib/libsolid.dylib
	/bin/rm -f /Users/kailfrank/imClever/dev/MoBeE/xcode/src/Apps/KinematicModelDemo/Release/KinematicModelDemo


PostBuild.KinematicModelDemo.MinSizeRel:
PostBuild.KinematicModel.MinSizeRel: /Users/kailfrank/imClever/dev/MoBeE/xcode/src/Apps/KinematicModelDemo/MinSizeRel/KinematicModelDemo
/Users/kailfrank/imClever/dev/MoBeE/xcode/src/Apps/KinematicModelDemo/MinSizeRel/KinematicModelDemo:\
	/Users/kailfrank/imClever/dev/MoBeE/xcode/src/LibKinematicModel/MinSizeRel/libKinematicModel.a\
	/usr/local/lib/libsolid.dylib
	/bin/rm -f /Users/kailfrank/imClever/dev/MoBeE/xcode/src/Apps/KinematicModelDemo/MinSizeRel/KinematicModelDemo


PostBuild.KinematicModelDemo.RelWithDebInfo:
PostBuild.KinematicModel.RelWithDebInfo: /Users/kailfrank/imClever/dev/MoBeE/xcode/src/Apps/KinematicModelDemo/RelWithDebInfo/KinematicModelDemo
/Users/kailfrank/imClever/dev/MoBeE/xcode/src/Apps/KinematicModelDemo/RelWithDebInfo/KinematicModelDemo:\
	/Users/kailfrank/imClever/dev/MoBeE/xcode/src/LibKinematicModel/RelWithDebInfo/libKinematicModel.a\
	/usr/local/lib/libsolid.dylib
	/bin/rm -f /Users/kailfrank/imClever/dev/MoBeE/xcode/src/Apps/KinematicModelDemo/RelWithDebInfo/KinematicModelDemo



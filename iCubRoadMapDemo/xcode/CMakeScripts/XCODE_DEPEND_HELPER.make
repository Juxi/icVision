# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to, avoiding a bug in XCode 1.5
all.Debug: \
	/Users/kail/imClever/dev/iCubRoadMapDemo/xcode/Debug/RoadmapDemo

all.Release: \
	/Users/kail/imClever/dev/iCubRoadMapDemo/xcode/Release/RoadmapDemo

all.MinSizeRel: \
	/Users/kail/imClever/dev/iCubRoadMapDemo/xcode/MinSizeRel/RoadmapDemo

all.RelWithDebInfo: \
	/Users/kail/imClever/dev/iCubRoadMapDemo/xcode/RelWithDebInfo/RoadmapDemo

# For each target create a dummy rule so the target does not have to exist
/opt/local/lib/libCGAL_Core.dylib:
/opt/local/lib/libCGAL.dylib:
/opt/local/lib/libgmpxx.dylib:
/opt/local/lib/libmpfr.dylib:
/opt/local/lib/libgmp.dylib:
/opt/local/lib/libboost_thread-mt-d.dylib:
/Users/kail/Library/yarp-2.3.4/lib/libYARP_OS.a:
/Users/kail/Library/yarp-2.3.4/lib/libYARP_sig.a:
/Users/kail/Library/yarp-2.3.4/lib/libYARP_math.a:
/Users/kail/Library/yarp-2.3.4/lib/libYARP_dev.a:
/Users/kail/Library/yarp-2.3.4/lib/libYARP_init.a:
/opt/local/lib/libgsl.dylib:
/opt/local/lib/libgslcblas.dylib:
/usr/local/lib/libACE.dylib:
/opt/local/lib/libboost_thread-mt.dylib:


# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
/Users/kail/imClever/dev/iCubRoadMapDemo/xcode/Debug/RoadmapDemo:\
	/opt/local/lib/libCGAL_Core.dylib\
	/opt/local/lib/libCGAL.dylib\
	/opt/local/lib/libgmpxx.dylib\
	/opt/local/lib/libmpfr.dylib\
	/opt/local/lib/libgmp.dylib\
	/opt/local/lib/libboost_thread-mt-d.dylib\
	/opt/local/lib/libCGAL_Core.dylib\
	/opt/local/lib/libCGAL.dylib\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_OS.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_sig.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_math.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_dev.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_init.a\
	/opt/local/lib/libgmpxx.dylib\
	/opt/local/lib/libmpfr.dylib\
	/opt/local/lib/libgmp.dylib\
	/opt/local/lib/libboost_thread-mt-d.dylib\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_math.a\
	/opt/local/lib/libgsl.dylib\
	/opt/local/lib/libgslcblas.dylib\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_dev.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_sig.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_OS.a\
	/usr/local/lib/libACE.dylib
	/bin/rm -f /Users/kail/imClever/dev/iCubRoadMapDemo/xcode/Debug/RoadmapDemo


/Users/kail/imClever/dev/iCubRoadMapDemo/xcode/Release/RoadmapDemo:\
	/opt/local/lib/libCGAL_Core.dylib\
	/opt/local/lib/libCGAL.dylib\
	/opt/local/lib/libgmpxx.dylib\
	/opt/local/lib/libmpfr.dylib\
	/opt/local/lib/libgmp.dylib\
	/opt/local/lib/libboost_thread-mt.dylib\
	/opt/local/lib/libCGAL_Core.dylib\
	/opt/local/lib/libCGAL.dylib\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_OS.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_sig.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_math.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_dev.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_init.a\
	/opt/local/lib/libgmpxx.dylib\
	/opt/local/lib/libmpfr.dylib\
	/opt/local/lib/libgmp.dylib\
	/opt/local/lib/libboost_thread-mt.dylib\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_math.a\
	/opt/local/lib/libgsl.dylib\
	/opt/local/lib/libgslcblas.dylib\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_dev.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_sig.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_OS.a\
	/usr/local/lib/libACE.dylib
	/bin/rm -f /Users/kail/imClever/dev/iCubRoadMapDemo/xcode/Release/RoadmapDemo


/Users/kail/imClever/dev/iCubRoadMapDemo/xcode/MinSizeRel/RoadmapDemo:\
	/opt/local/lib/libCGAL_Core.dylib\
	/opt/local/lib/libCGAL.dylib\
	/opt/local/lib/libgmpxx.dylib\
	/opt/local/lib/libmpfr.dylib\
	/opt/local/lib/libgmp.dylib\
	/opt/local/lib/libboost_thread-mt.dylib\
	/opt/local/lib/libCGAL_Core.dylib\
	/opt/local/lib/libCGAL.dylib\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_OS.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_sig.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_math.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_dev.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_init.a\
	/opt/local/lib/libgmpxx.dylib\
	/opt/local/lib/libmpfr.dylib\
	/opt/local/lib/libgmp.dylib\
	/opt/local/lib/libboost_thread-mt.dylib\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_math.a\
	/opt/local/lib/libgsl.dylib\
	/opt/local/lib/libgslcblas.dylib\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_dev.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_sig.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_OS.a\
	/usr/local/lib/libACE.dylib
	/bin/rm -f /Users/kail/imClever/dev/iCubRoadMapDemo/xcode/MinSizeRel/RoadmapDemo


/Users/kail/imClever/dev/iCubRoadMapDemo/xcode/RelWithDebInfo/RoadmapDemo:\
	/opt/local/lib/libCGAL_Core.dylib\
	/opt/local/lib/libCGAL.dylib\
	/opt/local/lib/libgmpxx.dylib\
	/opt/local/lib/libmpfr.dylib\
	/opt/local/lib/libgmp.dylib\
	/opt/local/lib/libboost_thread-mt.dylib\
	/opt/local/lib/libCGAL_Core.dylib\
	/opt/local/lib/libCGAL.dylib\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_OS.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_sig.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_math.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_dev.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_init.a\
	/opt/local/lib/libgmpxx.dylib\
	/opt/local/lib/libmpfr.dylib\
	/opt/local/lib/libgmp.dylib\
	/opt/local/lib/libboost_thread-mt.dylib\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_math.a\
	/opt/local/lib/libgsl.dylib\
	/opt/local/lib/libgslcblas.dylib\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_dev.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_sig.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_OS.a\
	/usr/local/lib/libACE.dylib
	/bin/rm -f /Users/kail/imClever/dev/iCubRoadMapDemo/xcode/RelWithDebInfo/RoadmapDemo



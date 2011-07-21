# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to, avoiding a bug in XCode 1.5
all.Debug: \
	/Users/kail/imClever/dev/VirtualSkin2/src/VirtualSkinLibrary/KinematicModel/xcode/WorldModelDemo/Debug/WorldModelDemo

all.Release: \
	/Users/kail/imClever/dev/VirtualSkin2/src/VirtualSkinLibrary/KinematicModel/xcode/WorldModelDemo/Release/WorldModelDemo

all.MinSizeRel: \
	/Users/kail/imClever/dev/VirtualSkin2/src/VirtualSkinLibrary/KinematicModel/xcode/WorldModelDemo/MinSizeRel/WorldModelDemo

all.RelWithDebInfo: \
	/Users/kail/imClever/dev/VirtualSkin2/src/VirtualSkinLibrary/KinematicModel/xcode/WorldModelDemo/RelWithDebInfo/WorldModelDemo

# For each target create a dummy rule so the target does not have to exist
/Users/kail/imClever/dev/VirtualSkin2/src/VirtualSkinLibrary/KinematicModel/xcode/LibKinematicModel/Debug/libKinematicModel.a:
/Users/kail/Library/FreeSOLID-3.5.6/src/.libs/libsolid.dylib:
/Users/kail/imClever/dev/VirtualSkin2/src/VirtualSkinLibrary/KinematicModel/xcode/LibKinematicModel/MinSizeRel/libKinematicModel.a:
/Users/kail/imClever/dev/VirtualSkin2/src/VirtualSkinLibrary/KinematicModel/xcode/LibKinematicModel/RelWithDebInfo/libKinematicModel.a:
/Users/kail/imClever/dev/VirtualSkin2/src/VirtualSkinLibrary/KinematicModel/xcode/LibKinematicModel/Release/libKinematicModel.a:


# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
/Users/kail/imClever/dev/VirtualSkin2/src/VirtualSkinLibrary/KinematicModel/xcode/WorldModelDemo/Debug/WorldModelDemo:\
	/Users/kail/imClever/dev/VirtualSkin2/src/VirtualSkinLibrary/KinematicModel/xcode/LibKinematicModel/Debug/libKinematicModel.a\
	/Users/kail/Library/FreeSOLID-3.5.6/src/.libs/libsolid.dylib
	/bin/rm -f /Users/kail/imClever/dev/VirtualSkin2/src/VirtualSkinLibrary/KinematicModel/xcode/WorldModelDemo/Debug/WorldModelDemo


/Users/kail/imClever/dev/VirtualSkin2/src/VirtualSkinLibrary/KinematicModel/xcode/WorldModelDemo/Release/WorldModelDemo:\
	/Users/kail/imClever/dev/VirtualSkin2/src/VirtualSkinLibrary/KinematicModel/xcode/LibKinematicModel/Release/libKinematicModel.a\
	/Users/kail/Library/FreeSOLID-3.5.6/src/.libs/libsolid.dylib
	/bin/rm -f /Users/kail/imClever/dev/VirtualSkin2/src/VirtualSkinLibrary/KinematicModel/xcode/WorldModelDemo/Release/WorldModelDemo


/Users/kail/imClever/dev/VirtualSkin2/src/VirtualSkinLibrary/KinematicModel/xcode/WorldModelDemo/MinSizeRel/WorldModelDemo:\
	/Users/kail/imClever/dev/VirtualSkin2/src/VirtualSkinLibrary/KinematicModel/xcode/LibKinematicModel/MinSizeRel/libKinematicModel.a\
	/Users/kail/Library/FreeSOLID-3.5.6/src/.libs/libsolid.dylib
	/bin/rm -f /Users/kail/imClever/dev/VirtualSkin2/src/VirtualSkinLibrary/KinematicModel/xcode/WorldModelDemo/MinSizeRel/WorldModelDemo


/Users/kail/imClever/dev/VirtualSkin2/src/VirtualSkinLibrary/KinematicModel/xcode/WorldModelDemo/RelWithDebInfo/WorldModelDemo:\
	/Users/kail/imClever/dev/VirtualSkin2/src/VirtualSkinLibrary/KinematicModel/xcode/LibKinematicModel/RelWithDebInfo/libKinematicModel.a\
	/Users/kail/Library/FreeSOLID-3.5.6/src/.libs/libsolid.dylib
	/bin/rm -f /Users/kail/imClever/dev/VirtualSkin2/src/VirtualSkinLibrary/KinematicModel/xcode/WorldModelDemo/RelWithDebInfo/WorldModelDemo



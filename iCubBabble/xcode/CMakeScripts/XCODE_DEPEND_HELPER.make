# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to, avoiding a bug in XCode 1.5
all.Debug: \
	/Users/kail/imClever/dev/iCubBabble/xcode/Debug/iCubBabble

all.Release: \
	/Users/kail/imClever/dev/iCubBabble/xcode/Release/iCubBabble

all.MinSizeRel: \
	/Users/kail/imClever/dev/iCubBabble/xcode/MinSizeRel/iCubBabble

all.RelWithDebInfo: \
	/Users/kail/imClever/dev/iCubBabble/xcode/RelWithDebInfo/iCubBabble

# For each target create a dummy rule so the target does not have to exist
/Users/kail/Library/yarp-2.3.4/lib/libYARP_OS.a:
/Users/kail/Library/yarp-2.3.4/lib/libYARP_sig.a:
/Users/kail/Library/yarp-2.3.4/lib/libYARP_math.a:
/Users/kail/Library/yarp-2.3.4/lib/libYARP_dev.a:
/Users/kail/Library/yarp-2.3.4/lib/libYARP_init.a:
/opt/local/lib/libgsl.dylib:
/opt/local/lib/libgslcblas.dylib:
/usr/local/lib/libACE.dylib:


# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
/Users/kail/imClever/dev/iCubBabble/xcode/Debug/iCubBabble:\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_OS.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_sig.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_math.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_dev.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_init.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_math.a\
	/opt/local/lib/libgsl.dylib\
	/opt/local/lib/libgslcblas.dylib\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_dev.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_sig.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_OS.a\
	/usr/local/lib/libACE.dylib
	/bin/rm -f /Users/kail/imClever/dev/iCubBabble/xcode/Debug/iCubBabble


/Users/kail/imClever/dev/iCubBabble/xcode/Release/iCubBabble:\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_OS.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_sig.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_math.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_dev.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_init.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_math.a\
	/opt/local/lib/libgsl.dylib\
	/opt/local/lib/libgslcblas.dylib\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_dev.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_sig.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_OS.a\
	/usr/local/lib/libACE.dylib
	/bin/rm -f /Users/kail/imClever/dev/iCubBabble/xcode/Release/iCubBabble


/Users/kail/imClever/dev/iCubBabble/xcode/MinSizeRel/iCubBabble:\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_OS.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_sig.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_math.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_dev.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_init.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_math.a\
	/opt/local/lib/libgsl.dylib\
	/opt/local/lib/libgslcblas.dylib\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_dev.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_sig.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_OS.a\
	/usr/local/lib/libACE.dylib
	/bin/rm -f /Users/kail/imClever/dev/iCubBabble/xcode/MinSizeRel/iCubBabble


/Users/kail/imClever/dev/iCubBabble/xcode/RelWithDebInfo/iCubBabble:\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_OS.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_sig.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_math.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_dev.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_init.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_math.a\
	/opt/local/lib/libgsl.dylib\
	/opt/local/lib/libgslcblas.dylib\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_dev.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_sig.a\
	/Users/kail/Library/yarp-2.3.4/lib/libYARP_OS.a\
	/usr/local/lib/libACE.dylib
	/bin/rm -f /Users/kail/imClever/dev/iCubBabble/xcode/RelWithDebInfo/iCubBabble




FRAMEWORKS_FLAGS=-F/System/Library/Frameworks/Carbon.framework/Frameworks
#FRAMEWORKS_FLAGS=-F/System/Library/Frameworks

RANLIB = ranlib

L = CameraControl.o MacICACameraControl.o MacICA_inventory.o MacICA_debug.o
libcamio.a: $L
	@rm -f libcamio.a
	ar cqv libcamio.a $L
	$(RANLIB) libcamio.a

FRAMEWORKS=-framework Carbon
O = main.o 

camctl: $O
	$(CXX) $(FRAMEWORKS_FLAGS) -o camctl $O -L. -lcamio $(FRAMEWORKS)

main.o: main.cc CameraControl.h
	$(CXX) $(FRAMEWORKS_FLAGS) -c  main.cc

CameraControl.o: CameraControl.cc CameraControl.h
	$(CXX) $(FRAMEWORKS_FLAGS) -c  CameraControl.cc

MacICACameraControl.o: MacICACameraControl.cc MacICACameraControl.h CameraControl.h
	$(CXX) $(FRAMEWORKS_FLAGS) -c  MacICACameraControl.cc

MacICA_inventory.o: MacICA_inventory.cc MacICACameraControl.h CameraControl.h
	$(CXX) $(FRAMEWORKS_FLAGS) -c  MacICA_inventory.cc

MacICA_debug.o: MacICA_debug.cc MacICACameraControl.h CameraControl.h
	$(CXX) $(FRAMEWORKS_FLAGS) -c  MacICA_debug.cc


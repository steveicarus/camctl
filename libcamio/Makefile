
FRAMEWORKS_FLAGS=-F/System/Library/Frameworks/Carbon.framework/Frameworks
#FRAMEWORKS_FLAGS=-F/System/Library/Frameworks

FRAMEWORKS=-framework Carbon
O = main.o CameraControl.o MacICACameraControl.o MacICA_inventory.o MacICA_debug.o

camctl: $O
	$(CXX) $(FRAMEWORKS_FLAGS) -o camctl $O $(FRAMEWORKS)

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


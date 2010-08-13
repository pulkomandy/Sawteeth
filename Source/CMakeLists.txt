include_directories(..)

add_executable(Sawteeth stAboutView.cpp stAboutWindow.cpp stAiffWindow.cpp stApp.cpp stBpView.cpp stChRulerView.cpp stChannelView.cpp stCurveControl.cpp stCurveKnob.cpp stCurveView.cpp stInstrumentWindow.cpp stMainWindow.cpp stPartSquare.cpp stPartWindow.cpp stSeqView.cpp stTrackerControl.cpp stTrackerEntry.cpp stTrackerView.cpp stsp.cpp)

add_custom_command(TARGET Sawteeth POST_BUILD COMMAND xres -o Sawteeth ../../Source/Resource.rsrc DEPENDS Resource.rsrc)

target_link_libraries(Sawteeth ST be tracker media translation)

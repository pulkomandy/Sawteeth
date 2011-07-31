include_directories(..)

add_executable(Sawteeth stAboutView.cpp stAboutWindow.cpp stAiffWindow.cpp stApp.cpp stBpView.cpp
stChRulerView.cpp stChannelView.cpp stCurveControl.cpp stCurveKnob.cpp stCurveView.cpp
stInstrumentWindow.cpp stMainWindow.cpp stPartSquare.cpp stPartWindow.cpp stSeqView.cpp
stTrackerControl.cpp stTrackerEntry.cpp stTrackerView.cpp stsp.cpp
PeakView.cpp
)

add_custom_command(TARGET Sawteeth POST_BUILD 
	COMMAND rc -o Resource.rsrc ${CMAKE_SOURCE_DIR}/Source/Resource.rdef
	COMMAND xres -o Sawteeth Resource.rsrc
	DEPENDS Resource.rdef)

target_link_libraries(Sawteeth ST be tracker media translation)

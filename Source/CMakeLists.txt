include_directories(..)

# Avoid getting the executable in Source/ folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(Sawteeth
	stAboutView.cpp stAboutWindow.cpp
	stAiffWindow.cpp
	stApp.cpp
	stMainWindow.cpp stPartSquare.cpp stBpView.cpp stChannelView.cpp stSeqView.cpp
	stCurveControl.cpp stCurveKnob.cpp stCurveView.cpp
	stInstrumentWindow.cpp stPartWindow.cpp
	stTrackerControl.cpp stTrackerEntry.cpp stTrackerView.cpp
	stsp.cpp
	PeakView.cpp
)

add_custom_command(TARGET Sawteeth POST_BUILD 
	COMMAND rc -o Resource.rsrc ${CMAKE_SOURCE_DIR}/Source/Resource.rdef
	COMMAND xres -o ${CMAKE_BINARY_DIR}/Sawteeth Resource.rsrc
	DEPENDS Resource.rdef)

target_link_libraries(Sawteeth ST be tracker media translation)

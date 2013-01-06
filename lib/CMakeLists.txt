add_library(ST SHARED ${STLIB_SOURCES})
set_target_properties(ST PROPERTIES
	COMPILE_FLAGS -Os
	LINK_FLAGS -Os
)

add_library(ST SHARED source/ins_part_channel.cpp source/insply.cpp source/lfo.cpp source/player.cpp source/song.cpp source/txt.cpp source/wave.cpp)
set_target_properties(ST PROPERTIES
	COMPILE_FLAGS -Os
	LINK_FLAGS -Os
)

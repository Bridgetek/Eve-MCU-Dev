set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPROGMEM=")
set(COMPONENT_SRCS "main.c" 
                    "../../common/eve_calibrate.c" 
                    "../../common/eve_example.c" 
                    "../../common/eve_fonts.c" 
                    "../../common/eve_images.c" 
                    "../../common/eve_helper.c")
                    
set(COMPONENT_ADD_INCLUDEDIRS "../../../../include" "../../common" "../../../../ports/eve_bt82x")

register_component()
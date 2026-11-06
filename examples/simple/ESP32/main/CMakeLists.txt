set(COMPONENT_NAME "eve_example")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPROGMEM= -DEVE_DEBUG_LEVEL=2")
                    
idf_component_register(SRCS 
                    "main.c" 
                    # Source for example code
                    "../../common/eve_example.c" 
                    "../../common/eve_fonts.c" 
                    "../../common/eve_images.c"
                    # Snippet files to add to example
                    "../../../snippets/touch.c" 
                    # Platform specific snippets
                    "../../../snippets/platform/platform_time.c"
                    "../../../snippets/platform/calib_esp32.c"
                    INCLUDE_DIRS 
                    "../../../../include" 
                    "../../common" 
                    "../../../snippets"
)

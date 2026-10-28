set(COMPONENT_NAME "eve_example")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPROGMEM= -DEVE_DEBUG_LEVEL=2")
                    
idf_component_register(SRCS 
                    "main.c" 
                    "../../../snippets/touch.c" 
                    "../../common/eve_example.c" 
                    "../../common/eve_fonts.c" 
                    "../../common/eve_images.c"
                    INCLUDE_DIRS 
                    "../../../../include" 
                    "../../common" 
                    "../../../snippets"
)

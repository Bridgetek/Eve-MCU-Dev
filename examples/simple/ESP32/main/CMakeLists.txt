set(COMPONENT_NAME "eve_example")
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPROGMEM=")
                    
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

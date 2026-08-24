set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPROGMEM=")
set(COMPONENT_SRCS "main.c" 
                    "../../../snippets/touch.c" 
                    "../../common/eve_example.c" 
                    "../../common/eve_fonts.c" 
                    "../../common/eve_images.c")
                    
set(COMPONENT_ADD_INCLUDEDIRS ${CMAKE_CURRENT_SOURCE_DIR} "../../../../include" "../../common" "../../../snippets")

idf_component_register()
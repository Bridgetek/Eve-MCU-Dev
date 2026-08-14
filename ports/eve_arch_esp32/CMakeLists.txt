# Tell EVE library to use ESP32 as base class

set(COMPONENT_NAME "eve_mcu_esp32")
# Tell EVE library to use ESP32 as base class
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPLATFORM_ESP32= -DDEBUG_LEVEL=1")

idf_component_register(SRCS 
                        "EVE_MCU_ESP32.c" 
                        "../../source/EVE_HAL.c" 
                        "../../source/EVE_API.c" 
                        # Add base patch for BT82x only
                        "../../source/extensions/patch_base.c"
                        # Add custom touch code FT81x/BT88x/BT81x only
                        "../../source/extensions/custom_touch.c"
                       INCLUDE_DIRS "../../include"
                       REQUIRES driver soc)
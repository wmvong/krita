set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories(   ${CMAKE_CURRENT_SOURCE_DIR}/..  ${CMAKE_SOURCE_DIR}/sdk/tests ${CMAKE_CURRENT_BINARY_DIR}/..)

macro_add_unittest_definitions()

include(KritaAddBrokenUnitTest)

ecm_add_tests(
    kis_animation_utils_test.cpp
    timeline_model_test.cpp
    NAME_PREFIX "plugins-dockers-animation-"
    LINK_LIBRARIES ${KDE4_KDEUI_LIBS} kritaanimationdocker kritaui kritaimage Qt5::Test)

    

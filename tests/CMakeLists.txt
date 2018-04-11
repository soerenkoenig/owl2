add_executable(testrunner
        main.cpp
        image/image.cpp
        utils/count_iterator.cpp
        utils/filter_iterator.cpp
        utils/handle.cpp
        utils/linear_index.cpp
        utils/non_copyable.cpp
        utils/stop_watch.cpp

        color/color.cpp
        color/color_maps.cpp

        math/angle.cpp
        math/interval.cpp
        math/matrix.cpp
        math/mesh.cpp
        math/nplane.cpp
        math/quaternion.cpp
        math/ray.cpp
        )
target_link_libraries(testrunner PRIVATE owl)
target_include_directories(testrunner PRIVATE $(PROJECT_SOURCE_DIR)/../thirdparty)


add_custom_command(
        TARGET testrunner PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/tests/data/images
        ${EXECUTABLE_OUTPUT_PATH}/images
        COMMENT "Copying image test data."
)
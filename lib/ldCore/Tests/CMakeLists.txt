SET(${PROJECT_NAME}_SRC
    ${CMAKE_CURRENT_LIST_DIR}/ldCore/Visualizations/util/MathsHelper/ldMathsTests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ldCore/Visualizations/util/MathsHelper/ldMathsTests.h
    )
ADD_EXECUTABLE(${PROJECT_NAME}Tests ${${PROJECT_NAME}_SRC} )

find_package(Qt5 CONFIG REQUIRED Test)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}Tests ${PROJECT_NAME} ${TEST_LIBRARIES} Qt5::Test)

ADD_TEST( ${PROJECT_NAME}Tests  ${PROJECT_NAME}Tests )

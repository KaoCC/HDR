


#includes


include_directories(${HDRI_DIR}/include)
include_directories( ${OpenCV_INCLUDE_DIRS} )



set(BASE_SRC
    ./include/ReinhardAlgo.hpp
    ./include/LinearLeastSquares.hpp
    ./include/DebevecWeight.hpp
    ./include/WeightFunction.hpp
    ./include/rawImage.hpp
    ./include/ToneMapAlgo.hpp
    ./include/Common.hpp
    ./include/HDRImage.hpp

    ./src/Common.cpp
    ./src/HDRImage.cpp
    ./src/LinearLeastSquares.cpp
    ./src/DebevecWeight.cpp
    ./src/rawImage.cpp
    ./src/ReinhardAlgo.cpp
    ./src/main.cpp
)


add_executable(
    hdri

    ${BASE_SRC}
)

target_link_libraries( hdri ${OpenCV_LIBS} )


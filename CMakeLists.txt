add_library(htu21d src/htu21d.c src/htu21d_i2c.c)
target_include_directories(htu21d PUBLIC include PRIVATE src)
target_link_libraries(htu21d PRIVATE swo)

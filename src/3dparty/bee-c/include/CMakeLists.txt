if(NOT DEFINED CMAKE_INSTALL_INCLUDEDIR)
    set(CMAKE_INSTALL_INCLUDEDIR include)
endif(NOT DEFINED CMAKE_INSTALL_INCLUDEDIR)

# installation of bee C client library headers
install(DIRECTORY bee DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

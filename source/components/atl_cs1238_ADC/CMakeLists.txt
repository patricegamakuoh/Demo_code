idf_component_register(SRCS "."
                       INCLUDE_DIRS "include"
					   PRIV_REQUIRES main)
                       
# Import the library, specifying a target name and the library path.
# The private dependencies of the library is also specified.
add_prebuilt_library(prebuilt "lib.a")
# `main` calls a function from the library, so link it to `main`
target_link_libraries(${COMPONENT_LIB} PRIVATE prebuilt)
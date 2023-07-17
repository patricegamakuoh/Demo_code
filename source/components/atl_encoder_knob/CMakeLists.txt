# set(COMPONENT_SRCS "uart_fuc.c")

# set(COMPONENT_ADD_INCLUDEDIRS ". include")

# register_component()

idf_component_register(SRCS "atl_encoder.c"
                       INCLUDE_DIRS "include"
					   PRIV_REQUIRES main)
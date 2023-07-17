# idf_component_register(SRCS
#                        INCLUDE_DIRS "include"
# 					   PRIV_REQUIRES main)
idf_component_register(SRCS "atl_max6675.c"
                       INCLUDE_DIRS "include"
					   PRIV_REQUIRES main)
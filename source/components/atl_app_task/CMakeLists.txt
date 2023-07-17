# set(COMPONENT_SRCS "atl_app_task_2.c" "atl_app_task_1.c" "uart_fuc.c")

# set(COMPONENT_ADD_INCLUDEDIRS ". include")

# register_component()

idf_component_register(SRCS "atl_LLTA_task.c" "atl_RPID_task.c"
                       INCLUDE_DIRS "include"
					   PRIV_REQUIRES main)
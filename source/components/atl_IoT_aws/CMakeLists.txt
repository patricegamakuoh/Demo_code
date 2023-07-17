set(COMPONENT_ADD_INCLUDEDIRS "port/include aws-iot-device-sdk-embedded-C/include")
set(aws_sdk_dir aws-iot-device-sdk-embedded-C/src)
set(COMPONENT_SRCS "aws_iot.c" "${aws_sdk_dir}/aws_iot_jobs_interface.c"
                   "${aws_sdk_dir}/aws_iot_jobs_json.c"
                   "${aws_sdk_dir}/aws_iot_jobs_topics.c"
                   "${aws_sdk_dir}/aws_iot_jobs_types.c"
                   "${aws_sdk_dir}/aws_iot_json_utils.c"
                   "${aws_sdk_dir}/aws_iot_mqtt_client.c"
                   "${aws_sdk_dir}/aws_iot_mqtt_client_common_internal.c"
                   "${aws_sdk_dir}/aws_iot_mqtt_client_connect.c"
                   "${aws_sdk_dir}/aws_iot_mqtt_client_publish.c"
                   "${aws_sdk_dir}/aws_iot_mqtt_client_subscribe.c"
                   "${aws_sdk_dir}/aws_iot_mqtt_client_unsubscribe.c"
                   "${aws_sdk_dir}/aws_iot_mqtt_client_yield.c"
                   "${aws_sdk_dir}/aws_iot_shadow.c"
                   "${aws_sdk_dir}/aws_iot_shadow_actions.c"
                   "${aws_sdk_dir}/aws_iot_shadow_json.c"
                   "${aws_sdk_dir}/aws_iot_shadow_records.c"
                   "port/network_mbedtls_wrapper.c"
                   "port/threads_freertos.c"
                   "port/timer.c"
				   
				   INCLUDE_DIRS "include"
                   PRIV_REQUIRES main
				   )	   			   

set(COMPONENT_REQUIRES "mbedtls")
set(COMPONENT_PRIV_REQUIRES "jsmn")
set(COMPONENT_REQUIRES "main")


# register_component()




set(srcs mqtt_lib.c)

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES "mqtt")
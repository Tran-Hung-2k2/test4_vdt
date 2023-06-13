set(srcs dht22_lib.c)
# set(pri_req)
idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES "driver")
set(srcs "src/adc_read.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS ".."
                    REQUIRES driver)

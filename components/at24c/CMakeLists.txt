# get IDF version for comparison
set(idf_version "${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}")

if(idf_version VERSION_GREATER_EQUAL "5.2")
	set(component_srcs "at24c_new.c")
else()
	set(component_srcs "at24c_legacy.c")
endif()

idf_component_register(SRCS "${component_srcs}" PRIV_REQUIRES driver esp_driver_i2c INCLUDE_DIRS ".")

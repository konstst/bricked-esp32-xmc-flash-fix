idf_component_register(
	SRCS
		"main.c"
		"xmc_flash_fix"
	INCLUDE_DIRS
		"include"
	REQUIRES
		driver
	PRIV_REQUIRES spi_flash
)

###################################################
# Sources
###################################################

SIO_SRC = middleware/MTK/sio/src
SIO_FILES = $(SIO_SRC)/sio/sio_main.c \
            $(SIO_SRC)/sio_uart/sio_uart_adapter.c \
            $(SIO_SRC)/sio_uart/sio_uart_main.c \
            $(SIO_SRC)/sio_uart/sio_uart_port.c \
            $(SIO_SRC)/sio_uart/sio_uart_ut.c \
            $(SIO_SRC)/msm/msm_eint.c \
            $(SIO_SRC)/msm/msm_gpio.c \
            $(SIO_SRC)/urc_app.c

C_FILES += $(SIO_FILES)

ifeq ($(MTK_CMUX_SUPPORT),y)
CMUX_FILES += $(SIO_SRC)/cmux/cmux.c \
              $(SIO_SRC)/cmux/cmux_cch.c \
              $(SIO_SRC)/cmux/cmux_create.c \
              $(SIO_SRC)/cmux/cmux_dummy.c \
              $(SIO_SRC)/cmux/cmux_hdlr.c \
              $(SIO_SRC)/cmux/cmux_lib.c \
              $(SIO_SRC)/cmux/cmux_main.c \
              $(SIO_SRC)/cmux/cmux_prot.c \
              $(SIO_SRC)/cmux/cmux_setting.c \
              $(SIO_SRC)/cmux/cmux_uh.c \
              $(SIO_SRC)/cmux/cmux_utl.c \
              $(SIO_SRC)/cmux/cmux_vp.c
              
C_FILES += $(CMUX_FILES)
endif

###################################################
# include path
###################################################

CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/sio/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/sio/inc/sio
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/sio/inc/sio_uart
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/sio/inc/msm

ifeq ($(MTK_CMUX_SUPPORT),y)
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/sio/inc/cmux
endif

###################################################
# definition
###################################################
ifeq ($(MTK_CMUX_SUPPORT),y)
CFLAGS+= -D__CMUX_SUPPORT__
endif


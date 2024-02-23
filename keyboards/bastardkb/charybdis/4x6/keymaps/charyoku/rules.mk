VIA_ENABLE = yes
REPEAT_KEY_ENABLE = yes
CAPS_WORD_ENABLE = yes
COMBO_ENABLE = yes

# MACCEL
MACCEL_ENABLE = yes
;MACCEL_VIA_ENABLE = yes
ifeq ($(strip $(VIA_ENABLE)), yes)
   	OPT_DEFS += -DMACCEL_ENABLE
	SRC += ./maccel/maccel.c
	ifeq ($(strip $(MACCEL_VIA_ENABLE)), yes)
		SRC += ./maccel/maccel_via.c
	endif
endif

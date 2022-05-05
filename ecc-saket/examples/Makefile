CONTIKI = ../..
APPS+=powertrace


CONTIKI_ECC_DIR = ..

all: alice bob alice_ecdsa bob_ecdsa messages



DOMAIN_PARAMS = SECP128R1

PROJECT_SOURCEFILES += messages.c
CONTIKI_WITH_RIME = 1
include $(CONTIKI_ECC_DIR)/Makefile.contikiecc

include $(CONTIKI)/Makefile.include

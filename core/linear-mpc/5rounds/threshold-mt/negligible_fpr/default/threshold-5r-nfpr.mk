CURRENT_MODE_NAME:=TCITH_MT_5R_NFPR
MODE_LIST:= $(MODE_LIST) $(CURRENT_MODE_NAME)
MODE_$(CURRENT_MODE_NAME)_PATH=api-sign,linear-mpc/5rounds/threshold-mt/negligible_fpr/default,trees/hash

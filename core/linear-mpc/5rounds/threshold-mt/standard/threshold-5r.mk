CURRENT_MODE_NAME:=TCITH_MT_5R
MODE_LIST:= $(MODE_LIST) $(CURRENT_MODE_NAME)
MODE_$(CURRENT_MODE_NAME)_PATH=api-sign,linear-mpc/5rounds/threshold-mt/standard,trees/hash
MODE_$(CURRENT_MODE_NAME)_MPC_UNITS_PATH=linear-mpc/units,linear-mpc/5rounds/units

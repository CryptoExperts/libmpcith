CURRENT_MODE_NAME:=HYPERCUBE_5R
MODE_LIST:= $(MODE_LIST) $(CURRENT_MODE_NAME)
MODE_$(CURRENT_MODE_NAME)_PATH=api-sign,linear-mpc/5rounds/additive/hypercube,trees/seeds/default
MODE_$(CURRENT_MODE_NAME)_MPC_UNITS_PATH=linear-mpc/units,linear-mpc/5rounds/units

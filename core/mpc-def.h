#ifndef LIBMPCITH_MPC_DEF_H
#define LIBMPCITH_MPC_DEF_H

#ifndef MPC_DEBUG
#define __MPC_BEGIN__ {}
#define __MPC_END__ {}
#define __MPC_PARAM__(name,value) {}
#define __MPC_INPUT_VAR__(name,var) {}
#define __MPC_INPUT_VAR_2__(name,var,size) {}
#define __MPC_INTERMEDIATE_VAR__(name,var) {}
#define __MPC_INTERMEDIATE_VAR_2_(name,var,size) {}
#define __MPC_CHALLENGE__(name,var) {}
#define __MPC_CHALLENGE_2__(name,var,size) {}
#define __MPC_INST_VAR__(name,var) {}
#define __MPC_INST_VAR_2__(name,var,size) {}
#else
#include <stdio.h>
#define MPC_LOG_FILENAME "mpc.log"
static inline void reset_mpc_log(void) {
    FILE* fptr = fopen(MPC_LOG_FILENAME, "w");
    if(fptr == NULL)
        return;
    fclose(fptr);
}
static inline void mpc_log(const char* label, const char* name, const void* var, unsigned int size) {
    FILE* fptr = fopen(MPC_LOG_FILENAME, "a");
    if(fptr == NULL)
        return;

    fputs(label, fptr);
    fprintf(fptr, " - ");
    fputs(name, fptr);
    fprintf(fptr, " - ");
    for(unsigned int i=0; i<size; i++) {
        fprintf(fptr, "%d.", ((uint8_t*)var)[i]);
    }
    fprintf(fptr, "\n");
    fclose(fptr);
}
static inline void mpc_log_value(const char* label, const char* name, int value) {
    FILE* fptr = fopen(MPC_LOG_FILENAME, "a");
    if(fptr == NULL)
        return;

    fputs(label, fptr);
    fprintf(fptr, " - ");
    fputs(name, fptr);
    fprintf(fptr, " - %d \n", value);
    fclose(fptr);
}
#define __MPC_BEGIN__ mpc_log("MPC_BEGIN","","",0)
#define __MPC_END__ mpc_log("MPC_END","","",0)
#define __MPC_PARAM__(name,value) mpc_log_value("PARAM",name,value)
#define __MPC_INPUT_VAR__(name,var) mpc_log("INPUT",name,var,sizeof(var))
#define __MPC_INPUT_VAR_2_(name,var,size) mpc_log("INPUT",name,var,size)
#define __MPC_INTERMEDIATE_VAR__(name,var) mpc_log("INTER",name,var,sizeof(var))
#define __MPC_INTERMEDIATE_VAR_2__(name,var,size) mpc_log("INTER",name,var,size)
#define __MPC_CHALLENGE__(name,var) mpc_log("CH",name,var,sizeof(var))
#define __MPC_CHALLENGE_2__(name,var,size) mpc_log("CH",name,var,size)
#define __MPC_INST_VAR__(name,var) mpc_log("INST",name,var,sizeof(var))
#define __MPC_INST_VAR_2__(name,var,size) mpc_log("INST",name,var,size)
#endif

#endif /* LIBMPCITH_MPC_DEF_H */

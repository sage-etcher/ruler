#ifndef EXTERN_C_HELPER_H
#define EXTERN_C_HELPER_H

/* C++ extern C declaration macros */
#ifdef __cplusplus
#define EXTERN_C_START() extern "C" {
#define EXTERN_C_END()   }
#else
#define EXTERN_C_START() ;
#define EXTERN_C_END()   ;
#endif


#endif /* EXTER_C_HELPER_H */
/* end-of-file */

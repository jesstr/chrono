#ifndef __COMMON_H__
#define __COMMON_H__

#define STR(x)  XSTR(x)
#define XSTR(x)  #x

#define MAX(x , y)  (((x) > (y)) ? (x) : (y))
#define array_len(x) (sizeof(x)/sizeof(x[0]))

#define isInterrupt() ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0)

#endif /* __COMMON_H__ */
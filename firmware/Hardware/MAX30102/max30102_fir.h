#ifndef __MAX30102_FIR_H
#define __MAX30102_FIR_H


void max30102_fir_init(void);
void ir_max30102_fir(float *input,float *output);
void red_max30102_fir(float *input,float *output);
#endif /* __MAX30102_FIR_H */


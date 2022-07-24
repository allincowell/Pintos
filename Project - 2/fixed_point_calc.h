#ifndef __THREAD_FIXED_POINT_CALC_H
#define __THREAD_FIXED_POINT_CALC_H

/* Basic definitions of fixed point. */
typedef int fixed_t;
/* 16 LSB used for fractional part. */
#define SHIFT 14

/* Some helpful macros. */
/* Convert a value to fixed-point value. */
#define CONSTANT(A) ((fixed_t)(A << SHIFT))
/* Add two fixed-point value. */
#define ADD(A,B) (A + B)
/* Add a fixed-point value A and an int value B. */
#define ADD_FP_INT(A,B) (A + (B << SHIFT))
/* Substract two fixed-point value. */
#define SUB(A,B) (A - B)
/* Substract an int value B from a fixed-point value A */
#define SUB_FP_INT(A,B) (A - (B << SHIFT))
/* Multiply a fixed-point value A by an int value B. */
#define MULT_FP_INT(A,B) (A * B)
/* Multiply two fixed-point value. */
#define MULT(A,B) ((fixed_t)((((int64_t) A) * B) >> SHIFT))
/* Divide a fixed-point value A by an int value B. */
#define DIV_FP_INT(A,B) (A / B)
/* Divide two fixed-point value. */
#define DIV(A,B) ((fixed_t)((((int64_t) A) << SHIFT) / B))
/* Get integer part of a fixed-point value. */
#define TRUNCATE(A) (A >> SHIFT)
/* Get rounded integer of a fixed-point value. */
#define ROUND_OFF(A) (A >= 0 ? ((A + (1 << (SHIFT - 1))) >> SHIFT) \
                            : ((A - (1 << (SHIFT - 1))) >> SHIFT))

#endif /* thread/fixed_point_calc.h */

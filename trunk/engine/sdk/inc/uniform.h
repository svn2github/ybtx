#pragma once
#include "CommonDefs.h"

namespace sqr
{

int COMMON_API  congruence ( int a, int b, int c, bool *error );
double COMMON_API  d_epsilon ( void );
int COMMON_API  d_nint ( double x );
double COMMON_API  d_normal ( double a, double b, int *seed );
double COMMON_API  d_normal_01 ( int *seed );
double COMMON_API  d_uniform ( double a, double b, int *seed );
double COMMON_API  d_uniform_01 ( int *seed );
void COMMON_API  d_uniform_write ( int m, int n, int seed, double r[], char *file_out_name );
char COMMON_API  digit_to_ch ( int i );
void COMMON_API  dmat_uniform ( int m, int n, double a, double b, int *seed, double r[] );
void COMMON_API  dmat_uniform_01 ( int m, int n, int *seed, double r[] );
double COMMON_API  *dvec_normal ( int n, double a, double b, int *seed );
double COMMON_API  *dvec_normal_01 ( int n, int *seed );
double COMMON_API  *dvec_uniform ( int n, double a, double b, int *seed );
double COMMON_API  *dvec_uniform_01 ( int n, int *seed );
int COMMON_API  get_seed ( void );
int COMMON_API  i_gcd ( int i, int j );
int COMMON_API  i_log_10 ( int i );
int COMMON_API  i_max ( int i1, int i2 );
int COMMON_API  i_min ( int i1, int i2 );
int COMMON_API  i_sign ( int i );
void COMMON_API  i_swap ( int *i, int *j );
char COMMON_API  *i_to_s ( int i );
int COMMON_API  i_uniform ( int a, int b, int *seed );
void COMMON_API  ivec_uniform ( int n, int a, int b, int *seed, int x[] );
int COMMON_API  lcrg_seed ( int a, int b, int c, int n, int seed );
int COMMON_API  power_mod ( int a, int n, int m );
float COMMON_API  r_uniform ( float a, float b, int *seed );
float COMMON_API  r_uniform_01 ( int *seed );
void COMMON_API  timestamp ( void );
char COMMON_API  *timestring ( void );

}

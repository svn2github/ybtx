#include "stdafx.h"
# include <cstdlib>
# include <iostream>
# include <iomanip>
# include <fstream>
# include <ctime>
# include <cmath>

using namespace std;

# include "uniform.h"

namespace sqr
{

	//******************************************************************************

	int congruence ( int a, int b, int c, bool *error )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    CONGRUENCE solves a congruence of the form A * X = C ( mod B ).
		//
		//  Discussion:
		//
		//    A, B and C are given integers.  The equation is solvable if and only
		//    if the greatest common divisor of A and B also divides C.
		//
		//  Modified:
		//
		//    15 November 2004
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Reference:
		//
		//    Eric Weisstein, editor,
		//    CRC Concise Encylopedia of Mathematics,
		//    CRC Press, 1998, page 446.
		//
		//  Parameters:
		//
		//    Input, int A, B, C, the coefficients of the Diophantine equation.
		//
		//    Output, bool *GERROR, error flag, is TRUE if an error occurred..
		//
		//    Output, int CONGRUENCE, the solution of the Diophantine equation.
		//    X will be between 0 and B-1.
		//
	{
# define N_MAX 100

		int a_copy;
		int a_mag;
		int a_sign;
		int b_copy;
		int b_mag;
		int b_sign;
		int c_copy;
		int g;
		int k;
		int n;
		int q[N_MAX];
		bool swap;
		int x;
		int y;
		//
		//  Defaults for output parameters.
		//
		*error = false;
		x = 0;
		y = 0;
		//
		//  Special cases.
		//
		if ( a == 0 && b == 0 && c == 0 )
		{
			return 0;
		}
		else if ( a == 0 && b == 0 && c != 0 )
		{
			*error = true;
			return (-1);
		}
		else if ( a == 0 && b != 0 && c == 0 )
		{
			return 0;
		}
		else if ( a == 0 && b != 0 && c != 0 )
		{
			if ( ( c % b ) != 0 )
			{
				*error = true;
				return (-1);
			}
			return 0;
		}
		else if ( a != 0 && b == 0 && c == 0 )
		{
			return 0;
		}
		else if ( a != 0 && b == 0 && c != 0 )
		{
			x = c / a;
			if ( ( c % a ) != 0 )
			{
				*error = true;
				return (-1);
			}
			return x;
		}
		else if ( a != 0 && b != 0 && c == 0 )
		{
			g = i_gcd ( a, b );
			x = b / g;
			return x;
		}
		//
		//  Now handle the "general" case: A, B and C are nonzero.
		//
		//  Step 1: Compute the GCD of A and B, which must also divide C.
		//
		g = i_gcd ( a, b );

		if ( ( c % g ) != 0 )
		{
			*error = true;
			return (-1);
		}

		a_copy = a / g;
		b_copy = b / g;
		c_copy = c / g;
		//
		//  Step 2: Split A and B into sign and magnitude.
		//
		a_mag = abs ( a_copy );
		a_sign = i_sign ( a_copy );
		b_mag = abs ( b_copy );
		b_sign = i_sign ( b_copy );
		//
		//  Another special case, A_MAG = 1 or B_MAG = 1.
		//
		if ( a_mag == 1 )
		{
			x = a_sign * c_copy;
			return x;
		}
		else if ( b_mag == 1 )
		{
			return 0;
		}
		//
		//  Step 3: Produce the Euclidean remainder sequence.
		//
		if ( b_mag <= a_mag )
		{
			swap = false;
			q[0] = a_mag;
			q[1] = b_mag;
		}
		else
		{
			swap = true;
			q[0] = b_mag;
			q[1] = a_mag;
		}

		n = 3;

		for ( ; ; )
		{
			q[n-1] = ( q[n-3] % q[n-2] );

			if ( q[n-1] == 1 )
			{
				break;
			}

			n = n + 1;

			if ( N_MAX < n )
			{
				*error = true;
				cout << "\n";
				cout << "CONGRUENCE - Fatal error!\n";
				cout << "  Exceeded number of iterations.\n";
				exit ( 1 );
			}

		}
		//
		//  Step 4: Now go backwards to solve X * A_MAG + Y * B_MAG = 1.
		//
		y = 0;
		for ( k = n; 2 <= k; k-- )
		{
			x = y;
			y = ( 1 - x * q[k-2] ) / q[k-1];
		}
		//
		//  Step 5: Undo the swapping.
		//
		if ( swap )
		{
			i_swap ( &x, &y );
		}
		//
		//  Step 6: Now apply signs to X and Y so that X * A + Y * B = 1.
		//
		x = x * a_sign;
		//
		//  Step 7: Multiply by C, so that X * A + Y * B = C.
		//
		x = x * c_copy;
		//
		//  Step 8: Now force 0 <= X < B.
		//
		x = x % b;
		//
		//  Step 9: Force positivity.
		//
		if ( x < 0 )
		{
			x = x + b;
		}

		return x;
# undef N_MAX
	}
	//******************************************************************************

	double d_epsilon ( void )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    D_EPSILON returns the round off unit for double precision arithmetic.
		//
		//  Discussion:
		//
		//    D_EPSILON is a number R which is a power of 2 with the property that,
		//    to the precision of the computer's arithmetic,
		//      1 < 1 + R
		//    but 
		//      1 = ( 1 + R / 2 )
		//
		//  Modified:
		//
		//    01 July 2004
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    Output, double D_EPSILON, the double precision round-off unit.
		//
	{
		double r;

		r = 1.0E+00;

		while ( 1.0E+00 < ( double ) ( 1.0E+00 + r )  )
		{
			r = r / 2.0E+00;
		}

		return ( 2.0E+00 * r );
	}
	//******************************************************************************

	int d_nint ( double x )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    D_NINT returns the nearest integer to a double precision value.
		//
		//  Examples:
		//
		//        X         D_NINT
		//
		//      1.3         1
		//      1.4         1
		//      1.5         1 or 2
		//      1.6         2
		//      0.0         0
		//     -0.7        -1
		//     -1.1        -1
		//     -1.6        -2
		//
		//  Modified:
		//
		//    26 August 2004
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    Input, double X, the value.
		//
		//    Output, int D_NINT, the nearest integer to X.
		//
	{
		int s;

		if ( x < 0.0 )
		{
			s = -1;
		}
		else
		{
			s = 1;
		}

		return ( s * ( int ) ( fabs ( x ) + 0.5 ) );
	}
	//******************************************************************************

	double d_normal ( double a, double b, int *seed )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    D_NORMAL returns a scaled normally distributed pseudorandom value.
		//
		//  Discussion:
		//
		//    The normal probability distribution function (PDF) is sampled,
		//    with mean A and standard deviation B.
		//
		//  Modified:
		//
		//    21 November 2004
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    Input, double A, the mean of the PDF.
		//
		//    Input, double B, the standard deviation of the PDF.
		//
		//    Input/output, int *SEED, a seed for the random number generator.
		//
		//    Output, double D_NORMAL, a sample of the normal PDF.
		//
	{
		double value;

		value = a + b * d_normal_01 ( seed );

		return value;
	}
	//**********************************************************************

	double d_normal_01 ( int *seed )

		//**********************************************************************
		//
		//  Purpose:
		//
		//    D_NORMAL_01 samples the unit normal probability distribution.
		//
		//  Discussion:
		//
		//    The standard normal probability distribution function (PDF) has 
		//    mean 0 and standard deviation 1.
		//
		//  Method:
		//
		//    The Box-Muller method is used, which is efficient, but 
		//    generates two values at a time.
		//
		//  Modified:
		//
		//    16 November 2004
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    Input/output, int *SEED, a seed for the random number generator.
		//
		//    Output, double D_NORMAL_01, a normally distributed random value.
		//
	{
# define PI 3.141592653589793

		double r1;
		double r2;
		static int used = -1;
		static int seed2 = 0;
		double x;
		static double y = 0.0;

		if ( used == -1 )
		{
			used = 0;
		}
		//
		//  If we've used an even number of values so far, generate two more, return one,
		//  and save one.
		//
		if ( ( used % 2 )== 0 )
		{
			r1 = d_uniform_01 ( seed );

			if ( r1 == 0.0 )
			{
				cout << "\n";
				cout << "D_NORMAL_01 - Fatal error!\n";
				cout << "  D_UNIFORM_01 returned a value of 0.\n";
				exit ( 1 );
			}

			seed2 = *seed;
			r2 = d_uniform_01 ( &seed2 );

			x = sqrt ( -2.0 * log ( r1 ) ) * cos ( 2.0 * PI * r2 );
			y = sqrt ( -2.0 * log ( r1 ) ) * sin ( 2.0 * PI * r2 );
		}
		//
		//  Otherwise, return the second, saved, value and the corresponding
		//  value of SEED.
		//
		else
		{
			x = y;
			*seed = seed2;
		}

		used = used + 1;

		return x;
# undef PI
	}
	//******************************************************************************

	double d_uniform ( double b, double c, int *seed )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    D_UNIFORM returns a scaled double precision pseudorandom number.
		//
		//  Discussion:
		//
		//    The pseudorandom number should be uniformly distributed
		//    between A and B.
		//
		//  Modified:
		//
		//    21 November 2004
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    Input, double B, C, the limits of the interval.
		//
		//    Input/output, int *SEED, the "seed" value, which should NOT be 0.
		//    On output, SEED has been updated.
		//
		//    Output, double D_UNIFORM, a number strictly between A and B.
		//
	{
		double value;

		value = b + ( c - b ) * d_uniform_01 ( seed );

		return value;
	}
	//******************************************************************************

	double d_uniform_01 ( int *seed )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    D_UNIFORM_01 returns a unit double precision pseudorandom number.
		//
		//  Discussion:
		//
		//    This routine implements the recursion
		//
		//      seed = 16807 * seed mod ( 2**31 - 1 )
		//      d_uniform_01 = seed / ( 2**31 - 1 )
		//
		//    The integer arithmetic never requires more than 32 bits,
		//    including a sign bit.
		//
		//    If the initial seed is 12345, then the first three computations are
		//
		//      Input     Output      D_UNIFORM_01
		//      SEED      SEED
		//
		//         12345   207482415  0.096616
		//     207482415  1790989824  0.833995
		//    1790989824  2035175616  0.947702
		//
		//  Modified:
		//
		//    11 August 2004
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Reference:
		//
		//    Paul Bratley, Bennett Fox, L E Schrage,
		//    A Guide to Simulation,
		//    Springer Verlag, pages 201-202, 1983.
		//
		//    Pierre L'Ecuyer,
		//    Random Number Generation,
		//    in Handbook of Simulation
		//    edited by Jerry Banks,
		//    Wiley Interscience, page 95, 1998.
		//
		//    Bennett Fox,
		//    Algorithm 647:
		//    Implementation and Relative Efficiency of Quasirandom
		//    Sequence Generators,
		//    ACM Transactions on Mathematical Software,
		//    Volume 12, Number 4, pages 362-376, 1986.
		//
		//    P A Lewis, A S Goodman, J M Miller,
		//    A Pseudo-Random Number Generator for the System/360,
		//    IBM Systems Journal,
		//    Volume 8, pages 136-143, 1969.
		//
		//  Parameters:
		//
		//    Input/output, int *SEED, the "seed" value.  Normally, this
		//    value should not be 0.  On output, SEED has been updated.
		//
		//    Output, double D_UNIFORM_01, a new pseudorandom variate, strictly between
		//    0 and 1.
		//
	{
		int k;
		double r;

		k = *seed / 127773;

		*seed = 16807 * ( *seed - k * 127773 ) - k * 2836;

		if ( *seed < 0 )
		{
			*seed = *seed + 2147483647;
		}
		//
		//  Although SEED can be represented exactly as a 32 bit integer,
		//  it generally cannot be represented exactly as a 32 bit real number!
		//
		r = ( double ) ( *seed ) * 4.656612875E-10;

		return r;
	}
	//******************************************************************************

	void d_uniform_write ( int m, int n, int seed, double r[], char *file_out_name )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    D_UNIFORM_WRITE writes a double precision uniform dataset to a file.
		//
		//  Discussion:
		//
		//    The initial lines of the file are comments, which begin with a
		//    "#" character.
		//
		//    Thereafter, each line of the file contains the NDIM-dimensional
		//    components of the next entry of the sequence.
		//
		//  Modified:
		//
		//    24 July 2004
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    Input, int M, the spatial dimension.
		//
		//    Input, int N, the number of elements in the subsequence.
		//
		//    Input, int SEED[M], the initial seed.
		//
		//    Input, double R[M*N], the points.
		//
		//    Input, char *FILE_OUT_NAME, the name of the output file.
		//
	{
		ofstream file_out;
		int i;
		int j;
		char *s;

		file_out.open ( file_out_name );

		if ( !file_out )
		{
			cout << "\n";
			cout << "D_UNIFORM_WRITE - Fatal error!\n";
			cout << "  Could not open the output file.\n";
			exit ( 1 );
		}

		s = timestring ( );

		file_out << "#  " << file_out_name << "\n";
		file_out << "#  created by routine D_UNIFORM_WRITE.C" << "\n";
		file_out << "#  at " << s << "\n";
		file_out << "#\n";
		file_out << "#  M =    " << setw(12) << m    << "\n";
		file_out << "#  N =    " << setw(12) << n    << "\n";
		file_out << "#  SEED = " << setw(12) << seed << "\n";
		file_out << "#  EPSILON (unit roundoff) = " << d_epsilon ( ) << "\n";
		file_out << "#\n";

		for ( j = 0; j < n; j++ )
		{
			for ( i = 0; i < m; i++ )
			{
				file_out << setw(10) << r[i+j*m] << "  ";
			}
			file_out << "\n";
		}

		file_out.close ( );

		delete [] s;

		return;
	}
	//******************************************************************************

	char digit_to_ch ( int i )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    DIGIT_TO_CH returns the base 10 digit character corresponding to a digit.
		//
		//  Example:
		//
		//     I     C
		//   -----  ---
		//     0    '0'
		//     1    '1'
		//   ...    ...
		//     9    '9'  
		//    10    '*'
		//   -83    '*'
		//
		//  Modified:
		//
		//    16 June 2003
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    Input, int I, the digit, which should be between 0 and 9.
		//
		//    Output, char DIGIT_TO_CH, the appropriate character '0' through '9' or '*'.
		//
	{
		char c;

		if ( 0 <= i && i <= 9 )
		{
			c = '0' + (char)i;
		}
		else
		{
			c = '*';
		}

		return c;
	}
	//******************************************************************************

	void dmat_uniform ( int m, int n, double b, double c, int *seed, double r[] )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    DMAT_UNIFORM fills a double precision array with scaled pseudorandom values.
		//
		//  Discussion:
		//
		//    This routine implements the recursion
		//
		//      seed = 16807 * seed mod ( 2**31 - 1 )
		//      unif = seed / ( 2**31 - 1 )
		//
		//    The integer arithmetic never requires more than 32 bits,
		//    including a sign bit.
		//
		//  Modified:
		//
		//    30 January 2005
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Reference:
		//
		//    Paul Bratley, Bennett Fox, L E Schrage,
		//    A Guide to Simulation,
		//    Springer Verlag, pages 201-202, 1983.
		//
		//    Bennett Fox,
		//    Algorithm 647:
		//    Implementation and Relative Efficiency of Quasirandom
		//    Sequence Generators,
		//    ACM Transactions on Mathematical Software,
		//    Volume 12, Number 4, pages 362-376, 1986.
		//
		//    P A Lewis, A S Goodman, J M Miller,
		//    A Pseudo-Random Number Generator for the System/360,
		//    IBM Systems Journal,
		//    Volume 8, pages 136-143, 1969.
		//
		//  Parameters:
		//
		//    Input, int M, N, the number of rows and columns.
		//
		//    Input, double B, C, the limits of the pseudorandom values.
		//
		//    Input/output, int *SEED, the "seed" value.  Normally, this
		//    value should not be 0, otherwise the output value of SEED
		//    will still be 0, and D_UNIFORM will be 0.  On output, SEED has 
		//    been updated.
		//
		//    Output, double DMAT_UNIFORM[M*N], a matrix of pseudorandom values.
		//
	{
		int i;
		int j;
		int k;

		for ( j = 0; j < n; j++ )
		{
			for ( i = 0; i < m; i++ )
			{
				k = *seed / 127773;

				*seed = 16807 * ( *seed - k * 127773 ) - k * 2836;

				if ( *seed < 0 )
				{
					*seed = *seed + 2147483647;
				}
				//
				//  Although SEED can be represented exactly as a 32 bit integer,
				//  it generally cannot be represented exactly as a 32 bit real number!
				//
				r[i+j*m] = b + ( c - b ) * ( double ) ( *seed ) * 4.656612875E-10;
			}
		}

		return;
	}
	//******************************************************************************

	void dmat_uniform_01 ( int m, int n, int *seed, double r[] )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    DMAT_UNIFORM_01 fills a double precision array with unit pseudorandom values.
		//
		//  Discussion:
		//
		//    This routine implements the recursion
		//
		//      seed = 16807 * seed mod ( 2**31 - 1 )
		//      unif = seed / ( 2**31 - 1 )
		//
		//    The integer arithmetic never requires more than 32 bits,
		//    including a sign bit.
		//
		//  Modified:
		//
		//    11 August 2004
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Reference:
		//
		//    Paul Bratley, Bennett Fox, L E Schrage,
		//    A Guide to Simulation,
		//    Springer Verlag, pages 201-202, 1983.
		//
		//    Bennett Fox,
		//    Algorithm 647:
		//    Implementation and Relative Efficiency of Quasirandom
		//    Sequence Generators,
		//    ACM Transactions on Mathematical Software,
		//    Volume 12, Number 4, pages 362-376, 1986.
		//
		//    P A Lewis, A S Goodman, J M Miller,
		//    A Pseudo-Random Number Generator for the System/360,
		//    IBM Systems Journal,
		//    Volume 8, pages 136-143, 1969.
		//
		//  Parameters:
		//
		//    Input, int M, N, the number of rows and columns.
		//
		//    Input/output, int *SEED, the "seed" value.  Normally, this
		//    value should not be 0, otherwise the output value of SEED
		//    will still be 0, and D_UNIFORM will be 0.  On output, SEED has 
		//    been updated.
		//
		//    Output, double DMAT_UNIFORM_01[M*N], a matrix of pseudorandom values.
		//
	{
		int i;
		int j;
		int k;

		for ( j = 0; j < n; j++ )
		{
			for ( i = 0; i < m; i++ )
			{
				k = *seed / 127773;

				*seed = 16807 * ( *seed - k * 127773 ) - k * 2836;

				if ( *seed < 0 )
				{
					*seed = *seed + 2147483647;
				}
				//
				//  Although SEED can be represented exactly as a 32 bit integer,
				//  it generally cannot be represented exactly as a 32 bit real number!
				//
				r[i+j*m] = ( double ) ( *seed ) * 4.656612875E-10;
			}
		}

		return;
	}
	//******************************************************************************

	double *dvec_normal ( int n, double b, double c, int *seed )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    DVEC_NORMAL samples the scaled normal probability distribution.
		//
		//  Discussion:
		//
		//    The scaled normal probability distribution function (PDF) has
		//    mean A and standard deviation B.
		//
		//    This routine can generate a vector of values on one call.  It
		//    has the feature that it should provide the same results
		//    in the same order no matter how we break up the task.
		//
		//    Before calling this routine, the user may call RANDOM_SEED
		//    in order to set the seed of the random number generator.
		//
		//  Method:
		//
		//    The Box-Muller method is used, which is efficient, but
		//    generates an even number of values each time.  On any call
		//    to this routine, an even number of new values are generated.
		//    Depending on the situation, one value may be left over.
		//    In that case, it is saved for the next call.
		//
		//  Modified:
		//
		//    02 February 2005
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    Input, int N, the number of values desired.  If N is negative,
		//    then the code will flush its internal memory; in particular,
		//    if there is a saved value to be used on the next call, it is
		//    instead discarded.  This is useful if the user has reset the
		//    random number seed, for instance.
		//
		//    Input, double B, C, the mean and standard deviation.
		//
		//    Input/output, int *SEED, a seed for the random number generator.
		//
		//    Output, double X(N), a sample of the standard normal PDF.
		//
		//  Local parameters:
		//
		//    Local, int MADE, records the number of values that have
		//    been computed.  On input with negative N, this value overwrites
		//    the return value of N, so the user can get an accounting of
		//    how much work has been done.
		//
		//    Local, real R(N+1), is used to store some uniform random values.
		//    Its dimension is N+1, but really it is only needed to be the
		//    smallest even number greater than or equal to N.
		//
		//    Local, int SAVED, is 0 or 1 depending on whether there is a
		//    single saved value left over from the previous call.
		//
		//    Local, int X_LO, X_HI, records the range of entries of
		//    X that we need to compute.  This starts off as 1:N, but is adjusted
		//    if we have a saved value that can be immediately stored in X(1),
		//    and so on.
		//
		//    Local, real Y, the value saved from the previous call, if
		//    SAVED is 1.
		//
	{
# define PI 3.141592653589793

		int i;
		int m;
		static int made = 0;
		double *r;
		static int saved = 0;
		double *x;
		int x_hi;
		int x_lo;
		static double y = 0.0;
		//
		//  I'd like to allow the user to reset the internal data.
		//  But this won't work properly if we have a saved value Y.
		//  I'm making a crock option that allows the user to signal
		//  explicitly that any internal memory should be flushed,
		//  by passing in a negative value for N.
		//
		if ( n < 0 )
		{
			made = 0;
			saved = 0;
			y = 0.0;
			return NULL;
		}
		else if ( n == 0 )
		{
			return NULL;
		}

		x = new double[n];
		//
		//  Record the range of X we need to fill in.
		//
		x_lo = 1;
		x_hi = n;
		//
		//  Use up the old value, if we have it.
		//
		if ( saved == 1 )
		{
			x[0] = y;
			saved = 0;
			x_lo = 2;
		}
		//
		//  Maybe we don't need any more values.
		//
		if ( x_hi - x_lo + 1 == 0 )
		{
		}
		//
		//  If we need just one new value, do that here to avoid null arrays.
		//
		else if ( x_hi - x_lo + 1 == 1 )
		{
			r = dvec_uniform_01 ( 2, seed );

			x[x_hi-1] = sqrt ( -2.0 * log ( r[0] ) ) * cos ( 2.0 * PI * r[1] );
			y =         sqrt ( -2.0 * log ( r[0] ) ) * sin ( 2.0 * PI * r[1] );

			saved = 1;

			made = made + 2;

			delete [] r;
		}
		//
		//  If we require an even number of values, that's easy.
		//
		else if ( ( x_hi - x_lo + 1 ) % 2 == 0 )
		{
			m = ( x_hi - x_lo + 1 ) / 2;

			r = dvec_uniform_01 ( 2*m, seed );

			for ( i = 0; i <= 2*m-2; i = i + 2 )
			{
				x[x_lo+i-1] = sqrt ( -2.0 * log ( r[i] ) ) * cos ( 2.0 * PI * r[i+1] );
				x[x_lo+i  ] = sqrt ( -2.0 * log ( r[i] ) ) * sin ( 2.0 * PI * r[i+1] );
			}
			made = made + x_hi - x_lo + 1;

			delete [] r;
		}
		//
		//  If we require an odd number of values, we generate an even number,
		//  and handle the last pair specially, storing one in X(N), and
		//  saving the other for later.
		//
		else
		{
			x_hi = x_hi - 1;

			m = ( x_hi - x_lo + 1 ) / 2 + 1;

			r = dvec_uniform_01 ( 2*m, seed );

			for ( i = 0; i <= 2*m-4; i = i + 2 )
			{
				x[x_lo+i-1] = sqrt ( -2.0 * log ( r[i] ) ) * cos ( 2.0 * PI * r[i+1] );
				x[x_lo+i  ] = sqrt ( -2.0 * log ( r[i] ) ) * sin ( 2.0 * PI * r[i+1] );
			}

			i = 2*m - 2;

			x[x_lo+i-1] = sqrt ( -2.0 * log ( r[i] ) ) * cos ( 2.0 * PI * r[i+1] );
			y           = sqrt ( -2.0 * log ( r[i] ) ) * sin ( 2.0 * PI * r[i+1] );

			saved = 1;

			made = made + x_hi - x_lo + 2;

			delete [] r;
		}

		for ( i = 0; i < n; i++ )
		{
			x[i] = b + c * x[i];
		}

		return x;
# undef PI
	}
	//******************************************************************************

	double *dvec_normal_01 ( int n, int *seed )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    DVEC_NORMAL_01 samples the unit normal probability distribution.
		//
		//  Discussion:
		//
		//    The standard normal probability distribution function (PDF) has
		//    mean 0 and standard deviation 1.
		//
		//    This routine can generate a vector of values on one call.  It
		//    has the feature that it should provide the same results
		//    in the same order no matter how we break up the task.
		//
		//    Before calling this routine, the user may call RANDOM_SEED
		//    in order to set the seed of the random number generator.
		//
		//  Method:
		//
		//    The Box-Muller method is used, which is efficient, but
		//    generates an even number of values each time.  On any call
		//    to this routine, an even number of new values are generated.
		//    Depending on the situation, one value may be left over.
		//    In that case, it is saved for the next call.
		//
		//  Modified:
		//
		//    18 October 2004
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    Input, int N, the number of values desired.  If N is negative,
		//    then the code will flush its internal memory; in particular,
		//    if there is a saved value to be used on the next call, it is
		//    instead discarded.  This is useful if the user has reset the
		//    random number seed, for instance.
		//
		//    Input/output, int *SEED, a seed for the random number generator.
		//
		//    Output, double X(N), a sample of the standard normal PDF.
		//
		//  Local parameters:
		//
		//    Local, int MADE, records the number of values that have
		//    been computed.  On input with negative N, this value overwrites
		//    the return value of N, so the user can get an accounting of
		//    how much work has been done.
		//
		//    Local, real R(N+1), is used to store some uniform random values.
		//    Its dimension is N+1, but really it is only needed to be the
		//    smallest even number greater than or equal to N.
		//
		//    Local, int SAVED, is 0 or 1 depending on whether there is a
		//    single saved value left over from the previous call.
		//
		//    Local, int X_LO, X_HI, records the range of entries of
		//    X that we need to compute.  This starts off as 1:N, but is adjusted
		//    if we have a saved value that can be immediately stored in X(1),
		//    and so on.
		//
		//    Local, real Y, the value saved from the previous call, if
		//    SAVED is 1.
		//
	{
# define PI 3.141592653589793

		int i;
		int m;
		static int made = 0;
		double *r;
		static int saved = 0;
		double *x;
		int x_hi;
		int x_lo;
		static double y = 0.0;

		x = new double[n];
		//
		//  I'd like to allow the user to reset the internal data.
		//  But this won't work properly if we have a saved value Y.
		//  I'm making a crock option that allows the user to signal
		//  explicitly that any internal memory should be flushed,
		//  by passing in a negative value for N.
		//
		if ( n < 0 )
		{
			made = 0;
			saved = 0;
			y = 0.0;
			return NULL;
		}
		else if ( n == 0 )
		{
			return NULL;
		}
		//
		//  Record the range of X we need to fill in.
		//
		x_lo = 1;
		x_hi = n;
		//
		//  Use up the old value, if we have it.
		//
		if ( saved == 1 )
		{
			x[0] = y;
			saved = 0;
			x_lo = 2;
		}
		//
		//  Maybe we don't need any more values.
		//
		if ( x_hi - x_lo + 1 == 0 )
		{
		}
		//
		//  If we need just one new value, do that here to avoid null arrays.
		//
		else if ( x_hi - x_lo + 1 == 1 )
		{
			r = dvec_uniform_01 ( 2, seed );

			x[x_hi-1] = sqrt ( -2.0 * log ( r[0] ) ) * cos ( 2.0 * PI * r[1] );
			y =         sqrt ( -2.0 * log ( r[0] ) ) * sin ( 2.0 * PI * r[1] );

			saved = 1;

			made = made + 2;

			delete [] r;
		}
		//
		//  If we require an even number of values, that's easy.
		//
		else if ( ( x_hi - x_lo + 1 ) % 2 == 0 )
		{
			m = ( x_hi - x_lo + 1 ) / 2;

			r = dvec_uniform_01 ( 2*m, seed );

			for ( i = 0; i <= 2*m-2; i = i + 2 )
			{
				x[x_lo+i-1] = sqrt ( -2.0 * log ( r[i] ) ) * cos ( 2.0 * PI * r[i+1] );
				x[x_lo+i  ] = sqrt ( -2.0 * log ( r[i] ) ) * sin ( 2.0 * PI * r[i+1] );
			}
			made = made + x_hi - x_lo + 1;

			delete [] r;
		}
		//
		//  If we require an odd number of values, we generate an even number,
		//  and handle the last pair specially, storing one in X(N), and
		//  saving the other for later.
		//
		else
		{
			x_hi = x_hi - 1;

			m = ( x_hi - x_lo + 1 ) / 2 + 1;

			r = dvec_uniform_01 ( 2*m, seed );

			for ( i = 0; i <= 2*m-4; i = i + 2 )
			{
				x[x_lo+i-1] = sqrt ( -2.0 * log ( r[i] ) ) * cos ( 2.0 * PI * r[i+1] );
				x[x_lo+i  ] = sqrt ( -2.0 * log ( r[i] ) ) * sin ( 2.0 * PI * r[i+1] );
			}

			i = 2*m - 2;

			x[x_lo+i-1] = sqrt ( -2.0 * log ( r[i] ) ) * cos ( 2.0 * PI * r[i+1] );
			y           = sqrt ( -2.0 * log ( r[i] ) ) * sin ( 2.0 * PI * r[i+1] );

			saved = 1;

			made = made + x_hi - x_lo + 2;

			delete [] r;
		}

		return x;
# undef PI
	}
	//******************************************************************************

	double *dvec_uniform ( int n, double b, double c, int *seed )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    DVEC_UNIFORM fills a double precision vector with scaled pseudorandom values.
		//
		//  Discussion:
		//
		//    This routine implements the recursion
		//
		//      seed = 16807 * seed mod ( 2**31 - 1 )
		//      unif = seed / ( 2**31 - 1 )
		//
		//    The integer arithmetic never requires more than 32 bits,
		//    including a sign bit.
		//
		//  Modified:
		//
		//    30 January 2005
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Reference:
		//
		//    Paul Bratley, Bennett Fox, L E Schrage,
		//    A Guide to Simulation,
		//    Springer Verlag, pages 201-202, 1983.
		//
		//    Bennett Fox,
		//    Algorithm 647:
		//    Implementation and Relative Efficiency of Quasirandom
		//    Sequence Generators,
		//    ACM Transactions on Mathematical Software,
		//    Volume 12, Number 4, pages 362-376, 1986.
		//
		//    P A Lewis, A S Goodman, J M Miller,
		//    A Pseudo-Random Number Generator for the System/360,
		//    IBM Systems Journal,
		//    Volume 8, pages 136-143, 1969.
		//
		//  Parameters:
		//
		//    Input, int N, the number of entries in the vector.
		//
		//    Input, double B, C, the lower and upper limits of the pseudorandom values.
		//
		//    Input/output, int *SEED, a seed for the random number generator.
		//
		//    Output, double DVEC_UNIFORM_01[N], the vector of pseudorandom values.
		//
	{
		int i;
		int k;
		double *r;

		r = new double[n];

		for ( i = 0; i < n; i++ )
		{
			k = *seed / 127773;

			*seed = 16807 * ( *seed - k * 127773 ) - k * 2836;

			if ( *seed < 0 )
			{
				*seed = *seed + 2147483647;
			}

			r[i] = b + ( c - b ) * ( double ) ( *seed ) * 4.656612875E-10;
		}

		return r;
	}
	//******************************************************************************

	double *dvec_uniform_01 ( int n, int *seed )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    DVEC_UNIFORM_01 fills a double precision vector with unit pseudorandom values.
		//
		//  Discussion:
		//
		//    This routine implements the recursion
		//
		//      seed = 16807 * seed mod ( 2**31 - 1 )
		//      unif = seed / ( 2**31 - 1 )
		//
		//    The integer arithmetic never requires more than 32 bits,
		//    including a sign bit.
		//
		//  Modified:
		//
		//    19 August 2004
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Reference:
		//
		//    Paul Bratley, Bennett Fox, L E Schrage,
		//    A Guide to Simulation,
		//    Springer Verlag, pages 201-202, 1983.
		//
		//    Bennett Fox,
		//    Algorithm 647:
		//    Implementation and Relative Efficiency of Quasirandom
		//    Sequence Generators,
		//    ACM Transactions on Mathematical Software,
		//    Volume 12, Number 4, pages 362-376, 1986.
		//
		//    P A Lewis, A S Goodman, J M Miller,
		//    A Pseudo-Random Number Generator for the System/360,
		//    IBM Systems Journal,
		//    Volume 8, pages 136-143, 1969.
		//
		//  Parameters:
		//
		//    Input, int N, the number of entries in the vector.
		//
		//    Input/output, int *SEED, a seed for the random number generator.
		//
		//    Output, double DVEC_UNIFORM_01[N], the vector of pseudorandom values.
		//
	{
		int i;
		int k;
		double *r;

		r = new double[n];

		for ( i = 0; i < n; i++ )
		{
			k = *seed / 127773;

			*seed = 16807 * ( *seed - k * 127773 ) - k * 2836;

			if ( *seed < 0 )
			{
				*seed = *seed + 2147483647;
			}

			r[i] = ( double ) ( *seed ) * 4.656612875E-10;
		}

		return r;
	}
	//******************************************************************************

	int get_seed ( void )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    GET_SEED returns a random seed for the random number generator.
		//
		//  Modified:
		//
		//    17 November 2004
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    Output, int GET_SEED, a random seed value.
		//
	{
# define I_MAX 2147483647
		time_t clock;
		int ihour;
		int imin;
		int isec;
		int seed;
		struct tm *lt;
		time_t tloc;
		//
		//  If the internal seed is 0, generate a value based on the time.
		//
		clock = time ( &tloc );
		lt = localtime ( &clock );
		//
		//  Hours is 1, 2, ..., 12.
		//
		ihour = lt->tm_hour;

		if ( 12 < ihour )
		{
			ihour = ihour - 12;
		}
		//
		//  Move Hours to 0, 1, ..., 11
		//
		ihour = ihour - 1;

		imin = lt->tm_min;

		isec = lt->tm_sec;

		seed = isec + 60 * ( imin + 60 * ihour );
		//
		//  We want values in [1,43200], not [0,43199].
		//
		seed = seed + 1;
		//
		//  Remap ISEED from [1,43200] to [1,IMAX].
		//
		seed = ( int ) 
			( ( ( double ) seed )
			* ( ( double ) I_MAX ) / ( 60.0E+00 * 60.0E+00 * 12.0E+00 ) );
		//
		//  Never use a seed of 0.
		//
		if ( seed == 0 )
		{
			seed = 1;
		}

		return seed;
# undef I_MAX
	}
	//******************************************************************************

	int i_gcd ( int i, int j )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    I_GCD finds the greatest common divisor of I and J.
		//
		//  Discussion:
		//
		//    Only the absolute values of I and J are considered, so that the 
		//    result is always nonnegative.
		//
		//    If I or J is 0, I_GCD is returned as max ( 1, abs ( I ), abs ( J ) ).
		//
		//    If I and J have no common factor, I_GCD is returned as 1.
		//
		//    Otherwise, using the Euclidean algorithm, I_GCD is the
		//    largest common factor of I and J.
		//
		//  Modified:
		//
		//    07 May 2003
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    Input, int I, J, two numbers whose greatest common divisor
		//    is desired.
		//
		//    Output, int I_GCD, the greatest common divisor of I and J.
		//
	{
		int ip;
		int iq;
		int ir;
		//
		//  Return immediately if either I or J is zero.
		//
		if ( i == 0 )
		{
			return i_max ( 1, abs ( j ) );
		}
		else if ( j == 0 )
		{
			return i_max ( 1, abs ( i ) );
		}
		//
		//  Set IP to the larger of I and J, IQ to the smaller.
		//  This way, we can alter IP and IQ as we go.
		//
		ip = i_max ( abs ( i ), abs ( j ) );
		iq = i_min ( abs ( i ), abs ( j ) );
		//
		//  Carry out the Euclidean algorithm.
		//
		for ( ; ; )
		{
			ir = ip % iq;

			if ( ir == 0 )
			{
				break;
			}

			ip = iq;
			iq = ir;
		}

		return iq;
	}
	//******************************************************************************

	int i_log_10 ( int i )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    I_LOG_10 returns the whole part of the logarithm base 10 of an integer.
		//
		//  Discussion:
		//
		//    It should be the case that 10^I_LOG_10(I) <= |I| < 10^(I_LOG_10(I)+1).
		//    (except for I = 0).
		//
		//    The number of decimal digits in I is I_LOG_10(I) + 1.
		//
		//  Examples:
		//
		//        I    I_LOG_10(I)
		//
		//        0     0
		//        1     0
		//        2     0
		//
		//        9     0
		//       10     1
		//       11     1
		//
		//       99     1
		//      100     2
		//      101     2
		//
		//      999     2
		//     1000     3
		//     1001     3
		//
		//     9999     3
		//    10000     4
		//    10001     4
		//
		//  Modified:
		//
		//    17 June 2003
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    Input, int I, the integer.
		//
		//    Output, int I_LOG_10, the whole part of the logarithm of abs ( I ).
		//
	{
		int ten_pow;
		int value;

		i = abs ( i );

		ten_pow = 10;
		value = 0;

		while ( ten_pow <= i )
		{
			ten_pow = ten_pow * 10;
			value = value + 1;
		}

		return value;
	}
	//******************************************************************************

	int i_max ( int i1, int i2 )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    I_MAX returns the maximum of two integers.
		//
		//  Modified:
		//
		//    05 May 2003
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    Input, int I1, I2, two integers to be compared.
		//
		//    Output, int I_MAX, the larger of I1 and I2.
		//
	{
		if ( i2 < i1 ) 
		{
			return i1;
		}
		else 
		{
			return i2;
		}

	}
	//******************************************************************************

	int i_min ( int i1, int i2 )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    I_MIN returns the smaller of two integers.
		//
		//  Modified:
		//
		//    05 May 2003
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    Input, int I1, I2, two integers to be compared.
		//
		//    Output, int I_MIN, the smaller of I1 and I2.
		//
	{
		if ( i1 < i2 ) 
		{
			return i1;
		}
		else 
		{
			return i2;
		}

	}
	//********************************************************************

	int i_sign ( int i )

		//********************************************************************
		//
		//  Purpose:
		//
		//    I_SIGN returns the sign of an integer.
		//
		//  Discussion:
		//
		//    The sign of 0 and all positive integers is taken to be +1.
		//    The sign of all negative integers is -1.
		//
		//  Modified:
		//
		//    06 May 2003
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    Input, int I, the integer whose sign is desired.
		//
		//    Output, int I_SIGN, the sign of I.
	{
		if ( i < 0 ) 
		{
			return (-1);
		}
		else
		{
			return 1;
		}

	}
	//*********************************************************************

	void i_swap ( int *i, int *j )

		//*********************************************************************
		//
		//  Purpose:
		//
		//    I_SWAP switches two integer values.
		//
		//  Modified:
		//
		//    07 January 2002
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    Input/output, int *I, *J.  On output, the values of I and
		//    J have been interchanged.
		//
	{
		int k;

		k = *i;
		*i = *j;
		*j = k;

		return;
	}
	//******************************************************************************

	char *i_to_s ( int i )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    I_TO_S converts an integer to a string.
		//
		//  Examples:
		//
		//    INTVAL  S
		//
		//         1  1
		//        -1  -1
		//         0  0
		//      1952  1952
		//    123456  123456
		//   1234567  1234567
		//
		//  Modified:
		//
		//    13 March 2004
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    Input, int I, an integer to be converted.
		//
		//    Output, char *I_TO_S, the representation of the integer.
		//
	{
		int digit;
		int j;
		int length;
		int ten_power;
		char *s;

		length = i_log_10 ( i );

		ten_power = ( int ) pow ( ( double ) 10, ( double ) length );

		if ( i < 0 )
		{
			length = length + 1;
		}
		//
		//  Add one position for the trailing null.
		//
		length = length + 1;

		s = new char[length];

		if ( i == 0 )
		{
			s[0] = '0';
			s[1] = '\0';
			return s;
		}
		//
		//  Now take care of the sign.
		//
		j = 0;
		if ( i < 0 )
		{
			s[j] = '-';
			j = j + 1;
			i = abs ( i );
		}
		//
		//  Find the leading digit of I, strip it off, and stick it into the string.
		//
		while ( 0 < ten_power )
		{
			digit = i / ten_power;
			s[j] = digit_to_ch ( digit );
			j = j + 1;
			i = i - digit * ten_power;
			ten_power = ten_power / 10;
		}
		//
		//  Tack on the trailing NULL.
		//
		s[j] = '\0';
		j = j + 1;

		return s;
	}
	//******************************************************************************

	int i_uniform ( int b, int c, int *seed )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    I_UNIFORM returns a pseudorandom integer.
		//
		//  Discussion:
		//
		//    The pseudorandom number should be uniformly distributed
		//    between A and B.
		//
		//  Modified:
		//
		//    26 February 2005
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    Input, int B, C, the limits of the interval.
		//
		//    Input/output, int *SEED, the "seed" value, which should NOT be 0.
		//    On output, SEED has been updated.
		//
		//    Output, int I_UNIFORM, a number between A and B.
		//
	{
		double d;
		int value;

		if ( b <= c )
		{
			d = ( double ) ( b ) + ( double ) ( 1 + c - b ) * d_uniform_01 ( seed );

			value = ( int ) ( d );

			if ( value < b )
			{
				value = b;
			}
			if ( c < value )
			{
				value = c;
			}
		}
		else
		{
			d = ( double ) ( c ) + ( double ) ( 1 + b - c ) * d_uniform_01 ( seed );

			value = ( int ) ( d );

			if ( value < c )
			{
				value = c;
			}
			if ( b < value )
			{
				value = b;
			}
		}

		return value;
	}
	//******************************************************************************

	void ivec_uniform ( int n, int b, int c, int *seed, int x[] )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    IVEC_UNIFORM returns a vector of integer pseudorandom numbers.
		//
		//  Discussion:
		//
		//    The pseudorandom numbers should be uniformly distributed
		//    between A and B.
		//
		//  Modified:
		//
		//    30 January 2005
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    Input, integer N, the dimension of the vector.
		//
		//    Input, int B, C, the limits of the interval.
		//
		//    Input/output, int *SEED, the "seed" value, which should NOT be 0.
		//    On output, SEED has been updated.
		//
		//    Output, int X[N], a vector of random values between A and B.
		//
	{
		double d;
		int i;
		int value;

		for ( i = 0; i < n; i++ )
		{
			if ( b <= c )
			{
				d = ( double ) ( b ) + ( double ) ( 1 + c - b ) * d_uniform_01 ( seed );

				value = ( int ) ( d );

				if ( value < b )
				{
					value = b;
				}
				if ( c < value )
				{
					value = c;
				}
			}
			else
			{
				d = ( double ) ( c ) + ( double ) ( 1 + b - c ) * d_uniform_01 ( seed );

				value = ( int ) ( d );

				if ( value < c )
				{
					value = c;
				}
				if ( b < value )
				{
					value = b;
				}
			}

			x[i] = value;
		}

		return;
	}
	//******************************************************************************

	int lcrg_seed ( int a, int b, int c, int n, int seed )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    LCRG_SEED computes the N-th seed of a linear congruential generator.
		//
		//  Discussion:
		//
		//    We are considering a linear congruential random number generator.
		//    The LCRG takes as input an integer value called SEED, and returns
		//    an updated value of SEED,
		//
		//      SEED(out) = a * SEED(in) + b, mod c.
		//
		//    and an associated pseudorandom real value
		//
		//      U = SEED(out) / c.
		//
		//    In most cases, a user is content to call the LCRG repeatedly, with
		//    the updating of SEED being taken care of automatically.
		//
		//    The purpose of this routine is to determine the value of SEED that
		//    would be output after N successive applications of the LCRG.  This
		//    allows the user to know, in advance, what the 1000-th value of
		//    SEED would be, for instance.  Obviously, one way to do this is to
		//    apply the LCRG formula 1,000 times.  However, it is possible to
		//    do this in a more direct and efficient way.
		//
		//    One use for such a facility would be to do random number computations
		//    in parallel.  If each processor is to compute 1,000 values, you can
		//    guarantee that they work with distinct random values by starting the
		//    first processor with SEED, the second with the value of SEED after
		//    1,000 applications of the LCRG, and so on.
		//
		//    To evaluate the N-th value of SEED directly, we start by ignoring
		//    the modular arithmetic, and working out the sequence of calculations
		//    as follows:
		//
		//      SEED(0) =     SEED.
		//      SEED(1) = a * SEED      + b
		//      SEED(2) = a * SEED(1)   + b = a^2 * SEED + a * b + b
		//      SEED(3) = a * SEED(2)   + b = a^3 * SEED + a^2 * b + a * b + b
		//      ...
		//      SEED(N) = a * SEED(N-1) + b = a^N * SEED
		//                                    + ( a^(n-1) + a^(n-2) + ... + a + 1 ) * b
		//
		//    or, using the geometric series,
		//
		//      SEED(N) = a^N * SEED + ( a^N - 1) / ( a - 1 ) * b
		//
		//    Therefore, we can determine SEED(N) directly if we can solve
		//
		//      ( a - 1 ) * BN = ( a^N - 1 ) * b in modular arithmetic,
		//
		//    and evaluated:
		//
		//      AN = a^N
		//
		//    Using the formula:
		//
		//      SEED(N) = AN * SEED + BN, mod c
		//
		//  Modified:
		//
		//    15 November 2004
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    Input, int A, the multiplier for the LCRG.
		//
		//    Input, int B, the added value for the LCRG.
		//
		//    Input, int C, the base for the modular arithmetic.  For 32 bit
		//    arithmetic, this is often 2^31 - 1, or 2147483647.  It is required
		//    that 0 < C.
		//
		//    Input, int N, the "index", or number of times that the LCRG
		//    is to be applied.  It is required that 0 <= N.
		//
		//    Input, int SEED, the starting value of SEED.  It is customary
		//    that 0 < SEED.
		//
		//    Output, int LCRG_SEED, the value of SEED that would be output
		//    if the LCRG were applied to the starting value N times.
		//
	{
		int an;
		int bn;
		bool ierror;
		int value;
		long int value2;

		if ( n < 0 )
		{
			cout << "\n";
			cout << "LCRG_SEED - Fatal error!\n";
			cout << "  Illegal input value of N = " << n << "\n";
			exit ( 1 );
		}

		if ( c <= 0 )
		{
			cout << "\n";
			cout << "LCRG_SEED - Fatal error!\n";
			cout << "  Illegal input value of C = " << c << "\n";
			exit ( 1 );
		}

		if ( n == 0 )
		{
			value = seed % c;
			if ( value < 0 )
			{
				value = value + c;
			}
			return value;
		}
		//
		//  Get A^N.
		//
		an = power_mod ( a, n, c );
		//
		//  Solve ( a - 1 ) * BN = ( a^N - 1 ) for BN.
		//
		//  The LCRG I have been investigating uses B = 0, so this code
		//  has not been properly tested yet.
		//
		bn = congruence ( a-1, c, ( an - 1 ) * b, &ierror );

		if ( ierror != 0 )
		{
			cout << "\n";
			cout << "LCRG_SEED - Fatal error!\n";
			cout << "  An error occurred in the CONGRUENCE routine.\n";
			cout << "  The error code was IERROR = " << ierror << "\n";
			exit ( 1 );
		}
		//
		//  Set the new SEED.
		//
		value2 = ( long int ) ( an ) * ( long int ) ( seed ) + ( long int ) ( bn );

		value2 = value2 % ( long int ) ( c );
		//
		//  Guarantee that the value is positive.
		//
		if ( value2 < 0 )
		{
			value2 = value2 + ( long int ) ( c );
		}

		value = ( int ) ( value2 );

		return value;
	}
	//******************************************************************************

	int power_mod ( int a, int n, int m )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    POWER_MOD computes mod ( A**N, M ).
		//
		//  Discussion:
		//
		//    Some programming tricks are used to speed up the computation, and to
		//    allow computations in which A**N is much too large to store in a
		//    real word.
		//
		//    First, for efficiency, the power A**N is computed by determining
		//    the binary expansion of N, then computing A, A**2, A**4, and so on
		//    by repeated squaring, and multiplying only those factors that
		//    contribute to A**N.
		//
		//    Secondly, the intermediate products are immediately "mod'ed", which
		//    keeps them small.
		//
		//    For instance, to compute mod ( A**13, 11 ), we essentially compute
		//
		//       13 = 1 + 4 + 8
		//
		//       A**13 = A * A**4 * A**8
		//
		//       mod ( A**13, 11 ) = mod ( A, 11 ) * mod ( A**4, 11 ) * mod ( A**8, 11 ).
		//
		//    Fermat's little theorem says that if P is prime, and A is not divisible
		//    by P, then ( A**(P-1) - 1 ) is divisible by P.
		//
		//  Modified:
		//
		//    16 November 2004
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    Input, int A, the base of the expression to be tested.
		//    A should be nonnegative.
		//
		//    Input, int N, the power to which the base is raised.
		//    N should be nonnegative.
		//
		//    Input, int M, the divisor against which the expression is tested.
		//    M should be positive.
		//
		//    Output, int POWER_MOD, the remainder when A**N is divided by M.
		//
	{
		long int a_square2;
		int d;
		long int m2;
		int x;
		long int x2;

		if ( a < 0 )
		{
			return -1;
		}

		if ( m <= 0 )
		{
			return -1;
		}

		if ( n < 0 )
		{
			return -1;
		}
		//
		//  A_SQUARE contains the successive squares of A.
		//
		a_square2 = ( long int ) a;
		m2 = ( long int ) m;
		x2 = ( long int ) 1;

		while ( 0 < n )
		{
			d = n % 2;

			if ( d == 1 )
			{
				x2 = ( x2 * a_square2 ) % m2;
			}

			a_square2 = ( a_square2 * a_square2 ) % m2;
			n = ( n - d ) / 2;
		}
		//
		//  Ensure that 0 <= X.
		//
		while ( x2 < 0 )
		{
			x2 = x2 + m2;
		}

		x = ( int ) x2;

		return x;
	}
	//******************************************************************************

	float r_uniform ( float b, float c, int *seed )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    R_UNIFORM returns a real pseudorandom number.
		//
		//  Discussion:
		//
		//    The pseudorandom number should be uniformly distributed
		//    between A and B.
		//
		//  Modified:
		//
		//    21 November 2004
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    Input, float B, C, the limits of the interval.
		//
		//    Input/output, int *SEED, the "seed" value, which should NOT be 0.
		//    On output, SEED has been updated.
		//
		//    Output, float R_UNIFORM, a number strictly between A and B.
		//
	{
		double value;

		value = b + ( c - b ) * r_uniform_01 ( seed );

		return (float)value;
	}
	//******************************************************************************

	float r_uniform_01 ( int *seed )

		//******************************************************************************
		//
		//  Purpose:
		//
		//    R_UNIFORM_01 returns a real pseudorandom number.
		//
		//  Discussion:
		//
		//    This routine implements the recursion
		//
		//      seed = 16807 * seed mod ( 2**31 - 1 )
		//      r_uniform_01 = seed / ( 2**31 - 1 )
		//
		//    The integer arithmetic never requires more than 32 bits,
		//    including a sign bit.
		//
		//    If the initial seed is 12345, then the first three computations are
		//
		//      Input     Output      R_UNIFORM_01
		//      SEED      SEED
		//
		//         12345   207482415  0.096616
		//     207482415  1790989824  0.833995
		//    1790989824  2035175616  0.947702
		//
		//  Modified:
		//
		//    16 November 2004
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Reference:
		//
		//    Paul Bratley, Bennett Fox, L E Schrage,
		//    A Guide to Simulation,
		//    Springer Verlag, pages 201-202, 1983.
		//
		//    Pierre L'Ecuyer,
		//    Random Number Generation,
		//    in Handbook of Simulation
		//    edited by Jerry Banks,
		//    Wiley Interscience, page 95, 1998.
		//
		//    Bennett Fox,
		//    Algorithm 647:
		//    Implementation and Relative Efficiency of Quasirandom
		//    Sequence Generators,
		//    ACM Transactions on Mathematical Software,
		//    Volume 12, Number 4, pages 362-376, 1986.
		//
		//    P A Lewis, A S Goodman, J M Miller,
		//    A Pseudo-Random Number Generator for the System/360,
		//    IBM Systems Journal,
		//    Volume 8, pages 136-143, 1969.
		//
		//  Parameters:
		//
		//    Input/output, int *SEED, the "seed" value.  Normally, this
		//    value should not be 0.  On output, SEED has been updated.
		//
		//    Output, float R_UNIFORM_01, a new pseudorandom variate, strictly between
		//    0 and 1.
		//
	{
		int k;
		float r;

		k = *seed / 127773;

		*seed = 16807 * ( *seed - k * 127773 ) - k * 2836;

		if ( *seed < 0 )
		{
			*seed = *seed + 2147483647;
		}
		//
		//  Although SEED can be represented exactly as a 32 bit integer,
		//  it generally cannot be represented exactly as a 32 bit real number!
		//
		r = ( float ) ( ( *seed ) * 4.656612875E-10 );

		return r;
	}
	//**********************************************************************

	void timestamp ( void )

		//**********************************************************************
		//
		//  Purpose:
		//
		//    TIMESTAMP prints the current YMDHMS date as a time stamp.
		//
		//  Example:
		//
		//    May 31 2001 09:45:54 AM
		//
		//  Modified:
		//
		//    02 October 2003
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    None
		//
	{
# define TIME_SIZE 40

		static char time_buffer[TIME_SIZE];
		const struct tm *tm;
		size_t len;
		time_t now;

		now = time ( NULL );
		tm = localtime ( &now );

		len = strftime ( time_buffer, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm );

		cout << time_buffer << "\n";

		return;
# undef TIME_SIZE
	}
	//**********************************************************************

	char *timestring ( void )

		//**********************************************************************
		//
		//  Purpose:
		//
		//    TIMESTRING returns the current YMDHMS date as a string.
		//
		//  Example:
		//
		//    May 31 2001 09:45:54 AM
		//
		//  Modified:
		//
		//    24 September 2003
		//
		//  Author:
		//
		//    John Burkardt
		//
		//  Parameters:
		//
		//    Output, char *TIMESTRING, a string containing the current YMDHMS date.
		//
	{
# define TIME_SIZE 40

		const struct tm *tm;
		size_t len;
		time_t now;
		char *s;

		now = time ( NULL );
		tm = localtime ( &now );

		s = new char[TIME_SIZE];

		len = strftime ( s, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm );

		return s;
# undef TIME_SIZE
	}

}


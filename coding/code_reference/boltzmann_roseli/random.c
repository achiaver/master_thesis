
/*
****************************************************************************

			Random Number Generator
		      Version 1.0,  Jan. 13, 1992
		      From Numerical Recipies in C 

		Generates random number in interval [0, 1].

****************************************************************************
*/

#define	MBIG	1000000000
#define	MSEED	161803398
#define	MZ	0
#define	FAC	(1.0/MBIG)

int	inext, inextp;
long	ma[56];
int	iff = 0;

double
ran3(idum)
int	*idum;
{
/*	static int	inext, inextp;
	static long	ma[56];
	static int	iff = 0;  */
	long		mj, mk;
	int		i, ii, k;

	if (*idum < 0 || iff == 0)
	{
		iff = 1;
		mj = MSEED - (*idum < 0 ? -*idum : *idum);
		mj %= MBIG;
		ma[55] = mj;
		mk = 1;
		for (i = 1; i <= 54; i++)
		{
			ii = (21 * i) % 55;
			ma[ii] = mk;
			mk = mj - mk;
			if (mk < MZ) mk += MBIG;
			mj = ma[ii];	
		}   /* end for  */
		
		for (k = 1; k <= 4; k++)
			for (i = 1; i <= 55; i++) 
			{
				ma[i] -= ma[1+(i + 30) % 55];
				if (ma[i] < MZ) ma[i] += MBIG;
			} /*  end for  */
		inext = 0;
		inextp = 31;
		*idum = 1;
	} /*  end if  */

	if (++inext == 56) inext = 1;	
	if (++inextp == 56) inextp = 1;
	mj = ma[inext] - ma[inextp];
	if (mj < MZ) mj += MBIG;
	ma[inext] = mj;
        if (mj * FAC == 1.0) return (0.0);
	else	return (mj * FAC);
} /*  end ran3  */

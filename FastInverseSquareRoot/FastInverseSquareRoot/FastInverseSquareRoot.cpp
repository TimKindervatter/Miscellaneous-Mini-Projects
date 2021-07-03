#include <iostream>
#include <cmath>

double magic_number()
{
    // Based on derivation here: https://en.wikipedia.org/wiki/Fast_inverse_square_root

    double L = pow(2, 23);
    char B = 127;
    double sigma = 0.0450465;

    double magic = 1.5*L*(B - sigma); // 0x5f3759df
    
    return magic;
}

float finv_sqrt(float number)
{
    long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                               // evil floating point bit level hacking
    i  = (unsigned)magic_number() - ( i >> 1 );         // what the fuck? 
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );           // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );           // 2nd iteration, this can be removed

    return y;
}

int main()
{
    std::cout << finv_sqrt(2) << '\n';
    std::cout << std::hex << (unsigned)magic_number();
}
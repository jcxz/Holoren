/**
 * This is a hello World kernel
 */

__constant char k;

__kernel void hello(__global char16 *out, __global char *i, char *a_pointer_to_private_argument)
{
  /* toto je v podstate to iste ako compound literal v c99
     s tym rozdielom, ze namiesto sisatych zatvoriek su tu gulate zatvorky */
  char16 msg = (char16) ('0', '1', '2', '3', '4', '5', '6', '7',
                         '8', '9', 'A', 'B', 'C', 'D', 'E', 'F');

  /* prekopiruj neparne komponenty do parnych */
  msg.even = msg.odd;

  /* nastav prve dva kmponenty vektoru na hodnotu poslednej zlozky */
  msg.s01 = msg.SFF;

  char3 msg2 = (char3) ('1', '2', '3');

  /* zapis vysledok */
  (*out).s01 = msg2.lo;
  (*out).s23 = msg2.hi;  // -> does not work here
  (*out).s45 = msg2.lo;  // -> does not work here

  //__global int x = 0;  // toto je error
  //__constant char k = *i;
  //__constant static char k = *i;
  //k = *i;

  (*out).s0 = k;

  *i = *a_pointer_to_private_argument;
}

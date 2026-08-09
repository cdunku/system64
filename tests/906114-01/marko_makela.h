#pragma once

/**
 * The following code has been modified and taken from the following link:
 * https://www.zimmers.net/anonftp/pub/cbm/firmware/computers/c64/pla.c
 * 
 * I do not own, nor claim to own that the code below belongs to me.
 * The following code was used to test my PLA lookup table.
**/



/** Program to convert logic equations
 * of 16 inputs and 8 outputs
 * to a 64-kilobyte truth table.
 * @author Marko Mäkelä (msmakela@nic.funet.fi)
 * @date 2nd July 2002, updated 8th July 2003
 *
 * Compilation:
 *	cc -o pla pla.c
 * Example usage:
 *	./pla | diff - pla-dump.bin
 * or
 *	./pla | od -Ax -t x1
 * or
 *	./pla | cksum
 * or
 *	./pla > pla-dump.bin
 *
 * The following Perl 5 code extracts output F7 from a truth table dump:
 * perl -e 'undef $/;$_=<>;s/(.)/chr(ord($1)&128)/ge;print' < pla-dump.bin
 *
 * To generate only selected outputs with this program,
 * you may as well comment out some lines in the main function.
 */

#include <stdio.h>
#include <stdlib.h>

/** Extract an input bit
 * @param b	the bit to be extracted
 * @return	nonzero if the input bit b is set
 */
#define I(b) (!!((i) & (1 << b)))

/** @name The input signals of the Commodore 64 PLA (906114-1).
 * This mapping corresponds to the 82S100 to 27512 adapter made by
 * Jens Schönfeld (jens@ami.ga).  Note also the permutation of outputs
 * in the main loop.
 * (In the first version of this program, the signals were numbered
 * from 0 to 15, or 0 to 7, respectively.)
 */
/*@{*/

#define A12	    I(0)
#define A13	    I(1)
#define A14	    I(2)
#define A15	    I(3)
#define VA12	I(4)
#define VA13	I(5)
#define VA14_	!I(6)
#define CAS_	!I(7)
#define LORAM_	!I(8)
#define HIRAM_	!I(9)
#define CHAREN_	!I(10)
#define GAME_	!I(11)
#define EXROM_	!I(12)
#define BA	    I(13)
#define AEC_	!I(14)
#define R_W_	I(15)
/*@}*/

/** @name The output signals of the Commodore 64 PLA.
 * Converted from
 * http://www.funet.fi/pub/cbm/firmware/computers/c64/pla.txt
 * by Marko Mäkelä on 2nd July 2002
 * and verified against a dump provided by William Levak.
 * The equations are in conjunctive normal form (CNF).
 * Operators:	!x	negation of x: not x
 * 		x || y	disjunction: x or b
 * 		x && y	conjunction: x and b
 */
/*@{*/
/* CASRAM_ */
#define F0	((LORAM_ && HIRAM_ && A15 && !A14 && A13 && 		  \
		  !AEC_ && R_W_ && GAME_) ||				  \
		 (HIRAM_ && A15 && A14 && A13 &&			  \
		  !AEC_ && R_W_ && GAME_) ||				  \
		 (HIRAM_ && A15 && A14 && A13 &&			  \
		  !AEC_ && R_W_ && !EXROM_ && !GAME_) ||		  \
		 (HIRAM_ && !CHAREN_ && A15 && A14 && !A13 && 		  \
		  A12 && !AEC_ && R_W_ && GAME_) ||			  \
		 (LORAM_ && !CHAREN_ && A15 && A14 && !A13 && 		  \
		  A12 && !AEC_ && R_W_ && GAME_) ||			  \
		 (HIRAM_ && !CHAREN_ && A15 && A14 && !A13 && 		  \
		  A12 && !AEC_ && R_W_ && !EXROM_ && !GAME_) ||		  \
		 (VA14_ && AEC_ && GAME_ && !VA13 && VA12) ||		  \
		 (VA14_ && AEC_ && !EXROM_ && !GAME_ && !VA13 && VA12) || \
		 (HIRAM_ && CHAREN_ && A15 && A14 && !A13 && 		  \
		  A12 && BA && !AEC_ && R_W_ && GAME_) ||		  \
		 (HIRAM_ && CHAREN_ && A15 && A14 && !A13 && 		  \
		  A12 && !AEC_ && !R_W_ && GAME_) ||			  \
		 (LORAM_ && CHAREN_ && A15 && A14 && !A13 && 		  \
		  A12 && BA && !AEC_ && R_W_ && GAME_) ||		  \
		 (LORAM_ && CHAREN_ && A15 && A14 && !A13 && 		  \
		  A12 && !AEC_ && !R_W_ && GAME_) ||			  \
		 (HIRAM_ && CHAREN_ && A15 && A14 && !A13 && 		  \
		  A12 && BA && !AEC_ && R_W_ && !EXROM_ && !GAME_) ||	  \
		 (HIRAM_ && CHAREN_ && A15 && A14 && !A13 && 		  \
		  A12 && !AEC_ && !R_W_ && !EXROM_ && !GAME_) ||	  \
		 (LORAM_ && CHAREN_ && A15 && A14 && !A13 && 		  \
		  A12 && BA && !AEC_ && R_W_ && !EXROM_ && !GAME_) ||	  \
		 (LORAM_ && CHAREN_ && A15 && A14 && !A13 && 		  \
		  A12 && !AEC_ && !R_W_ && !EXROM_ && !GAME_) ||	  \
		 (A15 && A14 && !A13 && A12 && BA && 			  \
		  !AEC_ && R_W_ && EXROM_ && !GAME_) ||			  \
		 (A15 && A14 && !A13 && A12 &&				  \
		  !AEC_ && !R_W_ && EXROM_ && !GAME_) ||		  \
		 (LORAM_ && HIRAM_ && A15 && !A14 && !A13 && 		  \
		  !AEC_ && R_W_ && !EXROM_) ||				  \
		 (A15 && !A14 && !A13 && !AEC_ && EXROM_ && !GAME_) ||	  \
		 (HIRAM_ && A15 && !A14 && A13 && !AEC_ && 		  \
		  R_W_ && !EXROM_ && !GAME_) ||				  \
		 (A15 && A14 && A13 && !AEC_ && EXROM_ && !GAME_) ||	  \
		 (AEC_ && EXROM_ && !GAME_ && VA13 && VA12) ||		  \
		 (!A15 && !A14 && A12 && EXROM_ && !GAME_) ||		  \
		 (!A15 && !A14 && A13 && EXROM_ && !GAME_) ||		  \
		 (!A15 && A14 && EXROM_ && !GAME_) ||			  \
		 (A15 && !A14 && A13 && EXROM_ && !GAME_) ||		  \
		 (A15 && A14 && !A13 && !A12 && EXROM_ && !GAME_) ||	  \
		 CAS_)
/* BASIC_ */
#define F1	(!LORAM_ || !HIRAM_ || !A15 || A14 || !A13 ||	\
		 AEC_ || !R_W_ || !GAME_ )
/* KERNAL_ */
#define F2	((!HIRAM_ || !A15 || !A14 || !A13 || AEC_ || 	\
		  !R_W_ || !GAME_) &&				\
		 (!HIRAM_ || !A15 || !A14 || !A13 || AEC_ || 	\
		  !R_W_ || EXROM_ || GAME_ ))
/* CHAROM_ */
#define F3	((!HIRAM_ || CHAREN_ || !A15 || !A14 || A13 || 	\
		  !A12 || AEC_ || !R_W_ || !GAME_) &&		\
		 (!LORAM_ || CHAREN_ || !A15 || !A14 || A13 || 	\
		  !A12 || AEC_ || !R_W_ || !GAME_) &&		\
		 (!HIRAM_ || CHAREN_ || !A15 || !A14 || A13 || 	\
		  !A12 || AEC_ || !R_W_ || EXROM_ || GAME_) &&	\
		 (!VA14_ || !AEC_ || !GAME_ || VA13 || !VA12) &&	\
		 (!VA14_ || !AEC_ || EXROM_ || GAME_ || VA13 || !VA12))
/* GR_W_ */
#define F4	(CAS_ || !A15 || !A14 || A13 || !A12 || AEC_ || R_W_)
/* I_O_ */
#define F5	((!HIRAM_ || !CHAREN_ || !A15 || !A14 || A13 || 	\
		  !A12 || !BA || AEC_ || !R_W_ || !GAME_) &&		\
		 (!HIRAM_ || !CHAREN_ || !A15 || !A14 || A13 || 	\
		  !A12 || AEC_ || R_W_ || !GAME_) &&			\
		 (!LORAM_ || !CHAREN_ || !A15 || !A14 || A13 || 	\
		  !A12 || !BA || AEC_ || !R_W_ || !GAME_) &&		\
		 (!LORAM_ || !CHAREN_ || !A15 || !A14 || A13 || 	\
		  !A12 || AEC_ || R_W_ || !GAME_) &&			\
		 (!HIRAM_ || !CHAREN_ || !A15 || !A14 || A13 || 	\
		  !A12 || !BA || AEC_ || !R_W_ || EXROM_ || 		\
		  GAME_) &&						\
		 (!HIRAM_ || !CHAREN_ || !A15 || !A14 || A13 || 	\
		  !A12 || AEC_ || R_W_ || EXROM_ || GAME_) &&		\
		 (!LORAM_ || !CHAREN_ || !A15 || !A14 || A13 || 	\
		  !A12 || !BA || AEC_ || !R_W_ || EXROM_ || 		\
		  GAME_) &&						\
		 (!LORAM_ || !CHAREN_ || !A15 || !A14 || A13 || 	\
		  !A12 || AEC_ || R_W_ || EXROM_ || GAME_) &&		\
		 (!A15 || !A14 || A13 || !A12 || !BA || 		\
		  AEC_ || !R_W_ || !EXROM_ || GAME_) &&			\
		 (!A15 || !A14 || A13 || !A12 || AEC_ || 		\
		  R_W_ || !EXROM_ || GAME_ ))
/* ROML_ */
#define F6	((!LORAM_ || !HIRAM_ || !A15 || A14 || A13 ||		\
		  AEC_ || !R_W_ || EXROM_) &&				\
		 (!A15 || A14 || A13 || AEC_ || !EXROM_ || GAME_))
/* ROMH_ */
#define F7	((!HIRAM_ || !A15 || A14 || !A13 ||			\
		  AEC_ || !R_W_ || EXROM_ || GAME_ ) &&			\
		 (!A15 || !A14 || !A13 || AEC_ || !EXROM_ || GAME_) &&	\
		 (!AEC_ || !EXROM_ || GAME_ || !VA13 || !VA12))
/*@}*/

/** The main program
 * @param argc	command line argument count
 * @param argv	command line argument vector
 * @return	zero on successful termination
 */
uint16_t
*marko_makela_lut()
{
  uint16_t *mm_lut = malloc(0x10000 * sizeof(uint16_t));
  if (!mm_lut) return NULL;

  /** The input combination, at least 16 bits */
  size_t i = 0;
  do {
    /** The output combination, 8 bits */
    uint8_t o = 0;
    /* The outputs are permuted so that they correspond to the adapter
     * made by Jens Schönfeld.
     */
    if (F0) o &= ~(1 << 0);
    if (F1) o &= ~(1 << 1);
    if (F2) o &= ~(1 << 2);
    if (F3) o &= ~(1 << 3);
    if (F4) o |= 1 << 4;
    if (F5) o &= ~(1 << 5);
    if (F6) o &= ~(1 << 6);
    if (F7) o &= ~(1 << 7);

    mm_lut[i] = o;
  }
  while (++i & 0xffff);
  return mm_lut;
}

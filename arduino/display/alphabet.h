#ifndef ALPHABET_H
#define ALPHABET_H

#include "configuration.h"

#ifdef INIT_EEPROM

#define START_CHAR 0x1D	// Ascii for group separator
#define END_CHAR '~'

//TODO : compléter table Ascii

#define HEART1 { \
    0b01101100,  \
    0b10111110, \
    0b10111110, \
    0b10111110, \
    0b01011100, \
    0b00111000, \
    0b00010000, \
    0b00000000, \
  }

#define HEART2 { \
    0b00000000,  \
    0b00101000, \
    0b01011100, \
    0b01011100, \
    0b00101000, \
    0b00010000, \
    0b00000000, \
    0b00000000, \
  }

#define HEART3 { \
    0b00000000,  \
    0b00000000, \
    0b00101000, \
    0b00111000, \
    0b00010000, \
    0b00000000, \
    0b00000000, \
    0b00000000, \
  }

#define SPACE { \
    0b00000000,  \
    0b00000000, \
    0b00000000, \
    0b00000000, \
    0b00000000, \
    0b00000000, \
    0b00000000, \
    0b00000000, \
  }

#define APOSTROPHE { \
    0b00001000,  \
    0b00010000, \
    0b00100000, \
    0b00000000, \
    0b00000000, \
    0b00000000, \
    0b00000000, \
    0b00000000, \
  }


#define A { \
    0b00011000,  \
    0b00100100, \
    0b01000010, \
    0b01000010, \
    0b01111110, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
  }

#define B { \
    0b01111100,  \
    0b01000010, \
    0b01000010, \
    0b01111100, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01111100, \
  }

#define C { \
    0b00011110,  \
    0b00100000, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b00100000, \
    0b00011110, \
  }

#define D { \
    0b01111000,  \
    0b01000100, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000100, \
    0b01111000, \
  }

#define E  { \
    0b01111110,  \
    0b01000000, \
    0b01000000, \
    0b01111110, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01111110, \
  }

#define F { \
    0b01111110,  \
    0b01000000, \
    0b01000000, \
    0b01111100, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
  }

#define G { \
    0b00011110,  \
    0b00100000, \
    0b01000000, \
    0b01000000, \
    0b01001110, \
    0b01000010, \
    0b00100010, \
    0b00011100, \
  }

#define H { \
    0b01000010,  \
    0b01000010, \
    0b01000010, \
    0b01111110, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
  }

#define I { \
    0b00111110,  \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00111110,  \
  }

#define J { \
    0b01111110,  \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00110000, \
  }

#define K { \
    0b01000100,  \
    0b01001000, \
    0b01010000, \
    0b01100000, \
    0b01100000, \
    0b01010000, \
    0b01001000, \
    0b01000100, \
  }

#define L { \
    0b01000000,  \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01111110, \
  }

#define M { \
    0b01000010,  \
    0b01100110, \
    0b01011010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
  }

#define N { \
    0b01000010,  \
    0b01100010, \
    0b01010010, \
    0b01001010, \
    0b01000110, \
    0b01000010, \
    0b01000000, \
    0b01000000, \
  }

#define O { \
    0b00011000,  \
    0b00100100, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b00100100, \
    0b00011000, \
  }

#define P { \
    0b01111100,  \
    0b01000010, \
    0b01000010, \
    0b01111100, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
  }

#define Q { \
    0b00011000,  \
    0b00100100, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01001010, \
    0b00100100, \
    0b00011010, \
  }

#define R { \
    0b01111100,  \
    0b01000010, \
    0b01000010, \
    0b01111100, \
    0b01100000, \
    0b01010000, \
    0b01001000, \
    0b01000100, \
  }

#define S { \
    0b00111110,  \
    0b01000000, \
    0b01000000, \
    0b00111100, \
    0b00000010, \
    0b00000010, \
    0b00000010, \
    0b00111100, \
  }

#define T { \
    0b00111110,  \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
  }

#define U { \
    0b01000010,  \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b00111100, \
  }

#define V { \
    0b01000010,  \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b00100100, \
    0b00011000, \
  }

#define W { \
    0b01000010,  \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01011010, \
    0b01100110, \
    0b01000010, \
  }

#define X { \
    0b01000010,  \
    0b01000010, \
    0b00100100, \
    0b00011000, \
    0b00011000, \
    0b00100100, \
    0b01000010, \
    0b01000010, \
  }

#define Y { \
    0b01000010,  \
    0b00100100, \
    0b00011000, \
    0b00011000, \
    0b00011000, \
    0b00011000, \
    0b00011000, \
    0b00011000, \
  }

#define Z { \
    0b01111110,  \
    0b00000010, \
    0b00000100, \
    0b00001000, \
    0b00010000, \
    0b00100000, \
    0b01000000, \
    0b01111110, \
  }
#endif	// INIT_EEPROM

#endif //ALPHABET_H
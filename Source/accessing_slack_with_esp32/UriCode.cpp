#include <Arduino.h>
#include "UriCode.h"

const byte DigitMin = 0x30;
const byte DigitMax = 0x39;
const byte AlphaBMin = 0x41;
const byte AlphaBMax = 0x5A;
const byte AlphaSMin = 0x61;
const byte AlphaSMax = 0x7A;
const byte Hyphen = 0x2D;
const byte Period = 0x2E;
const byte UnderLine = 0x5F;
const byte Chilled = 0x7E;
const char* HexChar = "0123456789ABCDEF";

//***************************************
// sour文字列をURLエンコードします
// dist[]配列はsourの3倍くらいの長さを確保しておいてください
//***************************************
void URI_enCode(const unsigned char* sour, unsigned char* dist) {
  int len = strlen((const char*)sour);
  byte* bp = dist;
  for (int i = 0; i < len; i++) {
    if ((sour[i] >= DigitMin && sour[i] <= DigitMax)
        || (sour[i] >= AlphaBMin && sour[i] <= AlphaBMax)
        || (sour[i] >= AlphaSMin && sour[i] <= AlphaSMax)
        || sour[i] == Hyphen
        || sour[i] == Period
        || sour[i] == UnderLine
        || sour[i] == Chilled) {
      *bp = sour[i];
      bp++;
    } else {
      *bp = '%';
      bp++;
      *bp = (byte)HexChar[sour[i] >> 4];
      bp++;
      *bp = (byte)HexChar[sour[i] & 0xF];
      bp++;
    }
  }
  *bp = 0;
}

//***************************************
// data文字列をURLデコードします
//***************************************
void URI_deCode(unsigned char* data) {
  int len = strlen((const char*)data);
  byte* bp = data;
  for (int i = 0; i < len; i++) {
    if ((data[i] >= DigitMin && data[i] <= DigitMax)
        || (data[i] >= AlphaBMin && data[i] <= AlphaBMax)
        || (data[i] >= AlphaSMin && data[i] <= AlphaSMax)
        || data[i] == Hyphen
        || data[i] == Period
        || data[i] == UnderLine
        || data[i] == Chilled) {
      *bp = data[i];
      bp++;
    } else if (data[i] == '%') {
      int c0 = data[i + 1];
      int c1 = data[i + 2];
      c0 = c0 < 0x41 ? c0 - 0x30 : c0 - 55;
      c1 = c1 < 0x41 ? c1 - 0x30 : c1 - 55;
      *bp = (byte)(c0 * 16 + c1);
      bp++;
      i += 2;
    }
  }
  *bp = 0;
}
//カラーグラフィックライブラリ

#include "graphlib.h"
#include "LCDdriver.h"

unsigned short palette[256];
const unsigned char __attribute__((weak)) FontData[256*8]={
//フォントデータ、キャラクタコード順に8バイトずつ、上位ビットが左
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x08,0x0C,0xFE,0xFE,0x0C,0x08,0x00,
	0x00,0x20,0x60,0xFE,0xFE,0x60,0x20,0x00,
	0x18,0x3C,0x7E,0x18,0x18,0x18,0x18,0x00,
	0x00,0x18,0x18,0x18,0x18,0x7E,0x3C,0x18,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x30,0x30,0x30,0x30,0x00,0x00,0x30,0x00,
	0x6C,0x6C,0x6C,0x00,0x00,0x00,0x00,0x00,
	0x6C,0x6C,0xFE,0x6C,0xFE,0x6C,0x6C,0x00,
	0x18,0x7E,0xD8,0x7E,0x1A,0xFE,0x18,0x00,
	0xE0,0xE6,0x0C,0x18,0x30,0x6E,0xCE,0x00,
	0x78,0xCC,0xD8,0x70,0xDE,0xCC,0x76,0x00,
	0x0C,0x18,0x30,0x00,0x00,0x00,0x00,0x00,
	0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00,
	0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00,
	0xD6,0x7C,0x38,0xFE,0x38,0x7C,0xD6,0x00,
	0x00,0x30,0x30,0xFC,0x30,0x30,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x60,
	0x00,0x00,0x00,0xFE,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x38,0x38,0x00,
	0x00,0x06,0x0C,0x18,0x30,0x60,0xC0,0x00,
	0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,
	0x18,0x38,0x78,0x18,0x18,0x18,0x7E,0x00,
	0x7C,0xC6,0x06,0x1C,0x70,0xC0,0xFE,0x00,
	0x7C,0xC6,0x06,0x3C,0x06,0xC6,0x7C,0x00,
	0x0C,0x1C,0x3C,0x6C,0xFE,0x0C,0x0C,0x00,
	0xFE,0xC0,0xF8,0x0C,0x06,0xCC,0x78,0x00,
	0x3C,0x60,0xC0,0xFC,0xC6,0xC6,0x7C,0x00,
	0xFE,0xC6,0x0C,0x18,0x30,0x30,0x30,0x00,
	0x7C,0xC6,0xC6,0x7C,0xC6,0xC6,0x7C,0x00,
	0x7C,0xC6,0xC6,0x7E,0x06,0x0C,0x78,0x00,
	0x00,0x30,0x00,0x00,0x00,0x30,0x00,0x00,
	0x00,0x30,0x00,0x00,0x00,0x30,0x30,0x60,
	0x0C,0x18,0x30,0x60,0x30,0x18,0x0C,0x00,
	0x00,0x00,0xFE,0x00,0xFE,0x00,0x00,0x00,
	0x60,0x30,0x18,0x0C,0x18,0x30,0x60,0x00,
	0x7C,0xC6,0x06,0x1C,0x30,0x00,0x30,0x00,
	0x3C,0x66,0xDE,0xF6,0xDC,0x60,0x3E,0x00,
	0x38,0x6C,0xC6,0xFE,0xC6,0xC6,0xC6,0x00,
	0xFC,0x66,0x66,0x7C,0x66,0x66,0xFC,0x00,
	0x3C,0x66,0xC0,0xC0,0xC0,0x66,0x3C,0x00,
	0xF8,0x6C,0x66,0x66,0x66,0x6C,0xF8,0x00,
	0xFE,0xC0,0xC0,0xF8,0xC0,0xC0,0xFE,0x00,
	0xFE,0xC0,0xC0,0xF8,0xC0,0xC0,0xC0,0x00,
	0x3C,0x66,0xC0,0xCE,0xC6,0x66,0x3C,0x00,
	0xC6,0xC6,0xC6,0xFE,0xC6,0xC6,0xC6,0x00,
	0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,
	0x1E,0x0C,0x0C,0x0C,0x0C,0xCC,0x78,0x00,
	0xC6,0xCC,0xD8,0xF0,0xD8,0xCC,0xC6,0x00,
	0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xFE,0x00,
	0xC6,0xEE,0xFE,0xD6,0xC6,0xC6,0xC6,0x00,
	0xC6,0xE6,0xF6,0xDE,0xCE,0xC6,0xC6,0x00,
	0x38,0x6C,0xC6,0xC6,0xC6,0x6C,0x38,0x00,
	0xFC,0xC6,0xC6,0xFC,0xC0,0xC0,0xC0,0x00,
	0x38,0x6C,0xC6,0xC6,0xDE,0x6C,0x3E,0x00,
	0xFC,0xC6,0xC6,0xFC,0xD8,0xCC,0xC6,0x00,
	0x7C,0xC6,0xC0,0x7C,0x06,0xC6,0x7C,0x00,
	0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00,
	0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,
	0xC6,0xC6,0xC6,0x6C,0x6C,0x38,0x38,0x00,
	0xC6,0xC6,0xC6,0xD6,0xFE,0xEE,0xC6,0x00,
	0xC6,0xC6,0x6C,0x38,0x6C,0xC6,0xC6,0x00,
	0xCC,0xCC,0xCC,0x78,0x30,0x30,0x30,0x00,
	0xFE,0x06,0x0C,0x38,0x60,0xC0,0xFE,0x00,
	0x3C,0x30,0x30,0x30,0x30,0x30,0x3C,0x00,
	0xCC,0xCC,0x78,0xFC,0x30,0xFC,0x30,0x00,
	0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00,
	0x30,0x78,0xCC,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x00,
	0x30,0x18,0x0C,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x7C,0x0C,0x7C,0xCC,0x7E,0x00,
	0xC0,0xC0,0xFC,0xE6,0xC6,0xE6,0xFC,0x00,
	0x00,0x00,0x7C,0xC6,0xC0,0xC6,0x7C,0x00,
	0x06,0x06,0x7E,0xCE,0xC6,0xCE,0x7E,0x00,
	0x00,0x00,0x7C,0xC6,0xFE,0xC0,0x7C,0x00,
	0x1C,0x36,0x30,0xFC,0x30,0x30,0x30,0x00,
	0x00,0x00,0x7E,0xCE,0xCE,0x7E,0x06,0x7C,
	0xC0,0xC0,0xFC,0xE6,0xC6,0xC6,0xC6,0x00,
	0x18,0x00,0x38,0x18,0x18,0x18,0x3C,0x00,
	0x0C,0x00,0x1C,0x0C,0x0C,0x0C,0xCC,0x78,
	0xC0,0xC0,0xCC,0xD8,0xF0,0xF8,0xCC,0x00,
	0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,
	0x00,0x00,0xFC,0xB6,0xB6,0xB6,0xB6,0x00,
	0x00,0x00,0xFC,0xE6,0xC6,0xC6,0xC6,0x00,
	0x00,0x00,0x7C,0xC6,0xC6,0xC6,0x7C,0x00,
	0x00,0x00,0xFC,0xE6,0xE6,0xFC,0xC0,0xC0,
	0x00,0x00,0x7E,0xCE,0xCE,0x7E,0x06,0x06,
	0x00,0x00,0xDC,0xE6,0xC0,0xC0,0xC0,0x00,
	0x00,0x00,0x7E,0xC0,0x7C,0x06,0xFC,0x00,
	0x30,0x30,0xFC,0x30,0x30,0x36,0x1C,0x00,
	0x00,0x00,0xC6,0xC6,0xC6,0xCE,0x76,0x00,
	0x00,0x00,0xC6,0xC6,0xC6,0x6C,0x38,0x00,
	0x00,0x00,0x86,0xB6,0xB6,0xB6,0xFC,0x00,
	0x00,0x00,0xC6,0x6C,0x38,0x6C,0xC6,0x00,
	0x00,0x00,0xC6,0xC6,0xCE,0x7E,0x06,0x7C,
	0x00,0x00,0xFE,0x0C,0x38,0x60,0xFE,0x00,
	0x3C,0x60,0x60,0xC0,0x60,0x60,0x3C,0x00,
	0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x00,
	0xF0,0x18,0x18,0x0C,0x18,0x18,0xF0,0x00,
	0x60,0xB6,0x1C,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
	0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,
	0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,
	0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
	0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,
	0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,
	0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,
	0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,
	0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,
	0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,
	0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,
	0x18,0x18,0x18,0x18,0xFF,0x18,0x18,0x18,
	0x18,0x18,0x18,0x18,0xFF,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0xFF,0x18,0x18,0x18,
	0x18,0x18,0x18,0x18,0xF8,0x18,0x18,0x18,
	0x18,0x18,0x18,0x18,0x1F,0x18,0x18,0x18,
	0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,
	0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
	0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,
	0x00,0x00,0x00,0x00,0x1F,0x18,0x18,0x18,
	0x00,0x00,0x00,0x00,0xF8,0x18,0x18,0x18,
	0x18,0x18,0x18,0x18,0x1F,0x00,0x00,0x00,
	0x18,0x18,0x18,0x18,0xF8,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x07,0x0C,0x18,0x18,
	0x00,0x00,0x00,0x00,0xE0,0x30,0x18,0x18,
	0x18,0x18,0x18,0x0C,0x07,0x00,0x00,0x00,
	0x18,0x18,0x18,0x30,0xE0,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x78,0x68,0x78,0x00,
	0x78,0x60,0x60,0x60,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x18,0x18,0x18,0x78,0x00,
	0x00,0x00,0x00,0x00,0x60,0x30,0x18,0x00,
	0x00,0x00,0x00,0x30,0x00,0x00,0x00,0x00,
	0xFE,0x06,0x06,0xFE,0x06,0x0C,0x78,0x00,
	0x00,0x00,0xFC,0x0C,0x38,0x30,0x60,0x00,
	0x00,0x00,0x0C,0x18,0x38,0x78,0x18,0x00,
	0x00,0x00,0x30,0xFC,0xCC,0x0C,0x38,0x00,
	0x00,0x00,0x00,0xFC,0x30,0x30,0xFC,0x00,
	0x00,0x00,0x18,0xFC,0x38,0x78,0xD8,0x00,
	0x00,0x00,0x60,0xFC,0x6C,0x68,0x60,0x00,
	0x00,0x00,0x00,0x78,0x18,0x18,0xFC,0x00,
	0x00,0x00,0x7C,0x0C,0x7C,0x0C,0x7C,0x00,
	0x00,0x00,0x00,0xAC,0xAC,0x0C,0x38,0x00,
	0x00,0x00,0x00,0xFE,0x00,0x00,0x00,0x00,
	0xFE,0x06,0x06,0x34,0x38,0x30,0x60,0x00,
	0x06,0x0C,0x18,0x38,0x78,0xD8,0x18,0x00,
	0x18,0xFE,0xC6,0xC6,0x06,0x0C,0x38,0x00,
	0x00,0x7E,0x18,0x18,0x18,0x18,0x7E,0x00,
	0x18,0xFE,0x18,0x38,0x78,0xD8,0x18,0x00,
	0x30,0xFE,0x36,0x36,0x36,0x36,0x6C,0x00,
	0x18,0x7E,0x18,0x7E,0x18,0x18,0x18,0x00,
	0x3E,0x66,0xC6,0x0C,0x18,0x30,0xE0,0x00,
	0x60,0x7E,0xD8,0x18,0x18,0x18,0x30,0x00,
	0x00,0xFE,0x06,0x06,0x06,0x06,0xFE,0x00,
	0x6C,0xFE,0x6C,0x0C,0x0C,0x18,0x30,0x00,
	0x00,0xF0,0x00,0xF6,0x06,0x0C,0xF8,0x00,
	0xFE,0x06,0x0C,0x18,0x38,0x6C,0xC6,0x00,
	0x60,0xFE,0x66,0x6C,0x60,0x60,0x3E,0x00,
	0xC6,0xC6,0x66,0x06,0x0C,0x18,0xF0,0x00,
	0x3E,0x66,0xE6,0x3C,0x18,0x30,0xE0,0x00,
	0x0C,0x78,0x18,0xFE,0x18,0x18,0xF0,0x00,
	0x00,0xD6,0xD6,0xD6,0x0C,0x18,0xF0,0x00,
	0x7C,0x00,0xFE,0x18,0x18,0x30,0x60,0x00,
	0x30,0x30,0x38,0x3C,0x36,0x30,0x30,0x00,
	0x18,0x18,0xFE,0x18,0x18,0x30,0x60,0x00,
	0x00,0x7C,0x00,0x00,0x00,0x00,0xFE,0x00,
	0x00,0x7E,0x06,0x6C,0x18,0x36,0x60,0x00,
	0x18,0x7E,0x0C,0x18,0x3C,0x7E,0x18,0x00,
	0x06,0x06,0x06,0x0C,0x18,0x30,0x60,0x00,
	0x30,0x18,0x0C,0xC6,0xC6,0xC6,0xC6,0x00,
	0xC0,0xC0,0xFE,0xC0,0xC0,0xC0,0x7E,0x00,
	0x00,0xFE,0x06,0x06,0x0C,0x18,0x70,0x00,
	0x00,0x30,0x78,0xCC,0x06,0x06,0x00,0x00,
	0x18,0x18,0xFE,0x18,0xDB,0xDB,0x18,0x00,
	0xFE,0x06,0x06,0x6C,0x38,0x30,0x18,0x00,
	0x00,0x3C,0x00,0x3C,0x00,0x7C,0x06,0x00,
	0x0C,0x18,0x30,0x60,0xCC,0xFC,0x06,0x00,
	0x02,0x36,0x3C,0x18,0x3C,0x6C,0xC0,0x00,
	0x00,0xFE,0x30,0xFE,0x30,0x30,0x3E,0x00,
	0x30,0x30,0xFE,0x36,0x3C,0x30,0x30,0x00,
	0x00,0x78,0x18,0x18,0x18,0x18,0xFE,0x00,
	0xFE,0x06,0x06,0xFE,0x06,0x06,0xFE,0x00,
	0x7C,0x00,0xFE,0x06,0x0C,0x18,0x30,0x00,
	0xC6,0xC6,0xC6,0x06,0x06,0x0C,0x38,0x00,
	0x6C,0x6C,0x6C,0x6E,0x6E,0x6C,0xC8,0x00,
	0x60,0x60,0x60,0x66,0x6C,0x78,0x70,0x00,
	0x00,0xFE,0xC6,0xC6,0xC6,0xC6,0xFE,0x00,
	0x00,0xFE,0xC6,0xC6,0x06,0x0C,0x38,0x00,
	0x00,0xF0,0x06,0x06,0x0C,0x18,0xF0,0x00,
	0x18,0xCC,0x60,0x00,0x00,0x00,0x00,0x00,
	0x70,0xD8,0x70,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0xFF,0x00,0x00,0xFF,0x00,0x00,
	0x18,0x18,0x1F,0x18,0x18,0x1F,0x18,0x18,
	0x18,0x18,0xFF,0x18,0x18,0xFF,0x18,0x18,
	0x18,0x18,0xF8,0x18,0x18,0xF8,0x18,0x18,
	0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF,
	0x80,0xC0,0xE0,0xF0,0xF8,0xFC,0xFE,0xFF,
	0xFF,0x7F,0x3F,0x1F,0x0F,0x07,0x03,0x01,
	0xFF,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80,
	0x10,0x38,0x7C,0xFE,0xFE,0x38,0x7C,0x00,
	0x6C,0xFE,0xFE,0xFE,0x7C,0x38,0x10,0x00,
	0x10,0x38,0x7C,0xFE,0x7C,0x38,0x10,0x00,
	0x38,0x38,0xFE,0xFE,0xD6,0x10,0x7C,0x00,
	0x00,0x3C,0x7E,0x7E,0x7E,0x7E,0x3C,0x00,
	0x00,0x7C,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,
	0x03,0x06,0x0C,0x18,0x30,0x60,0xC0,0x80,
	0x80,0xC0,0x60,0x30,0x18,0x0C,0x06,0x03,
	0x83,0xC6,0x6C,0x38,0x38,0x6C,0xC6,0x83,
	0xFE,0xB6,0xB6,0xFE,0x86,0x86,0x86,0x00,
	0xC0,0xFE,0xD8,0x7E,0x58,0xFE,0x18,0x00,
	0x7E,0x66,0x7E,0x66,0x7E,0x66,0xC6,0x00,
	0xFE,0xC6,0xC6,0xFE,0xC6,0xC6,0xFE,0x00,
	0x06,0xEF,0xA6,0xFF,0xA2,0xFF,0x0A,0x06,
	0x00,0x38,0x6C,0xC6,0x7C,0x34,0x6C,0x00,
	0xFC,0x6C,0xFE,0x6E,0xF6,0xEC,0x6C,0x78,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

volatile unsigned short drawcount;
volatile unsigned short delaytimerflag;

void set_palette(unsigned char n,unsigned char b,unsigned char r,unsigned char g){
//グラフィック用カラーパレット設定
	palette[n]=((r>>3)<<11)+((g>>2)<<5)+(b>>3);
}

void pset(int x,int y,unsigned char c)
// (x,y)の位置にカラーパレット番号cで点を描画
{
	if(x>=0 && x<X_RES && y>=0 && y<Y_RES)
		drawPixel(x,y,palette[c]);
}

void putbmpmn(int x,int y,unsigned char m,unsigned char n,const unsigned char bmp[])
// 横m*縦nドットのキャラクターを座標x,yに表示
// unsigned char bmp[m*n]配列に、単純にカラー番号を並べる
// カラー番号が0の部分は透明色として扱う
{
	int i,j;
	int skip;
	const unsigned char *p;
	if(x<=-m || x>X_RES || y<=-n || y>=Y_RES) return; //画面外
	if(y<0){ //画面上部に切れる場合
		i=0;
		p=bmp-y*m;
	}
	else{
		i=y;
		p=bmp;
	}
	for(;i<y+n;i++){
		if(i>=Y_RES) return; //画面下部に切れる場合
		if(x<0){ //画面左に切れる場合は残る部分のみ描画
			j=0;
			p+=-x;
		}
		else{
			j=x;
		}
		skip=1;
		for(;j<x+m;j++){
			if(j>=X_RES){ //画面右に切れる場合
				p+=x+m-j;
				break;
			}
			if(*p!=0){ //カラー番号が0の場合、透明として処理
				if(skip){
					LCD_SetCursor(j,i);
					skip=0;
				}
				LCD_WriteData2(palette[*p]);
			}
			else skip=1;
			p++;
		}
	}
}


void LCD_continuous_output(unsigned short x,unsigned short y,unsigned short color,int n);

// 縦m*横nドットのキャラクター消去
// カラー0で塗りつぶし
void clrbmpmn(int x,int y,unsigned char m,unsigned char n)
{
	int i,j,k;
	if(x<=-m || x>X_RES || y<=-n || y>=Y_RES) return; //画面外
	if(y<0) i=0; //画面上部に切れる場合
	else i=y;
	if(x<0) j=0; //画面左に切れる場合は残る部分のみ描画
	else j=x;
	if(x+m>=X_RES) k=X_RES-j; //画面右に切れる場合
	else k=x+m-j;
	for(;i<y+n;i++){
		if(i>=Y_RES) return; //画面下部に切れる場合
		LCD_continuous_output(j,i,0,k);
	}
}

void gline(int x1,int y1,int x2,int y2,unsigned char c)
// (x1,y1)-(x2,y2)にカラーパレット番号cで線分を描画
{
	int sx,sy,dx,dy,i;
	int e;

	if(x2>x1){
		dx=x2-x1;
		sx=1;
	}
	else{
		dx=x1-x2;
		sx=-1;
	}
	if(y2>y1){
		dy=y2-y1;
		sy=1;
	}
	else{
		dy=y1-y2;
		sy=-1;
	}
	if(dx>=dy){
		e=-dx;
		for(i=0;i<=dx;i++){
			pset(x1,y1,c);
			x1+=sx;
			e+=dy*2;
			if(e>=0){
				y1+=sy;
				e-=dx*2;
			}
		}
	}
	else{
		e=-dy;
		for(i=0;i<=dy;i++){
			pset(x1,y1,c);
			y1+=sy;
			e+=dx*2;
			if(e>=0){
				x1+=sx;
				e-=dy*2;
			}
		}
	}
}
void hline(int x1,int x2,int y,unsigned char c)
// (x1,y)-(x2,y)への水平ラインを高速描画
{
	int temp;

	if(y<0 || y>=Y_RES) return;
	if(x1>x2){
		temp=x1;
		x1=x2;
		x2=temp;
	}
	if(x2<0 || x1>=X_RES) return;
	if(x1<0) x1=0;
	if(x2>=X_RES) x2=X_RES-1;
	LCD_continuous_output(x1,y,palette[c],x2-x1+1);
}

void circle(int x0,int y0,unsigned int r,unsigned char c)
// (x0,y0)を中心に、半径r、カラーパレット番号cの円を描画
{
	int x,y,f;
	x=r;
	y=0;
	f=-2*r+3;
	while(x>=y){
		pset(x0-x,y0-y,c);
		pset(x0-x,y0+y,c);
		pset(x0+x,y0-y,c);
		pset(x0+x,y0+y,c);
		pset(x0-y,y0-x,c);
		pset(x0-y,y0+x,c);
		pset(x0+y,y0-x,c);
		pset(x0+y,y0+x,c);
		if(f>=0){
			x--;
			f-=x*4;
		}
		y++;
		f+=y*4+2;
	}
}
void boxfill(int x1,int y1,int x2,int y2,unsigned char c)
// (x1,y1),(x2,y2)を対角線とするカラーパレット番号cで塗られた長方形を描画
{
	int temp;
	if(x1>x2){
		temp=x1;
		x1=x2;
		x2=temp;
	}
	if(x2<0 || x1>=X_RES) return;
	if(y1>y2){
		temp=y1;
		y1=y2;
		y2=temp;
	}
	if(y2<0 || y1>=Y_RES) return;
	if(y1<0) y1=0;
	if(y2>=Y_RES) y2=Y_RES-1;
	while(y1<=y2){
		hline(x1,x2,y1++,c);
	}
}
void circlefill(int x0,int y0,unsigned int r,unsigned char c)
// (x0,y0)を中心に、半径r、カラーパレット番号cで塗られた円を描画
{
	int x,y,f;
	x=r;
	y=0;
	f=-2*r+3;
	while(x>=y){
		hline(x0-x,x0+x,y0-y,c);
		hline(x0-x,x0+x,y0+y,c);
		hline(x0-y,x0+y,y0-x,c);
		hline(x0-y,x0+y,y0+x,c);
		if(f>=0){
			x--;
			f-=x*4;
		}
		y++;
		f+=y*4+2;
	}
}

void putfont(int x,int y,unsigned char c,int bc,unsigned char n)
//8*8ドットのアルファベットフォント表示
//座標(x,y)、カラーパレット番号c
//bc:バックグランドカラー、負数の場合無視
//n:文字番号
{
	int i,j;
	unsigned char d;
	int skip;
	unsigned short c1;
	const unsigned char *p;
	static unsigned char lcddatabuf[16];
	unsigned char *lcdbufp;
	if(x<=-8 || x>=X_RES || y<=-8 || y>=Y_RES) return; //画面外
	if(y<0){ //画面上部に切れる場合
		i=0;
		p=FontData+n*8-y;
	}
	else{
		i=y;
		p=FontData+n*8;
	}
	c1=palette[c];
	if(bc>=0) bc=palette[bc];
	for(;i<y+8;i++){
		if(i>=Y_RES) return; //画面下部に切れる場合
		d=*p++;
		if(x<0){ //画面左に切れる場合は残る部分のみ描画
			j=0;
			d<<=-x;
		}
		else{
			j=x;
		}
		if(bc<0){
			skip=1;
			for(;j<x+8;j++){
				if(j>=X_RES){ //画面右に切れる場合
					break;
				}
				if(d&0x80){
					if(skip){
						LCD_SetCursor(j,i);
						skip=0;
					}
					LCD_WriteData2(c1);
				}
				else skip=1;
				d<<=1;
			}
		}
		else{
			LCD_SetCursor(j,i);
			lcdbufp=lcddatabuf;
			for(;j<x+8;j++){
				if(j>=X_RES){ //画面右に切れる場合
					break;
				}
				if(d&0x80){
					*lcdbufp++=c1>>8;
					*lcdbufp++=(unsigned char)c1;
				}
				else{
					*lcdbufp++=bc>>8;
					*lcdbufp++=(unsigned char)bc;
				}
				d<<=1;
			}
			if(lcdbufp!=lcddatabuf) LCD_WriteDataN(lcddatabuf,lcdbufp-lcddatabuf);
		}
	}
}

void printstr(int x,int y,unsigned char c,int bc,unsigned char *s){
	//座標(x,y)からカラーパレット番号cで文字列sを表示、bc:バックグランドカラー
	//bcが負の場合は無視
	while(*s){
		putfont(x,y,c,bc,*s++);
		x+=8;
	}
}
void printnum(int x,int y,unsigned char c,int bc,unsigned int n){
	//座標(x,y)にカラー番号cで数値nを表示、bc:バックグランドカラー
	unsigned int d,e;
	d=10;
	e=0;
	while(n>=d){
		e++;
		if(e==9) break;
		d*=10;
	}	
	x+=e*8;
	do{
		putfont(x,y,c,bc,'0'+n%10);
		n/=10;
		x-=8;
	}while(n!=0);
}
void printnum2(int x,int y,unsigned char c,int bc,unsigned int n,unsigned char e){
	//座標(x,y)にカラー番号cで数値nを表示、bc:バックグランドカラー、e桁で表示
	if(e==0) return;
	x+=(e-1)*8;
	do{
		putfont(x,y,c,bc,'0'+n%10);
		e--;
		n/=10;
		x-=8;
	}while(e!=0 && n!=0);
	while(e!=0){
		putfont(x,y,c,bc,' ');
		x-=8;
		e--;
	}
}

void init_graphic(void){
	//グラフィックLCD使用開始
	int i;
	//カラーパレット初期化
	for(i=0;i<8;i++){
		set_palette(i,255*(i&1),255*((i>>1)&1),255*(i>>2));
	}
	for(i=0;i<8;i++){
		set_palette(i+8,128*(i&1),128*((i>>1)&1),128*(i>>2));
	}
	for(i=16;i<256;i++){
		set_palette(i,255,255,255);
	}

	LCD_Init();
	LCD_Clear(0);
}
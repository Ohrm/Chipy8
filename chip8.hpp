#ifndef CHIP8_HPP
#define CHIP8_HPP

#include <string>
#include <stdio.h>

class chip8{

public:
    void Init();
    bool LoadGame(char* name);
    void EmualteCycle();
	void DecreaseTimers();
    bool drawFlag;
    unsigned char gfx[64*32];
	unsigned char key[16];

private:
    unsigned char memory[4096];
    unsigned char V[16];
    unsigned char delayTimer;
    unsigned char soundTimer;

    unsigned short opcode;
    unsigned short I;
    unsigned short pc;
    unsigned short stackp[16];
    unsigned short sp;

    //Font set
    static unsigned char chip8_fontset[];

};

#endif // CHIP8_HPP

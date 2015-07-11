#include "chip8.hpp"
#include <stdlib.h>
#include <stdio.h>

unsigned char chip8::chip8_fontset[80] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F

};

void chip8::Init(){

    // Initialize registers and memory once
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;

    // Clear display
	for(int i = 0; i < 2048; i++)
		gfx[i] = 0;
    // Clear stack
	for(int i = 0; i < 16; i++)
		stackp[i] = 0;
    // Clear registers V0-VF
    for(int i = 0; i < 16; i++)
        key[i] = V[i] = 0;
    // Clear memory
	for(int i = 0; i < 4096; ++i)
		memory[i] = 0;

    // Load fontset
    for (int i = 0; i < 80; ++i){

        memory[i] = chip8_fontset[i];

    }

    // Reset timers
    delayTimer = 0;
    soundTimer = 0;

	drawFlag = true;

}

bool chip8::LoadGame(char* name){

    FILE * myFile;
    myFile = fopen(name, "rb");

    if(myFile != NULL){

        fread(&memory[0x200], 0xFFF, 1, myFile);
        printf("File loaded ");
		fclose(myFile);
		return true;

    }else{

        printf("File not found ");
		return false;

    }

}

void chip8::EmualteCycle(){

    //Fetch opcode
    opcode = memory[pc] << 8 | memory[pc + 1];

    //Decode opcode then execute
    switch(opcode & 0xF000){

    case 0x0000:
        switch(opcode & 0x000F){

        case 0x0000: //0x00E0 clears the screen
            for(int i = 0; i < 2048; ++i)
                gfx[i] = 0x0;
                drawFlag = true;
				pc += 2;
            break;

        case 0x000E: // 0x000E returns from subroutine
            --sp;
            pc = stackp[sp];
            pc += 2;
            break;

        }
		break;
    case 0x1000: //1NNN jump to address at NNN
        pc = opcode & 0x0FFF;
        break;

    case 0x2000: //2NNN calls subroutine at NNN
        stackp[sp] = pc;
        ++sp;
        pc = opcode & 0x0FFF;
        break;

    case 0x3000: //3XNN skips the next instruction if VX equals NN.
        if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
            pc += 4;
        else
            pc += 2;
        break;

    case 0x4000: //4XNN skips the next instruction if VX doesn't equal NN.
        if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
            pc += 4;
        else
            pc += 2;
        break;

    case 0x5000: //5XY0 skips the next instruction if VX equals VY.
        if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
            pc += 4;
        else
            pc += 2;
        break;

    case 0x6000: //6XNN sets VX to NN
        V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
        pc += 2;
        break;

    case 0x7000: //7XNN adds NN to VX
        V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
        pc += 2;
        break;

    case 0x8000:
        switch(opcode & 0x000F){

        case 0x0000: //8XY0 sets VX to the value of VY
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;

        case 0x0001: //8XY1 sets VX to VX or VY.
            V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;

        case 0x0002: //8XY2 sets VX to VX and VY.
            V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;

        case 0x0003: //8XY3 sets VX to VX xor VY.
            V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;

        case 0x0004: //8XY4 adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.

            if(V[(opcode & 0x00F0) >> 4 ] > (0xFF - V[(opcode & 0x0F00) >> 8]))
                V[0xF] = 1; //Carry
            else
                V[0xF] = 0; //No carry

            V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;

        case 0x0005: //8XY5 VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
           if(V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
                V[0xF] = 0; //Borrow
            else
                V[0xF] = 1; //No Borrow

            V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
            pc +=2;

            break;

        case 0x0006: //8XY6 shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift
            V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
            V[(opcode & 0x0F00) >> 8] >>= 1;
            pc += 2;
            break;

        case 0x0007: //8XY7 sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
            if(V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])	// VY-VX
                V[0xF] = 0; // there is a borrow
            else
                V[0xF] = 1;
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
            pc += 2;
            break;

        case 0x000E: //8XYE shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.
            V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
            V[(opcode & 0x0F00) >> 8] <<= 1;
            pc += 2;
            break;

        }
        break;
    case 0x9000: //9XY0 skips the next instruction if VX doesn't equal VY.
        if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
            pc += 4;
        else
            pc += 2;
        break;

    case 0xA000: //ANNN sets i to the address NNN
        I = opcode & 0x0FFF;
        pc += 2;
        break;

    case 0xB000: //BNNN jumps to the address NNN plus V0
        pc = (opcode & 0x0FFF) + V[0];
        break;

    case 0xC000: //CXNN Sets VX to a random number, masked by NN.
        V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
        pc += 2;
        break;

    case 0xD000: //DXYN sprites stored in memory at location in index register (I), maximum 8bits wide. Wraps around the screen. If when drawn, clears a pixel, register VF is set to 1 otherwise it is zero. All drawing is XOR drawing (i.e. it toggles the screen pixels)
        {

            unsigned short x = V[(opcode & 0x0F00) >> 8];
            unsigned short y = V[(opcode & 0x00F0) >> 4];
            unsigned short height = opcode & 0x000F;
            unsigned short pixel;

            V[0xF] = 0;
            for(int yline = 0; yline < height; yline++){

                pixel = memory[I + yline];
                for(int xline = 0; xline < 8; xline++){

                    if((pixel & (0x80 >> xline)) != 0){

                        if(gfx[(x + xline + ((y + yline) * 64))] == 1)
                            V[0xF] = 1;
                        gfx[x + xline + ((y + yline) * 64)] ^= 1;

                    }

                }

            }

            drawFlag = true;
            pc += 2;

        }
        break;

    case 0xE000:
        switch(opcode & 0x00FF){

        case 0x009E: //EX9E skips the next instruction if the key stored in VX is pressed.
            if(key[V[(opcode & 0x0F00) >> 8]] != 0)
                pc += 4;
            else
                pc += 2;
            break;

        case 0x00A1: //EXA1 skips the next instruction if the key stored in VX isn't pressed.
            if(key[V[(opcode & 0x0F00) >> 8]] == 0)
                pc += 4;
            else
                pc += 2;
            break;

        }
        break;

    case 0xF000:
        switch(opcode & 0x00FF){

        case 0x0007: //FX07 sets VX to the value of the delay timer.
            V[(opcode & 0x0F00) >> 8] = delayTimer;
            pc += 2;
            break;

        case 0x000A: //FX0A a key press is awaited, and then stored in VX.
            {

                bool keyPress = false;

                for(int i = 0; i < 16; i++){

                    if(key[i] != 0){

                        V[(opcode & 0x0F00) >> 8] = i;
                        keyPress = true;

                    }

                }

                if(!keyPress)
					return;

                pc += 2;

            }
            break;

        case 0x0015: //FX15 sets the delay timer to VX.
            delayTimer = V[(opcode & 0x0F00) >> 8];
            pc += 2;
            break;

        case 0x0018: //FX18 sets the sound timer to VX.
            soundTimer = V[(opcode & 0x0F00) >> 8];
            pc += 2;
            break;

        case 0x001E: //FX1E adds VX to I
            if(I + V[(opcode & 0x0F00) >> 8] > 0xFFF)	// VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't.
				V[0xF] = 1;
			else
				V[0xF] = 0;
			I += V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;
        case 0x0029: //FX29 sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
            I = V[(opcode & 0x0F00) >> 8] * 0x5;
            pc += 2;
            break;

        case 0x0033: //FX33 stores the Binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2. (In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.)
            memory[I]     = V[(opcode & 0x0F00) >> 8] / 100;
            memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
			memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
			pc += 2;
            break;

        case 0x0055: //FX55 stores V0 to VX in memory starting at address I.
            for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
				memory[I + i] = V[i];	

			// On the original interpreter, when the operation is done, I = I + X + 1.
			I += ((opcode & 0x0F00) >> 8) + 1;
			pc += 2;
			break;

        case 0x0065: //FX65 fills V0 to VX with values from memory starting at address I.
            for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
				V[i] = memory[I + i];			

			// On the original interpreter, when the operation is done, I = I + X + 1.
			I += ((opcode & 0x0F00) >> 8) + 1;
			pc += 2;
			break;

        }
        break;
    default:
        printf("Unknown opcode: 0x%X", opcode);

    }

    //Update timers
    if(delayTimer > 0)
    --delayTimer;

    if(soundTimer > 0)
    {
        if(soundTimer == 1)
        printf("BEEP!\n");
        --soundTimer;
    }

}

void chip8::SetKeys(){



}

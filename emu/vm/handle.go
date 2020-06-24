package vm

import (
	"fmt"
	"math/rand"
)

// HandleOp receives a chip8 instance and 2 byte long slice and performs any required action
// This might get moved to a chip8 method or something...idk yo
func HandleOp(c *Chip8, buf []byte) {
	// Go ahead and advance pc by 2. This will be overridden by any opcodes that exclusive set pc
	// prior to returning from the function
	c.pc += 2

	// Chip 8 instruction are technically 2 bytes long however they are read in via 1 byte increments
	// Therefore we need to combine them into a single 16 bit integer
	var op uint16
	op = uint16(buf[1])<<8 | uint16(buf[0])

	addr := op & 0xFFF

	// n/nibble is used during display
	// n := op & 0xF
	x := op & 0x0F00
	y := op & 0x00F0
	kk := byte(op & 0x00FF)

	switch op >> 12 {
	case 0x0:
		switch op & 0xFF {
		case 0xE0:
			fmt.Printf("CLS\n")
		case 0xEE:
			fmt.Printf("RET\n")
		}
	case 0x1:
		c.pc = addr
	case 0x2:
		c.pc = addr
	case 0x3:
		c.pc += 2
	case 0x4:
		c.pc += 2
	case 0x5:
		c.pc += 2
	case 0x6:
		c.v[x] = kk
	case 0x7:
		c.v[x] += kk
	case 0x8:
		switch op & 0xF {
		case 0x0:
			c.v[x] = c.v[y]
		case 0x1:
			c.v[x] |= c.v[y]
		case 0x2:
			c.v[x] &= c.v[y]
		case 0x3:
			c.v[x] ^= c.v[y]
		case 0x4:
			tmp := uint16(c.v[x]) + uint16(c.v[y])
			if tmp > 255 {
				c.v[0xF] = 1
				c.v[x] = 255
			} else {
				c.v[0xF] = 0
				c.v[x] = byte(tmp)
			}
		case 0x5:
			// TODO: This might need to be revisited. Not sure what to actually set v[x] to
			if c.v[x] > c.v[y] {
				c.v[0xF] = 1
			} else {
				c.v[0xF] = 0
			}
			c.v[x] -= c.v[y]
		case 0x6:
			c.v[0xF] = c.v[x] & 0x1
			c.v[x] /= 2
		case 0x7:
			// TODO: This might need to be revisited. Not sure what to actually set v[x] to
			if c.v[y] > c.v[x] {
				c.v[0xF] = 1
			} else {
				c.v[0xF] = 0
			}
			c.v[x] = c.v[y] - c.v[x]
		case 0xE:
			c.v[0xF] = c.v[x] >> 7
			c.v[x] *= 2
		}
	case 0x9:
		if c.v[x] != c.v[y] {
			c.pc += 2
		}
	case 0xA:
		c.i = addr
	case 0xB:
		c.pc = addr + uint16(c.v[0])
	case 0xC:
		c.v[x] = byte(rand.Intn(256)) & kk
	case 0xD:
		fmt.Println("DRW Vx, Vy, n")
		// TODO
		/*
		   Dxyn - DRW Vx, Vy, nibble
		   Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.

		   The interpreter reads n bytes from memory, starting at the address stored in I. These bytes are then displayed as sprites on screen at coordinates (Vx, Vy). Sprites are XORed onto the existing screen. If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0. If the sprite is positioned so part of it is outside the coordinates of the display, it wraps around to the opposite side of the screen. See instruction 8xy3 for more information on XOR, and section 2.4, Display, for more information on the Chip-8 screen and sprites.
		*/
	case 0xE:
		switch op & 0xFF {
		// TODO: Need to implement keyboard
		case 0x9E:
			// if keys[c.v[x]] {
			// 	c.pc += 2
			// }
		case 0xA1:
			// if !keys[c.v[x]] {
			// 	c.pc += 2
			// }
		}
	case 0xF:
		switch op & 0xFF {
		case 0x07:
			fmt.Println("LD Vx, DT")
			c.v[x] = c.dt
		case 0x0A:
			// TODO: Implement waitForInput()
			// c.v[x] = waitForInput()
		case 0x15:
			c.dt = c.v[x]
		case 0x18:
			c.st = c.v[x]
		case 0x1E:
			c.i += uint16(c.v[x])
		case 0x29:
			// TODO
			// This one might need revisiting. Supposed to set i to location of hexadecimal font
			c.i = uint16(c.v[x])
		case 0x33:
			// TODO: Figure this one out
			fmt.Println("LD B, Vx")
		case 0x55:
			var i uint16
			for i = 0x0; i <= x; i++ {
				c.mem[c.i+i] = c.v[i]
			}
		case 0x65:
			var i uint16
			for i = 0x0; i <= x; i++ {
				c.v[i] = c.mem[c.i+i]
			}
		}
	default:
		fmt.Printf("UNKNOWN OPCODE 0x%X\n", op)
	}
}

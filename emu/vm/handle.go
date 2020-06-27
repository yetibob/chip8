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
	op = uint16(buf[0])<<8 | uint16(buf[1])
	fmt.Printf("OPCODE: %#04x\n", op)

	addr := op & 0xFFF
	n := byte(op & 0xF)
	x := byte((op & 0x0F00) >> 8)
	y := byte((op & 0x00F0) >> 4)
	kk := byte(op & 0x00FF)

	switch op >> 12 {
	case 0x0:
		switch op & 0xFF {
		case 0xE0:
			for y := 0; y < len(c.display); y++ {
				for x := 0; x < len(c.display[y]); x++ {
					c.display[y][x] = 0x0
				}
			}
		case 0xEE:
			c.pc = c.stack[c.sp]
			c.sp--
		}
	case 0x1:
		c.pc = addr
	case 0x2:
		c.sp++
		c.stack[c.sp] = c.pc
		c.pc = addr
	case 0x3:
		if c.v[x] == kk {
			c.pc += 2
		}
	case 0x4:
		if c.v[x] != kk {
			c.pc += 2
		}
	case 0x5:
		if c.v[x] == c.v[y] {
			c.pc += 2
		}
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
		var erased bool

		var i byte
		for ; i < n; i++ {
			loc_y := c.v[y] + i

			sprite := c.mem[c.i+uint16(i)]
			var oldSprite byte

			var j byte

			// Mush together display into single byte for xoring
			for ; j < 8; j++ {
				loc_x := c.v[x] + j
				oldSprite = (oldSprite | c.display[loc_y][loc_x]) << 1
			}

			sprite = sprite ^ oldSprite

			// break sprite back up into separate display bytes
			// we use j != 255 because we are dealing with a uint
			// and uints wrap around back to the top when they go below zero
			// so j >= 0 would always hold true
			for j = 7; j != 255; j-- {
				loc_x := c.v[x] + j
				tmp := c.display[loc_y][loc_x]

				c.display[loc_y][loc_x] = sprite & 0x1
				sprite = sprite >> 1

				if !erased && tmp == 0x1 && c.display[loc_y][loc_x] == 0x0 {
					c.v[0xF] = 1
					erased = true
				}
			}
		}

		if !erased {
			c.v[0xF] = 0
		}
	case 0xE:
		switch op & 0xFF {
		case 0x9E:
			if c.ckeys[c.v[x]].pressed {
				c.pc += 2
			}
		case 0xA1:
			if !c.ckeys[c.v[x]].pressed {
				c.pc += 2
			}
		}
	case 0xF:
		switch op & 0xFF {
		case 0x07:
			c.v[x] = c.dt
		case 0x0A:
			// TODO: Implement waitForInput()
			c.v[x] = c.waitForInput()
		case 0x15:
			c.dt = c.v[x]
		case 0x18:
			c.st = c.v[x]
		case 0x1E:
			c.i += uint16(c.v[x])
		case 0x29:
			// TODO
			// This one might need revisiting. Supposed to set i to location of hexadecimal font
			c.i = uint16(c.v[x*5])
		case 0x33:
			bcd := uint32(c.v[x])

			// double dabble algorithm for binary to bcd
			// https://en.wikipedia.org/wiki/Double_dabble
			// we can hardcode our limit to 8 since chip8 registers are 8 bits in length
			for i := 0; i < 8; i++ {
				// Check if hundreds column is greater than 4. If so, add 3 to hundreds column
				if ((bcd & 0xF0000) >> 16) > 4 {
					bcd = (((bcd >> 16) + 3) << 16) | (bcd & 0xFFFF)
				}

				// Check if tens column is greater than 4. If so, add 3 to tens column
				if ((bcd & 0xF000) >> 12) > 4 {
					bcd = (((bcd >> 12) + 3) << 12) | (bcd & 0xFFF)
				}

				// Check if ones column is greater than 4. If so, add 3 to ones column
				if ((bcd & 0xF00) >> 8) > 4 {
					bcd = (((bcd >> 8) + 3) << 8) | (bcd & 0xFF)
				}

				bcd = bcd << 1
			}

			c.mem[c.i] = byte((bcd & 0xF0000) >> 16)
			c.mem[c.i+1] = byte((bcd & 0xF000) >> 12)
			c.mem[c.i+2] = byte((bcd & 0xF00) >> 8)
		case 0x55:
			var i uint16
			for i = 0x0; i <= uint16(x); i++ {
				c.mem[c.i+i] = c.v[i]
			}
		case 0x65:
			var i uint16
			for i = 0x0; i <= uint16(x); i++ {
				c.v[i] = c.mem[c.i+i]
			}
		}
	default:
		fmt.Printf("UNKNOWN OPCODE 0x%X\n", op)
	}
}

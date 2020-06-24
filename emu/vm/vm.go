package vm

import (
	"errors"
	"os"
)

// Chip8 represents the interpreted that will load and run Chip8 programs
type Chip8 struct {
	romFile string

	// Represents machine RAM. Chip8 can access 4096MB (0xFFF) of RAM
	// However the interpreter itself takes up the first 512mb on physical systems
	// therefore your typical program starts at pox 512 (0x200)
	mem [4096]byte

	// Stack pointer; 8 bit pointer used to point to current location in stack
	sp byte

	// The stack has 16 "levels" of nesting each one being a 16 bit value that contains a memory address
	// used for handling subroutines and other operations which require jumping between program memory
	stack [16]uint16

	// Program Counter
	// PC is used to track the next operation in memory to execute
	pc uint16

	// V0-VF registers
	// These are general purpose registers used by Chip8 programs
	// VF i(the carry flag) is generally not directly modified by programs and is normally read for collision detection
	v [16]byte

	// The I register is a 16bit register normally used to store memory addresses
	i uint16

	// Delay Timer & Sound Timer
	// When these registers are non zero; they are automaticalled decremented at a rate of 60hz (1/60th of a second)
	dt byte
	st byte

	// Display is a monochrome display with a size of 64x32
	// Since it is a monochrome display; a pixel can either be on or 1
	// We have no way of working directly with single bits; so we have 2 options for storing pixel info
	// Storing every pixel as a byte which makes the required memory 64x32 bytes
	// OR
	// storing 8 pixels within a single byte; thus making the size of the display memory 64x32 / 8 bytes
	// For now we are going to give each pixel its own designated byte
	// However this might change once I have looked more into how chip8 handles drawing to the screen
	// Since all drawing is done via sprites, and sprites are 8x15, it might make more sense to treat bits as chunks of 8 wide (so 4 chunks makes would be the entire display width
	display [64 * 32]byte
}

// Load romFile into interpreter
func (c *Chip8) Load(romFile string) error {
	err := c.Reset()
	if err != nil {
		return err
	}

	f, err := os.Open(romFile)
	if err != nil {
		return err
	}

	// f.Read requires a byte slice, however since we know the exact chip8 hardware limitations
	// we are using an array directly
	// pass in c.mem[:] to force pass as slice
	_, err = f.Read(c.mem[:])
	if err != nil {
		return err
	}
	c.romFile = romFile
	return nil
}

// Reset will preps chip for loading another rom
func (c *Chip8) Reset() error {
	// no need to clear memory, registers, or stack because a chip8 program should set anything up that it needs prior to reading from these
	// we just need to recent all of our data pointers and the sound timers
	c.romFile = ""
	c.pc = 0x200
	c.sp = 0
	c.dt = 0
	c.st = 0

	return nil
}

// Start interpreter execution using loaded data
func (c *Chip8) Start() error {
	if c.romFile == "" {
		return errors.New("No ROM loaded")
	}
	for {
		HandleOp(c, c.mem[c.pc:c.pc+2])
	}
	return nil
}

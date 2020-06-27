package vm

import (
	"errors"
	"fmt"
	"os"
	"time"

	"github.com/veandco/go-sdl2/sdl"
)

var hexChars = []byte{
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
	0xF0, 0x80, 0xF0, 0x80, 0x80, // F
}

type ckey struct {
	key     sdl.Scancode
	pressed bool
}

type pkey struct {
	key     byte
	pressed bool
}

// Chip8 represents the interpreted that will load and run Chip8 programs
type Chip8 struct {
	initialized bool
	romFile     string

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
	// Since all drawing is done via sprites, and sprites are 8x15, it might make more sense to treat pixels as chunks of 8 wide (so 4 chunks makes would be the entire display width
	display [32][64]byte

	// ckeys is a mapping of chip8 keys to physical keys
	ckeys map[byte]ckey

	// pkeys is a mapping of physical keys to chip8 keys
	pkeys map[sdl.Scancode]pkey

	// window and surface represent the sdl components for rendering
	window  *sdl.Window
	surface *sdl.Surface

	// scale is used to determine the size of the display and pixels
	scale int32
}

func (c *Chip8) waitForInput() byte {
	var ckey byte
	for event := sdl.WaitEvent(); event != nil; event = sdl.WaitEvent() {
		switch e := event.(type) {
		case *sdl.KeyboardEvent:
			if key, ok := c.pkeys[e.Keysym.Scancode]; ok && e.Type == sdl.KEYDOWN {
				ckey = key.key
			}
		}
	}
	return ckey
}

// this isn't great tbh. i wish i could just draw directly to the screen
// and scale pixel sizes as needed. such is life
var i bool
var rects [][]sdl.Rect

func (c *Chip8) draw() {
	if !i {
		for y := 0; y < 32; y++ {
			rects = append(rects, make([]sdl.Rect, 64))
			for x := 0; x < 64; x++ {
				y := int32(y)
				x := int32(x)
				rects[y][x] = sdl.Rect{c.scale * x, c.scale * y, c.scale, c.scale}
			}
		}
		i = true
	}

	c.surface.FillRect(nil, 0)
	for y, scanLine := range c.display {
		for x, pixel := range scanLine {
			y := int32(y)
			x := int32(x)
			if pixel == 0x1 {
				c.surface.FillRect(&rects[y][x], sdl.MapRGB(c.surface.Format, 0, 255, 255))
			}
		}
	}

	c.window.UpdateSurface()
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
	_, err = f.Read(c.mem[0x200:])
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

	for i := 0; i < len(c.display); i++ {
		for j := 0; j < len(c.display[i]); j++ {
			c.display[i][j] = 0x0
		}
	}

	if !c.initialized {
		// TODO: Load fonts

		for i := 0; i < len(hexChars); i++ {
			c.mem[i] = hexChars[i]
		}

		// Init keyboard
		c.ckeys = map[byte]ckey{
			0x1: {sdl.SCANCODE_1, false},
			0x2: {sdl.SCANCODE_2, false},
			0x3: {sdl.SCANCODE_3, false},
			0xC: {sdl.SCANCODE_4, false},
			0x4: {sdl.SCANCODE_Q, false},
			0x5: {sdl.SCANCODE_W, false},
			0x6: {sdl.SCANCODE_E, false},
			0xD: {sdl.SCANCODE_R, false},
			0x7: {sdl.SCANCODE_A, false},
			0x8: {sdl.SCANCODE_S, false},
			0x9: {sdl.SCANCODE_D, false},
			0xE: {sdl.SCANCODE_F, false},
			0xA: {sdl.SCANCODE_Z, false},
			0x0: {sdl.SCANCODE_X, false},
			0xB: {sdl.SCANCODE_C, false},
			0xF: {sdl.SCANCODE_V, false},
		}
		c.pkeys = map[sdl.Scancode]pkey{
			sdl.SCANCODE_1: {0x1, false},
			sdl.SCANCODE_2: {0x2, false},
			sdl.SCANCODE_3: {0x3, false},
			sdl.SCANCODE_4: {0xC, false},
			sdl.SCANCODE_Q: {0x4, false},
			sdl.SCANCODE_W: {0x5, false},
			sdl.SCANCODE_E: {0x6, false},
			sdl.SCANCODE_R: {0xD, false},
			sdl.SCANCODE_A: {0x7, false},
			sdl.SCANCODE_S: {0x8, false},
			sdl.SCANCODE_D: {0x9, false},
			sdl.SCANCODE_F: {0xE, false},
			sdl.SCANCODE_Z: {0xA, false},
			sdl.SCANCODE_X: {0x0, false},
			sdl.SCANCODE_C: {0xB, false},
			sdl.SCANCODE_V: {0xF, false},
		}
		c.initialized = true
	}

	return nil
}

// Start interpreter execution using loaded data
func (c *Chip8) Start(scale int32) error {
	c.scale = scale
	if c.romFile == "" {
		return errors.New("No ROM loaded")
	}

	err := sdl.Init(sdl.INIT_VIDEO & sdl.INIT_AUDIO & sdl.INIT_EVENTS)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Failed to initialize sdl: %s\n", err)
		os.Exit(1)
	}

	defer sdl.Quit()
	// try to create a window
	c.window, err = sdl.CreateWindow("Chip8", sdl.WINDOWPOS_UNDEFINED, sdl.WINDOWPOS_UNDEFINED, 64*scale, 32*scale, sdl.WINDOW_SHOWN)
	if err != nil {
		fmt.Fprint(os.Stderr, "Failed to create window: %s\n", err)
		os.Exit(2)
	}
	defer c.window.Destroy()

	c.surface, err = c.window.GetSurface()
	if err != nil {
		fmt.Fprint(os.Stderr, "Failed to get surface: %s\n", err)
	}

	// Start time for figuring out delta time
	st := time.Now()

	// Cumulative time to track when its been long enough to decrease timers
	cu := 0.0

	// rate at which to decrease timers
	rate := 1.0 / 60.0

	running := true
	audio_off := true
	for running {
		for event := sdl.PollEvent(); event != nil; event = sdl.PollEvent() {
			switch event.(type) {
			case *sdl.QuitEvent:
				running = false
				break
			}
		}
		currTime := time.Now()
		dt := currTime.Sub(st)
		st = currTime

		// TODO: figure out why this hack is needed to correct timers
		sec := dt.Seconds() / 4

		// hopefully this will semi accurately drop the timers at a rate of 1/60hz
		if cu >= rate {
			if c.dt > 0 {
				c.dt--
			}
			if c.st > 0 {
				if audio_off {
					audio_off = false
					// sdl.PauseAudio(audio_off)
				}
				c.st--
			} else {
				if !audio_off {
					audio_off = true
					// sdl.PauseAudio(audio_off)
				}
			}
			cu = 0
		} else {
			cu += sec
		}
		HandleOp(c, c.mem[c.pc:c.pc+2])
		c.draw()
	}
	return nil
}

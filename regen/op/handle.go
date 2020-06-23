package op

import (
	"fmt"
)

func HandleOp(buf []byte, pc int) int {

	// Opbytes is the default amount that will be added to pc assuming we are not jumping or skipping instructions
	// if we are skipping instructions then we will set pc accordingly
	opbytes := 2

	// Chip 8 instruction are technically 2 bytes long however they are read in via 1 byte increments
	// Therefore we need to combine them into a single 16 bit integer
	var op uint16
	op = uint16(buf[1])<<8 | uint16(buf[0])

	// addr := op & 0xFFF
	// n := op & 0xF
	// x := op & 0x0F00
	// y := op & 0x00F0
	// kk := op & 0x00FF

	switch op >> 12 {
	case 0x0:
		switch op & 0xFF {
		case 0xE0:
			fmt.Printf("CLS\n")
		case 0xEE:
			fmt.Printf("RET\n")
		}
	case 0x1:
		fmt.Println("JMP addr")
	case 0x2:
		fmt.Println("CALL addr")
	case 0x3:
		fmt.Println("SE Vx, kk")
	case 0x4:
		fmt.Println("SNE Vx, kk")
	case 0x5:
		fmt.Println("SE Vx, Vy")
	case 0x6:
		fmt.Println("LD Vx, kk")
	case 0x7:
		fmt.Println("ADD Vx, kk")
	case 0x8:
		switch op & 0xF {
		case 0x0:
			fmt.Println("LD Vx, Vy")
		case 0x1:
			fmt.Println("OR Vx, Vy")
		case 0x2:
			fmt.Println("AND Vx, Vy")
		case 0x3:
			fmt.Println("XOR Vx, Vy")
		case 0x4:
			fmt.Println("ADD Vx, Vy")
		case 0x5:
			fmt.Println("SUB Vx, Vy")
		case 0x6:
			fmt.Println("SHR Vx, {, Vy}")
		case 0x7:
			fmt.Println("SUBN Vx, Vy")
		case 0xE:
			fmt.Println("SHL Vx {, Vy}")
		}
	case 0x9:
		fmt.Println("SNE Vx, Vy")
	case 0xA:
		fmt.Println("LD I, addr")
	case 0xB:
		fmt.Println("JMP V0, addr")
	case 0xC:
		fmt.Println("RND Vx, kk")
	case 0xD:
		fmt.Println("DRW Vx, Vy, n")
	case 0xE:
		switch op & 0xFF {
		case 0x9E:
			fmt.Println("SKP Vx")
		case 0xA1:
			fmt.Println("SKNP Vx")
		}
	case 0xF:
		switch op & 0xFF {
		case 0x07:
			fmt.Println("LD Vx, DT")
		case 0x0A:
			fmt.Println("LD Vx, K")
		case 0x15:
			fmt.Println("LD DT, Vx")
		case 0x18:
			fmt.Println("LD ST, Vx")
		case 0x1E:
			fmt.Println("ADD I, Vx")
		case 0x29:
			fmt.Println("LD F, Vx")
		case 0x33:
			fmt.Println("LD B, Vx")
		case 0x55:
			fmt.Println("LD [I], Vx")
		case 0x65:
			fmt.Println("LD Vx, [I]")
		}
	default:
		fmt.Printf("UNKNOWN OPCODE 0x%X\n", op)
	}
	return pc + opbytes
}

package cmd

import (
	"io/ioutil"

	"github.com/spf13/cobra"
	"github.com/yetibob/chip8/emu/vm"
)

var (
	// Used for flags.
	rootCmd = &cobra.Command{
		Use:   "regen",
		Short: "regen runs through a chip8 program and spits out assembly ",
		Run: func(cmd *cobra.Command, args []string) {
			romFile, err := cmd.PersistentFlags().GetString("rom")
			panicErr(err)

			var emu Chip8
			err := emu.Load(romFile)
			panicErr(err)
			err = emu.Start()
			panicErr(err)
		},
	}
)

func panicErr(err error) {
	if err != nil {
		panic(err)
	}
}

// Execute t
func Execute() error {
	return rootCmd.Execute()
}

func init() {
	cobra.OnInitialize(initConfig)
	rootCmd.PersistentFlags().StringP("rom", "r", "", "rom file to regen")
	rootCmd.MarkPersistentFlagRequired("rom")
}

func initConfig() {
	return
}

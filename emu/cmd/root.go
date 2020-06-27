package cmd

import (
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

			var vm vm.Chip8
			err = vm.Load(romFile)
			panicErr(err)

			err = vm.Start(10)
			panicErr(err)
		},
	}
)

func panicErr(err error) {
	if err != nil {
		panic(err)
	}
}

// Execute
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

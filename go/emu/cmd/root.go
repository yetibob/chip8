package cmd

import (
	"github.com/spf13/cobra"
	"github.com/yetibob/chip8/emu/vm"
)

var (
	// Used for flags.
	rootCmd = &cobra.Command{
		Use:   "chip8",
		Short: "runs chip8 programs by emulating chip8 runtime",
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

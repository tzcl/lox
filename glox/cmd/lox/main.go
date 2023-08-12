package main

import (
	"bufio"
	"fmt"
	"os"
)

/* NOTES

What I really want is to do all the dependency injection here and then have
most of the logic running in packages.
*/

func main() {
	args := os.Args[1:]

	if len(args) > 1 {
		fmt.Println("Usage: lox [script]")
		os.Exit(1)
	} else if len(args) == 1 {
		// TODO: How do I want to handle errors?
		runFile(args[0])
	} else {
		runPrompt()
	}
}

// TODO: Move this logic to its own package?
func runFile(path string) error {
	data, err := os.ReadFile(path)
	if err != nil {
		return err
	}

	run(string(data))
	return nil
}

func runPrompt() error {
	reader := bufio.NewScanner(os.Stdin)

	for {
		fmt.Print("> ")
		if ok := reader.Scan(); !ok {
			break
		}
		line := reader.Text()
		run(line)
	}

	return nil
}

func run(src string) {
	fmt.Println(src)
}

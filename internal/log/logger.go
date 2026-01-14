package logger

import "fmt"

type Logger struct {
	level int
}

const (
	BASE   int = 0
	INFO   int = 1
	NOTICE int = 2
	DEBUG  int = 3
)

func (log *Logger) Log(level int, format string, a ...any) {
	if level <= log.level {
		fmt.Printf(format, a...)
	}
}

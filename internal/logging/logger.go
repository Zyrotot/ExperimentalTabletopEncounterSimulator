package logging

import (
	"log"
	"os"
)

type Level int

const (
	DEBUG Level = iota
	INFO
	NOTICE
	WARN
	ERROR
)

type Logger struct {
	level Level
	base  *log.Logger
	name  string
}

func New(name string, level Level) *Logger {
	return &Logger{
		level: level,
		name:  name,
		base:  log.New(os.Stdout, "", log.LstdFlags),
	}
}

func (l *Logger) logf(lv Level, prefix, format string, args ...any) {
	if lv < l.level {
		return
	}
	l.base.Printf("[%s] [%s] "+format, append([]any{l.name, prefix}, args...)...)
}

func (l *Logger) Debugf(format string, args ...any) {
	l.logf(DEBUG, "DEBUG", format, args...)
}

func (l *Logger) Infof(format string, args ...any) {
	l.logf(INFO, "INFO", format, args...)
}

func (l *Logger) Noticef(format string, args ...any) {
	l.logf(NOTICE, "NOTICE", format, args...)
}

func (l *Logger) Warnf(format string, args ...any) {
	l.logf(WARN, "WARN", format, args...)
}

func (l *Logger) Errorf(format string, args ...any) {
	l.logf(ERROR, "ERROR", format, args...)
}

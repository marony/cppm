#pragma once

extern char *user_input;

extern void error(char *fmt, ...);
extern void error_at(char *loc, char *msg);
extern void debug(char *fmt, ...);

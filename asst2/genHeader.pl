#!/usr/bin/perl
#
# Extract lexemes from a lex file. Very fragile.
#
# Anything that looks like:
#  "\"regexp\" <junk> { return(LEXEME); <junk> }"
# is treated as defining a lexeme.
#

use strict;

main:
{
  my $filename = shift(@ARGV);

  if(!(-e $filename)) {
    print "Usage $0 filename\n";
    exit(1);
  } else {
    genheader($filename);
  }
}

sub genheader {
  my $filename = shift(@_);
  my $ofilename = $filename;

  $ofilename =~ /^(\w+)\./;
  $ofilename = $1;

  my $ufilename = uc($ofilename);
  my %lexemes;

  open(FINPUT, "<$filename") 		or die "can't open $filename: $!";
  open(FOUTPUT, ">$ofilename" . ".h")	or die "can't open $filename: $!";

print FOUTPUT <<EOM;
/*
 * Lexical Analyser header file.
 *
 * Automatically generated by genHeader.pl from $filename, DO NOT HAND EDIT.
 */

#ifndef _${ufilename}_H_
#define _${ufilename}_H_

#include <stdio.h>
#include "util.h"

/* Token types. */
typedef enum {
	LEXER_EOF
EOM

  while(<FINPUT>) {
    if(/^<INITIAL>("(.*)")?.*return\((.*)\);\s*}/) {
      if(defined $2) {
	$lexemes{$3} = $2;
      } else {
	$lexemes{$3} = $3;
      }
      print FOUTPUT ",\t$3\n";
    }
  }

print FOUTPUT <<EOM;
} token_t;

/* Lexical semantic data */
typedef struct yystype {
	token_t lexeme;
	char charval;
	double numval;
	char *strval;
        struct yystype *next; /* internal use only, don't touch. */
} YYSTYPE;

/* The following are defined in the lexer module. */

/* The token currently under consideration. */
extern YYSTYPE yylval;

/* The file handle the lexer is reading from. */
extern FILE *yyin;

/* The current line number. */
extern int line_num;

/* Constant for "no position" things. */
#define noPos (-1)

/*
 * Toggles whether lexemes are echoed to stdout as they are processed.
 */
void toggle_output_lexemes(void);

/*
 * Updates the shared variables to reflect the next token on the input
 * stream.
 */
void lex(void);

/*
 * Returns the nth token ahead of the current one, but does not
 * consume them. In other words, yylval and the lexer state are unchanged.
 */
token_t lex_lookahead(int n);

/*
 * Expect the given lexeme on the input stream; if it is, lex() and return,
 * otherwise parse_error(msg). NOTE: if there is any semantic information
 * attached to the expected lexeme, this function should not be used. (the
 * following token may overwrite the pertinent field of yylval.)
 *
 * Same calling convention as printf(3).
 */
void lex_expect(token_t lexeme, char *fmt, ...);

/*
 * Convert a lexeme number into a comprehendable string.
 */
char *lex_toString(token_t lexeme);

/*
 * Lexically analyse an in-memory string.
 */
void lex_string_start(const char *str);
void lex_string_end(void);

#endif /* _${ufilename}_H_ */

EOM

  close(FINPUT);
  close(FOUTPUT);

  open(FOUTPUT, ">>$ofilename" . ".c")	or die "can't open $filename: $!";
  seek(FOUTPUT, 0, 2);

print FOUTPUT <<EOM;

char *lex_toString(token_t tokentype)
{
    switch(tokentype) {
    case LEXER_EOF:
        return "<< EOF >>";
EOM

  # Generate the lex_toString() function.
  foreach my $key (keys %lexemes) {
print FOUTPUT <<EOM;
    case $key:
        return "'$lexemes{$key}' ($key)";
EOM
  }
print FOUTPUT <<EOM;
    default:
        fprintf(stdout, "Lexer error: %d: unknown lexeme number: %d.\\n",
	        line_num,
	        tokentype);
        exit(1);
        return NULL; /* shut up, gcc! */
    }
}
EOM
}
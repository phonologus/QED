" Fix asciidoctor manpage output.
ba
0r
1 "
/^\.SH "NAME"/;+1 c
.SH "PROGRAMMING IN QED: A TUTORIAL"
by Robert Pike
.
/^\.SH "SYNOPSIS"/,/^\.SH "INTRO/-1 d
" Put back tabs which are not indents
,s/\([^ ]\)        /\1	/g
" \f(CR\fB is ridiculous. Make it sane.
,s/\cf(CR\cfB/\cfB/g
,s/\cfP\cfP/\cfP/g
,s/\cf(CR/\cfI/g
" Sigh...
1 "
/random words/+3;+10s/	/        /g
w
q

TXT=txt
HTML=html
PDF=pdf
SECTION=7

.PHONY: all tutorial manpage clean release

RELDIR=..

QED=qed - -e

AD2HTML=asciidoctor
AD2MAN=asciidoctor -b manpage
AD2PDF=asciidoctor-pdf

TUTORIAL=qed-tutorial
MANPAGE=qed.1
TFIX=fix.q

TCOMMON= \
  $(TUTORIAL)-body.$(TXT)

TOUT= \
  $(TUTORIAL).$(SECTION) \
  $(TUTORIAL).$(HTML) \
  $(TUTORIAL).$(PDF)

TIN= \
  $(TUTORIAL)-article.$(TXT) \
  $(TUTORIAL)-manpage.$(TXT) \
  $(TUTORIAL).$(TXT)

all: tutorial manpage

manpage: $(MANPAGE)
	man -t ./$(MANPAGE) | ps2pdf - > $<.$(PDF)

release: all
	mv $(TOUT) $(RELDIR)/
	cp $(MANPAGE) $(RELDIR)/
	mv $(MANPAGE).$(PDF) $(RELDIR)/

tutorial: $(TOUT)

clean:
	rm -f $(TOUT)

$(TUTORIAL).$(HTML): $(TUTORIAL).$(TXT) $(TCOMMON)
	$(AD2HTML) -a as_article -o $@ $<

$(TUTORIAL).$(PDF): $(TUTORIAL).$(TXT) $(TCOMMON)
	$(AD2PDF) -a as_article -o $@ $<

$(TUTORIAL).$(SECTION): $(TUTORIAL).$(TXT) $(TCOMMON) $(TFIX)
	$(AD2MAN) -a as_manpage -o $@ $< 
	$(QED) -x $(TFIX) $@ > /dev/null 2>&1


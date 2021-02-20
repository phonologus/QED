TXT=txt
HTML=html
PDF=pdf
SECTION=7

.PHONY: all tutorial manpage clean release

RELDIR=..

AD2HTML=asciidoctor
AD2MAN=asciidoctor -b manpage
AD2PDF=asciidoctor-pdf

TUTORIAL=qed-tutorial

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

all: tutorial

release: all
	mv $(TOUT) $(RELDIR)/

tutorial: $(TOUT)

clean:
	rm -f $(TOUT)

$(TUTORIAL).$(HTML): $(TUTORIAL).$(TXT) $(TCOMMON)
	$(AD2HTML) -a as_article -o $@ $<

$(TUTORIAL).$(PDF): $(TUTORIAL).$(TXT) $(TCOMMON)
	$(AD2PDF) -a as_article -o $@ $<

$(TUTORIAL).$(SECTION): $(TUTORIAL).$(TXT) $(TCOMMON)
	$(AD2MAN) -a as_manpage -o $@ $< 


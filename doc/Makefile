TXT=txt
HTML=html
PDF=pdf
MSECTION=1
TSECTION=7

.SUFFIXES: .$(TXT) .$(HTML) .$(TSECTION) .$(MSECTION) .$(PDF)

.PHONY: all tutorial manpage clean

AD2HTML=asciidoctor
AD2MAN=asciidoctor -b manpage
AD2PDF=asciidoctor-pdf

MANPAGE=qed-manpage
TUTORIAL=qed-tutorial

TCOMMON= \
  $(TUTORIAL)-attr.$(TXT) \
  $(TUTORIAL)-main.$(TXT)

TOUT= \
  $(TUTORIAL).$(TSECTION) \
  $(TUTORIAL).$(HTML) \
  $(TUTORIAL).$(PDF)

TIN= \
  $(TUTORIAL)-$(TSECTION).$(TXT) \
  $(TUTORIAL).$(TXT)

.$(TXT).$(HTML):
	$(AD2HTML) -o $*.$(HTML) $<

.$(TXT).$(PDF):
	$(AD2PDF) -o $*.$(PDF) $<

.$(TXT).$(TSECTION):
	$(AD2MAN) -o $*.$(TSECTION) $< 

.$(TXT).$(MSECTION):
	$(AD2MAN) -o $(MSECTION).$* $< 

all: tutorial

tutorial: $(TOUT)

clean:
	rm -f $(TOUT)

$(TUTORIAL).$(HTML): $(TUTORIAL).$(TXT) $(TCOMMON)

$(TUTORIAL).$(PDF): $(TUTORIAL).$(TXT) $(TCOMMON)

$(TUTORIAL).$(TSECTION): $(TUTORIAL)-$(TSECTION).$(TSECTION) $(TCOMMON)
	mv $(TUTORIAL)-$(TSECTION).$(TSECTION) $(TUTORIAL).$(TSECTION)


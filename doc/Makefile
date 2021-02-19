TXT=txt
HTML=html
MSECTION=1
TSECTION=7

.SUFFIXES: .$(TXT) .$(HTML) .$(TSECTION) .$(MSECTION)

.PHONY: all tutorial manpage clean

AD2HTML=asciidoctor
AD2MAN=asciidoctor -b manpage

MANPAGE=qed-manpage
TUTORIAL=qed-tutorial

TCOMMON= \
  $(TUTORIAL)-attr.$(TXT) \
  $(TUTORIAL)-main.$(TXT)

TOUT= \
  $(TUTORIAL).$(TSECTION) \
  $(TUTORIAL).$(HTML)

TIN= \
  $(TUTORIAL)-$(TSECTION).$(TXT) \
  $(TUTORIAL).$(TXT)

.$(TXT).$(HTML):
	$(AD2HTML) -o $*.$(HTML) $<

.$(TXT).$(TSECTION):
	$(AD2MAN) -o $*.$(TSECTION) $< 

.$(TXT).$(MSECTION):
	$(AD2MAN) -o $(MSECTION).$* $< 

all: tutorial

tutorial: $(TOUT)

clean:
	rm -f $(TOUT)

$(TUTORIAL).$(HTML): $(TUTORIAL).$(TXT) $(TCOMMON)

$(TUTORIAL).$(TSECTION): $(TUTORIAL)-$(TSECTION).$(TSECTION) $(TCOMMON)
	mv $(TUTORIAL)-$(TSECTION).$(TSECTION) $(TUTORIAL).$(TSECTION)


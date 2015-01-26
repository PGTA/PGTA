# A template Makefile. Clone and own.

# see these links:
# http://geosoft.no/development/javamake.html
# http://www.cs.swarthmore.edu/~newhall/unixhelp/javamakefiles.html

.SUFFIXES: .pdf .tex .dot .bib
.PHONY : clean 

TEX = $(shell find . -type f -name '*.tex' | sort)
DOT = $(shell find . -type f -name '*.dot' | sort)
BIB = $(shell find . -type f -name '*.bib' | sort)
#INC = $(shell find -name '*.tex' | xargs grep 'includegraphics.*{.*}' | sed -e 's/.*{//' -e 's/}.*//')

main.pdf: $(TEX) $(DOT) $(BIB)
	pdflatex -shell-escape main.tex
	bibtex -min-crossrefs=9000 main
	pdflatex -shell-escape main.tex

clean:
	rm -rf *.aux *.bak *.bbl *.blg *.brf *.drv *.dvi *.glo *.gls *.idx *.ilg *.ind *.lof *.lot *.log *.nav *.nlo *.out *.snm *.tdo *.toc *.vrb
	rm -f main.pdf

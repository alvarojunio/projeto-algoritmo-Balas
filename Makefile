all:
	latex balas.tex
	bibtex balas.aux
	bibtex balas.aux
	latex balas.tex
	dvipdf balas.dvi

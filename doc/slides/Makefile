TEXMF=/usr/share/texlive/texmf-dist
PDFREADER=evince

THEME=beamerthemessrg.sty
INNER=beamerinnerthemessrg.sty
COLOR=beamercolorthemessrg.sty
TITLE_FRAME_BG=include/ssrg-title-frame-bg.pdf
NORMAL_FRAME_BG=include/ssrg-normal-frame-bg.pdf

SAMPLE_DOC=slimguard

all: compile_sample
	make clean
install:
	cp -f $(THEME) $(TEXMF)/tex/latex/beamer/themes/theme/
	cp -f $(COLOR) $(TEXMF)/tex/latex/beamer/themes/color/
	cp -f $(INNER) $(TEXMF)/tex/latex/beamer/themes/inner/
	mkdir -p $(TEXMF)/tex/generic/images
	cp -f $(TITLE_FRAME_BG) $(TEXMF)/tex/generic/images/
	cp -f $(NORMAL_FRAME_BG) $(TEXMF)/tex/generic/images/
	mktexlsr
	texhash

compile_sample:
		pdflatex --draftmode $(SAMPLE_DOC)
		pdflatex $(SAMPLE_DOC)

show: compile_sample
		$(PDFREADER) $(SAMPLE_DOC).pdf

clean:
		rm -rf *.log *.aux *.out *.bbl *.blg *.nav *.snm *.toc *.vrb

superclean: clean
		rm -rf $(SAMPLE_DOC).pdf

HTMLFLAGS = --book --duplex --no-numbered --format pdf14 --title --size letter

%.pdf: %.html
	-htmldoc $(HTMLFLAGS) --outfile $@ $<

all: electronAnalyserDoc.pdf doxygen iocBootDocs

electronAnalyserDoc.pdf:
	

doxygen:
	doxygen doxyfile

iocBootDocs:
	txt2html --outfile electronAnalyser_st_cmd.html -pb 0         ../iocBoot/iocelectronAnalyser/stelectronAnalyser.src

install:all
#	cp *.html ../doc/
#	cp *.png ../doc/
#	cp *.jpg ../doc/
#	cp *.pdf ../doc/
#	cp -rp html ../doc/

clean:
	rm -rf html
	rm electronAnalyser_st_cmd.html


realclean: clean

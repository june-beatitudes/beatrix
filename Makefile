cfiles := $(wildcard kernel/src/*.c)
hfiles := $(wildcard kernel/include/*.h)
mdfiles := $(wildcard *.md) $(wildcard kernel/*.md)

docs: $(cfiles) $(hfiles) $(mdfiles) Doxyfile
	rm -rf docs/*
	doxygen Doxyfile
	cd extern/doxybook; \
	uv run doxybook -i ../../docs/xml -o ../../docs/markdown.md --template-lang c --doxygen-extra-args="../../Doxyfile"
	head -n 1 docs/markdown.md > docs/fixed-markdown.md
	tail +3 docs/markdown.md | sed "s/^## File/# File/; s/^#/##/" >> docs/fixed-markdown.md
	pandoc -N -s -t chunkedhtml -o docs/html.zip docs/fixed-markdown.md --shift-heading-level-by=-1 --toc=true
	unzip docs/html.zip -d docs/html
	rm -rf docs/xml docs/markdown.md docs/fixed-markdown.md docs/html.zip

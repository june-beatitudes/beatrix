cfiles := $(wildcard kernel/src/*.c)
hfiles := $(wildcard kernel/include/*.h)
mdfiles := $(wildcard *.md) $(wildcard kernel/*.md)

docs: $(cfiles) $(hfiles) $(mdfiles) Doxyfile
	rm -rf docs/*
	doxygen Doxyfile
	cd extern/doxybook; \
	uv run doxybook -i ../../docs/xml -o ../../docs/markdown/ --template-lang c --doxygen-extra-args="../../Doxyfile" -t mkdocs


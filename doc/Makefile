
export ROME_BASE := $(CURDIR)/..

all:: doxygen
	# Nothing
	@echo Done

doxygen:
	@echo Making Doxygen...
	doxygen > $(CURDIR)/doxygen.log 2>&1

clean:
	rm -rf doxygen.log html doxygen_mavis_warnings.out

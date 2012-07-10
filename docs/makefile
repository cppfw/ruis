default: doc

doc: html_doc_files

html_doc_files:
	@echo Building docs...
	@doxygen doxygen.cfg

clean:
	@rm -rf html_doc_files

include ../targets.mk

libMyTerm.a: myTerm.c
	@gcc -c myTerm.c
	@ar ruv libMyTerm.a myTerm.o
	@ranlib libMyTerm.a
	@rm -fR myTerm.o
	@cp -R ./libMyTerm.a ./test/

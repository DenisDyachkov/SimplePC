libMyBigChars.a: myBigChars.c
	@gcc -c myBigChars.c
	@ar ruv libMyBigChars.a myBigChars.o
	@ranlib libMyBigChars.a
	@rm -fR myBigChars.o
	@cp -R ./libMyBigChars.a ./test/

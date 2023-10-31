server:main.o
	g++ $^ -o $@ -lwfrest -lworkflow
main.o:main.cc
	g++ -c $^ -o $@
clean:
	$(RM) server main.o
MAIN = test.o Buffer.o ./Log/AsyncLog.o ./Log/Log.o
main: ${MAIN}
	g++ -o main ${MAIN} -g -Wall

clean:
	rm -f main ${MAIN}
MAIN = test.o Buffer.o ./Log/AsyncLog.o ./Log/Log.o ./Base/Timeoperator.o EventLoop.o Channel.o Epoller.o Timer.o TimerQueue.o Sockets.o EventLoopThread.o Socket.o Acceptor.o InetAddr.o BlockingQueue.o ThreadPool.o TcpConnection.o 
main: ${MAIN}
	g++ -o main ${MAIN} -g -Wall
clean:
	rm -f main ${MAIN}
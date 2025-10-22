import os, select

fd = os.open("/dev/simtemp", os.O_RDONLY | os.O_NONBLOCK)
ep = select.epoll()
ep.register(fd, select.EPOLLIN)

while True:
    events = ep.poll(1)
    for fileno, event in events:
        if event & select.EPOLLIN:
            data = os.read(fileno, 16)
            print("New sample:", data)

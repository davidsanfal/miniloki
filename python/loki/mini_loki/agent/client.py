import socket
import pygame
import json


def client(ip="172.16.17.75", port=5044):
    pygame.init()
    pygame.joystick.init()
    joystick = pygame.joystick.Joystick(0)
    joystick.init()
    s = socket.socket()
    s.connect((ip, port))
    s.send(json.dumps({'type': 'client',
                       'ip': s.getsockname()[0]}))
    msg = s.recv(1024)
    info = json.loads(msg)
    s.close()
    try:
        s = socket.socket()
        if info['loki_0']:
            print info['loki_0']
            s.connect((info['loki_0'], 80))
            try:
                while True:
                    clock = pygame.time.Clock()
                    for _ in pygame.event.get():
                        pass
                    clock.tick(50)
                    s.send("%.2f,%.2f,%.2f,\n" % (float(joystick.get_axis(1)),
                                                  float(joystick.get_axis(0)),
                                                  float(joystick.get_axis(2))))
            except socket.error:
                pass
        else:
            print "\n\nNO LOKI CONNECTED\n\n"
    except KeyboardInterrupt:
        pass
    s.close()
    pygame.quit()

if __name__ == '__main__':
    client()

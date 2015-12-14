import socket
import pygame
import json

OFFSET_SPEED = 0.68
OFFSET_SWARM = 0.0


def w_speed(w, x, y):
    w = float(w) if x > 0 else float(w) * 0.6
    return w - y*OFFSET_SPEED


def w_swarm(w, y):
    return float(w) - y*OFFSET_SWARM


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
                    x = float(joystick.get_axis(1))
                    y = float(joystick.get_axis(0))
                    w = w_speed(joystick.get_axis(2), x, y)
                    s.send("%.2f,%.2f,%.2f,\n" % (x ** 3, y ** 3, w ** 3))
            except socket.error as e:
                print e.message
        else:
            print "\n\nNO LOKI CONNECTED\n\n"
    except KeyboardInterrupt:
        pass
    s.close()
    pygame.quit()

if __name__ == '__main__':
    client()

import socket
import pygame

BOOST = 1

PRECISION = 0.5


def client(ip="172.16.7.185", port=80):
    pygame.init()
    pygame.joystick.init()
    joystick = pygame.joystick.Joystick(0)
    joystick.init()
    try:
        s = socket.socket()
        s.connect((ip, 80))
        try:
            b = 0
            while True:
                clock = pygame.time.Clock()
                for _ in pygame.event.get():
                    pass
                clock.tick(50)
                boost = 0.7
                if joystick.get_button(4):
                    boost = PRECISION
                elif joystick.get_button(5):
                    boost = BOOST
                x = float(joystick.get_axis(1)) * boost
                y = float(joystick.get_axis(0)) * boost
                w = float(joystick.get_axis(3)) * boost
                s.send("%.2f,%.2f,%.2f,%i,\n" % (x ** 3, y ** 3, w ** 3, b))
        except socket.error as e:
            print e.message
    except KeyboardInterrupt:
        pass
    except socket.error as e:
        print e.message
        print "\n\nNO LOKI CONNECTED\n\n"
    s.close()
    pygame.quit()

if __name__ == '__main__':
    client()

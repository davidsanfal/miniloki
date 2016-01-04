import socket
import pygame
import json
import telepot
import time

BOOST = 1

PRECISION = 0.5


def client(ip="172.16.17.75", port=5044):
    pygame.init()
    pygame.joystick.init()
    joystick = pygame.joystick.Joystick(0)
    joystick.init()
    s = socket.socket()
    s.connect((ip, port))
    s.send(json.dumps({'type': 'client',
                       'ip': s.getsockname()[0]}))
    _msg = s.recv(1024)
    info = json.loads(_msg)
    s.close()

    def handle(msg):
        show_keyboard = {'keyboard': [['Yes', 'No']]}
        bot.sendMessage(msg['from']['id'], 'BIP or not BIP', reply_markup=show_keyboard)
        print msg
        s = socket.socket()
        if info['loki_0']:
            s.connect((info['loki_0'], 80))
            try:
                if msg['text'] == u'Yes':
                    s.send("0,0,0,1,\n")
                if msg['text'] == u'No':
                    s.send("0,0,0,0,\n")
            except:
                pass
            s.close()

    bot = telepot.Bot("TOKEN")
    bot.notifyOnMessage(handle)
    while 1:
        time.sleep(10)

if __name__ == '__main__':
    client()

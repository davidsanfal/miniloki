import socket
import json
from mini_loki import ping


class Overmind(object):

    def __init__(self):
        self.agents = {}
        self.references = {}

    def run(self, ip):
        try:
            server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server.bind((ip, 5044))
            server.settimeout(0.5)
            while True:
                try:
                    server.listen(1)
                    client, _ = server.accept()
                    client.settimeout(2)
                    try:
                        self.register_agent(client)
                    except (socket.timeout, socket.error):
                        client.close()
                except socket.timeout:
                    self.review_agents()
        except KeyboardInterrupt:
            pass
        server.close()

    def register_agent(self, client):
        msg = client.recv(1024)
        info = json.loads(msg)
        getattr(self, 'register_%s' % info['type'])(info, client)

    def review_agents(self):
        for agent in self.agents.keys():
            print agent, ping.do_one(self.agents[agent]['ip'], 1)


class LokiOvermind(Overmind):

    def register_mini_loki(self, info, client):
        chip_id = info['chip_id']
        dns_name = "loki_%s" % len(self.agents)
        if chip_id in self.references.keys():
            dns_name = self.references[chip_id]
        else:
            self.references[chip_id] = dns_name
            self.agents[dns_name] = info
        client.send("%s\n" % dns_name)
        print "mini loki added: ", dns_name
        print self.agents[dns_name]

    def register_client(self, info, client):
        loki_info = json.dumps({'loki_0': None})
        if 'loki_0' in self.agents.keys():
            self.agents['client'] = info
            loki_info = json.dumps({'loki_0': self.agents['loki_0']['ip']})
        print loki_info
        client.send(loki_info)

if __name__ == '__main__':
    overmind = LokiOvermind()
    overmind.run("172.16.17.75")

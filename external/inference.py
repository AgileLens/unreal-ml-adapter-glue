import socket
import json
import time
import random

HOST = '127.0.0.1'
PORT = 5555

def main():
    with socket.create_connection((HOST, PORT)) as sock:
        print('Connected to Unreal ML Adapter')
        while True:
            # Send a dummy actor state
            state = {
                "actor": "DemoPawn",
                "position": {
                    "x": random.uniform(-100, 100),
                    "y": random.uniform(-100, 100),
                    "z": random.uniform(0, 200)
                },
                "variables": {"health": random.randint(0, 100)}
            }
            msg = json.dumps(state)
            sock.sendall(msg.encode('utf-8'))
            # Discard the echo response (optional)
            _ = sock.recv(4096)
            # Compute a simple prediction: shift X by +10
            pred = {
                "prediction": {
                    "x": state["position"]["x"] + 10,
                    "y": state["position"]["y"],
                    "z": state["position"]["z"]
                }
            }
            sock.sendall(json.dumps(pred).encode('utf-8'))
            # Optional ack from server (can be empty)
            ack = sock.recv(4096)
            if not ack:
                break
            print('Sent prediction:', pred)
            time.sleep(2)

if __name__ == '__main__':
    main()

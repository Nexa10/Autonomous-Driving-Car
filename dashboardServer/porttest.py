# Get data from the esp32 and send it to the React dashboard

import socket
import threading
from flask import Flask, jsonify
from flask_cors import CORS
import logging

app = Flask(__name__)
CORS(app)

log = logging.getLogger('werkzeug')
log.setLevel(logging.WARNING)

latest_data = {}
lock = threading.Lock()

def udp_server():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server_address = ('', 6000) # port 600 to get message from the esp32
    sock.bind(server_address)
    print("Listening on port 6000...")

    try:
        while True:
            data, address = sock.recvfrom(1024)
            with lock:
                # splitting data to disctionary
                extracted_data = data.decode().split(',')

                try:
                    light_status = int(extracted_data[0].strip())
                    distance = float(extracted_data[1].strip())
                    latest_data['light'] = light_status
                    latest_data['distance'] = distance
                except:
                    print('error')
           
            print(f"Received {data.decode()}")

    except KeyboardInterrupt:
        print("Server stopped by Ctrl C....")
    finally:
        sock.close()

@app.route('/data', methods=['GET'])
def get_data():
    with lock:
        if latest_data:
            return jsonify(latest_data)
        else:
            return jsonify({"error": "No data available"}), 404

if __name__ == '__main__':

    udp_thread = threading.Thread(target=udp_server)
    udp_thread.start()
    app.run(host='localhost', port=8080)

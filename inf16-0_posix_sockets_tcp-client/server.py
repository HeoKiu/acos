import socket
import random
import struct

server_socket = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)

if __name__ == '__main__':
    port = random.randint(8000, 9000)
    print('listen', port)
    server_socket.bind(('0.0.0.0', port))

    server_socket.listen(100)

    try:
        count = 0
        while True:
            (client_socket, addr) = server_socket.accept()
            try:
                while True:
                    data = struct.unpack('<i', client_socket.recv(1024))
                    server_value = random.randint(0, 100)
                    print('server value', server_value)
                    encoded_value = struct.pack('<i', server_value)
                    client_socket.send(encoded_value)
                    count += 1
                    if count > 2:
                        client_socket.close()
                        break
            except:
                print('wrong data format')
            finally:
                client_socket.close()
    except:
        print('error')
    finally:
        server_socket.close()

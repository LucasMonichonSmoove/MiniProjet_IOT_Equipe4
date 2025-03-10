import time
import sys
import socket
import serial
import threading

# Configuration
HOST = "0.0.0.0"  # Adresse d'écoute
UDP_PORT = 44444  # Port UDP pour l'envoi des données
SERIALPORT = "COM4"  # Remplacez par le port de votre Micro:bit
BAUDRATE = 115200

# Initialisation de l'UART
ser = serial.Serial()
def initUART():
    ser.port = SERIALPORT
    ser.baudrate = BAUDRATE
    ser.bytesize = serial.EIGHTBITS
    ser.parity = serial.PARITY_NONE
    ser.stopbits = serial.STOPBITS_ONE
    ser.timeout = None
    ser.xonxoff = False
    ser.rtscts = False
    ser.dsrdtr = False
    
    print("Initialisation de l'UART avec la Micro:bit...")
    try:
        ser.open()
    except serial.SerialException:
        print(f"Erreur : Port série {SERIALPORT} non disponible")
        sys.exit()

# Envoi des données reçues via UDP
def sendUDPData(data, udp_socket, client_address):
    udp_socket.sendto(data.encode(), client_address)
    print(f"Données envoyées via UDP à {client_address}: {data}")

# Lecture des données UART et transmission UDP
def uartListener(udp_socket, client_address):
    if ser.isOpen() and ser.inWaiting() > 0:
        data_str = ser.read(ser.inWaiting()).decode().strip()
        print(f"Données reçues via UART: {data_str}")
        sendUDPData(data_str, udp_socket, client_address)

# Envoi de données à la Micro:bit via UART
def sendToMicrobit(data):
    if ser.isOpen():
        ser.write(data.encode() + b'\n')
        print(f"Message envoyé à la Micro:bit: {data}")

# Programme principal
if __name__ == '__main__':
    initUART()
    print("Serveur prêt. Attente des connexions UDP...")
    
    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_socket.bind((HOST, UDP_PORT))
    
    try:
        while True:
            print("Attente d'une connexion UDP...")
            data, client_address = udp_socket.recvfrom(1024)
            message = data.decode().strip()
            print(f"Message reçu de {client_address}: {message}")
            
            if message == "Reload":
                uartListener(udp_socket, client_address)
            else:
                message=message[:3]
                sendToMicrobit(message)
    except KeyboardInterrupt:
        ser.close()
        udp_socket.close()
        print("Serveur arrêté.")

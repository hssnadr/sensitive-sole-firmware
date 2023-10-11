import websocket

isRecv = True

ws = websocket.WebSocket() # init
ws.connect("ws://192.168.1.200:443/ws") # connect
ws.send("start") # send message

while isRecv:
    result = ws.recv()
    print(result)
    # if result == "stop":
    #     isRecv = False

ws.close() # close 

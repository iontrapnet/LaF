import paho.mqtt.client as mqtt

def on_connect(mqttc, obj, flags, rc):
    pass

def on_message(mqttc, obj, msg):
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))

def on_publish(mqttc, obj, mid):
    pass

def on_subscribe(mqttc, obj, mid, granted_qos):
    pass

mqttc = mqtt.Client()
mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_publish = on_publish
mqttc.on_subscribe = on_subscribe
mqttc.connect("localhost", 1883, 60)

mqttc.loop_start()
mqttc.publish("test", "hello", qos=2).wait_for_publish()

#mqttc.subscribe("test", 0)
#mqttc.loop_forever()
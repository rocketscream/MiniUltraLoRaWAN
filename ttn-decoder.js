/**
 * JS function to decode incoming LoRaWAN data for TheThingsNetwork.  This
 * function will be called for every received packet @ TTN.  The return
 * hash will be passed as a JSON object to MQTT clients and TTN applications.
 * 
 * Wiki: https://www.thethingsnetwork.org/docs/network/architecture.html#payload-conversion-handler
 * 
 * Details: https://www.thethingsnetwork.org/forum/t/payload-functions/5277/2
 * 
 * @param bytes LoRaWAN packet data bytes
 * @param port LoRaWAN packet port number
 * @returns hash of decoded data
 */
function Decoder(bytes, port) {
  var decoded = {};
  if (port === 1) {
    decoded.battery = ((bytes[0] << 8) | bytes[1]) / 100;
  }
  return decoded;
}
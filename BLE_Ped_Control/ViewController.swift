//
//  ViewController.swift
//  BLE_Ped_Control
//
//  Created by Larry Bonnette on 1/20/19.
//  Copyright © 2019 Larry Bonnette. All rights reserved.
//

import UIKit
import CoreBluetooth

let arduinoSvc = CBUUID.init(string: "DF01") // The Arduino service
let arduinoLEDchar = CBUUID.init(string: "DF02") // This is the Rx charateristic on the Arduino. It is the Tx here on the iPhone
let arduinoLEDstate = CBUUID.init(string: "DF03") // This is the Tx charateristic on the Arduino. It is the Rx here on the iPhone
var LedSendChar: CBCharacteristic! // This is the data sent to the Arduino (for the LED on/off)
var LedReadState: CBCharacteristic! // This is the data sent to the iPhone indicating the LED on or off state
var savedPeripheral: CBPeripheral? // This is the peripheral name used to address the Adruino

class ViewController: UIViewController, CBCentralManagerDelegate, CBPeripheralDelegate  {

    @IBAction func Stop(_ sender: Any) {
    }
    @IBAction func Up(_ sender: Any) {
    }
    @IBAction func Down(_ sender: Any) {
    }
    @IBAction func Right(_ sender: Any) {
    }
    @IBAction func Left(_ sender: Any) {
    }
    // This looks to see if Bluetooth is powered on
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        if central.state == CBManagerState.poweredOn {
            // if the power is on we look for (Scan for)  all bluetooth peripherals
            central.scanForPeripherals(withServices: nil, options: nil)
            print ("scanning...") // sent to console for debug purposes
        }
    }
    // If we find a periphal we look to see if it is our ESP32
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        if peripheral.name?.contains("ESP") == true { // if it's ours it will be advertising "ESP"
            savedPeripheral = peripheral // place the peripheral in global for use in button
            print ("The peripheral Name is ", peripheral.name ?? "no name") // sent to console for debug purposes
            centralManager.stopScan() // since we found ours we stop scanning
            print ("The Advert data is ", advertisementData) // sent to console for debug purposes
            central.connect(peripheral, options: nil) // we connect to our Adruino
        }
    }
    // if we get disconnected we start the scan again
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        central.scanForPeripherals(withServices: nil, options: nil)
    }
    // If we connect to a Bluetooth device we look for services
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        print ("Connected to = ", peripheral.name!)// sent to console for debug purposes
        peripheral.discoverServices(nil)
        peripheral.delegate = self
    }
    
    // If a service is found
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        if let services = peripheral.services {
            for svc in services { // we search through all services found
                if svc.uuid == arduinoSvc { // If we find our Arduino services
                    print ("We have found ", svc.uuid.uuidString, " This is our Arduino's Service") // sent to console for debug purposes
                    peripheral.discoverCharacteristics(nil, for: svc) // We look for all Charateristics on the Arduino
                }
            }
        }
    }
    // If we find charateristics
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        if let chars = service.characteristics {
            for char in chars { // we look at all charateristics
                print ("We have found ", char.uuid.uuidString) // sent to console for debug purposes
                if char.uuid == arduinoLEDchar { // we see if any of them are the send to Adruino charateristic
                    LedSendChar = char // place the charateristic in global for use in button
                    // We look for any "Notify" charateristics
                }else if char.properties.contains(CBCharacteristicProperties.notify) {
                    print("read and notify Characteristic \(char.uuid.uuidString)") // sent to console for debug purposes
                    LedReadState = char // Place read charateristic in global for use later
                    peripheral.setNotifyValue(true, for: char) // We turn on notify "listening"
                }
            }
        }
        // We are connected to toaster and we have all the info needed to begin so....

    }
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
    }


}


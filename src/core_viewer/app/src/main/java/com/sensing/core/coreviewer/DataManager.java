package com.sensing.core.coreviewer;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.le.ScanResult;
import android.util.Log;
import android.widget.EditText;

import org.apache.commons.collections.buffer.CircularFifoBuffer;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Singleton class to manage data received from BLE-devices.
 */
public class DataManager {
    private static final int MAX_SIZE = 100;
    private static final String TAG = "DATAMANAGER";
    private static DataManager sInstance;
    int manufacturerId = 0xffff;

    private Map<String, CircularFifoBuffer> mSensorData;
    private Map<String, Integer> mRssi = new HashMap<>(); // received signal strength indication
    private Map<String, Pair> mIdentifier = new HashMap<>(); // received signal strength indication
    private Map<String, String> mName = new HashMap<>();
    private Map<String, Integer> mBattery = new HashMap<>();
    private Map<String, byte[]> mBytes = new HashMap<>();
    private Map<String, BluetoothDevice> mDevice = new HashMap<>();
    private Map<String, String> mCalibrationUnit = new HashMap<>();
    private Map<String, Integer> mAvailableCalibrationUnits = new HashMap<>();
    private Map<String, Float> mCalibrationValue = new HashMap<>();
    private Map<String, Boolean> mIsHidden = new HashMap<>();



    public static DataManager getInstance() {
        if(sInstance == null) {
            sInstance = new DataManager();
        }
        return sInstance;
    }

    private DataManager() { // TODO: use context, see below
        mSensorData = new HashMap<>();
        mAvailableCalibrationUnits.put("Nm", 0);
        mAvailableCalibrationUnits.put("N", 1);
        mAvailableCalibrationUnits.put("mV", 2);
        mAvailableCalibrationUnits.put("kg", 3);
    }

    public void clearAll(){
        //mName.clear();
        mSensorData.clear();


    }
    /**
     * @brief takes result parameter and extracts devicestructure, devicename, raw bytes, rssi, batterylvl revisionlvl
     * @param result ScanResult parameter from ScanCallback method onScanResult(int callbackType, ScanResult result)
     */
    public void setResult(ScanResult result){
        String deviceAddress = result.getDevice().getAddress();
        byte[] manufactureSpecificData;

        if(result.getScanRecord().getManufacturerSpecificData(manufacturerId)!= null){
            manufactureSpecificData = result.getScanRecord().getManufacturerSpecificData(manufacturerId);
        }else{
            manufactureSpecificData = null;
        }
        mDevice.put(deviceAddress, result.getDevice());
        mBytes.put(deviceAddress, result.getScanRecord().getBytes());
        mRssi.put(deviceAddress, result.getRssi());
        mName.put(deviceAddress, result.getDevice().getName());

        setBatteryLvl(deviceAddress, manufactureSpecificData);
        setIdentifier(deviceAddress, manufactureSpecificData);
        setCalibrationUnit(deviceAddress, manufactureSpecificData);
        setCalibrationValue(deviceAddress, manufactureSpecificData);

    }



    private void setBatteryLvl(String deviceAddress, byte[] manufactureSpecificData){
        int battery_temp = SampleParser.twoByte2int(
                manufactureSpecificData[6],
                manufactureSpecificData[5]
        );
        mBattery.put(deviceAddress, battery_temp);
    }
    private void setIdentifier(String deviceAddress, byte[] manufactureSpecificData){
        int x = manufactureSpecificData[7]&0xff;
        int y = manufactureSpecificData[8]&0xff;

        mIdentifier.put(deviceAddress, new Pair(x, y));
    }

    private void setCalibrationUnit(String deviceAddress, byte[] manufactureSpecificData){
        int calibrationUnitByte = manufactureSpecificData[0];
        String calibrationUnit = getCalibrationUnitFromNumber(calibrationUnitByte);
        mCalibrationUnit.put(deviceAddress, calibrationUnit);
    }

    private void setCalibrationValue(String deviceAddress, byte[] manufactureSpecificData){
        byte[] calibrationValueBytes = {
                manufactureSpecificData[4],
                manufactureSpecificData[3],
                manufactureSpecificData[2],
                manufactureSpecificData[1]
        };
        ByteBuffer buffer = ByteBuffer.wrap(calibrationValueBytes);
        mCalibrationValue.put(deviceAddress, (Float) buffer.getFloat());
    }

    private String getCalibrationUnitFromNumber(Integer Number){
        for(String calibrationUnit : mAvailableCalibrationUnits.keySet()){
            Integer calibrationUnit2Number = mAvailableCalibrationUnits.get(calibrationUnit);
            if(Number.equals(calibrationUnit2Number)){
                return calibrationUnit;
            }
        }
        return null;
    }

    /**
     * Get the advertisement data available for device with address.
     * @param address Address of the device
     * @return Available data as List
     */
    public List<DeviceData> getSensorData(String address) {
        List<DeviceData> dataAsList = new ArrayList<>();

        for(Object o : mSensorData.get(address)) {
            dataAsList.add((DeviceData) o); // cast is necessary, because of CircularFifoBuffer
        }

        return dataAsList;
    }

    public Map<String, CircularFifoBuffer> getSensorData() {
        return mSensorData;
    }

    public int getCircularPufferSize(String address){
        return mSensorData.get(address).size();
    }

    /**
     * @brief adds deviceData class to ringbuffer
     * @param address
     * @param deviceData
     */
    public void add(String address, DeviceData deviceData) {
        if(mSensorData.containsKey(address)) {
            mSensorData.get(address).add(deviceData);
        } else {
            CircularFifoBuffer newList = new CircularFifoBuffer(MAX_SIZE);
            newList.add(deviceData);
            mSensorData.put(address, newList);

            DeviceListAdapter.getInstance().refreshFiltering();
        }
//        Log.d(TAG,"Added data for address: " + address
         //       + ". Size of this dataset: " + mSensorData.get(address).size());
    }


    public Map<String, Integer> getRssi(){
        return mRssi;
    }

    public void setRssi(String address, Integer rssi){
        mRssi.put(address, rssi);
    }

    public Map<String, String> getName(){
        return mName;
    }

    public Map<String, Integer> getBatteryLvl(){
        return mBattery;
    }

    public Map<String, Pair>  getRevisionLvl(){
        return mIdentifier;
    }

    public Map<String, BluetoothDevice> getBleDevice(){
        return mDevice;
    }

    public void setCalibrationUnit(String address, String calibrationUnit){
        mCalibrationUnit.put(address,calibrationUnit);
    }

    public Map<String, String> getCalibrationUnit(){
        return mCalibrationUnit;
    }

    public void setCalibrationValue(String address, Float calibrationValue){
        mCalibrationValue.put(address, calibrationValue);
    }
    public Map<String, Float> getCalibrationValue(){ return mCalibrationValue;}

    public Map<String, Integer> getAvailableCalibrationUnits(){
        return mAvailableCalibrationUnits;
    }


    public Boolean isVisible(String address){
        return mIsHidden.get(address);
    }

    public void setVisible(String address, Boolean visible){
        mIsHidden.put(address, visible);
    }

    public class Pair {
        public int x;
        public int y;

        Pair(int x, int y) {
            this.x = x;
            this.y = y;
        }

        @Override
        public String toString() {
            return Integer.toString(x) + "." + Integer.toString(y);
        }
    }
}

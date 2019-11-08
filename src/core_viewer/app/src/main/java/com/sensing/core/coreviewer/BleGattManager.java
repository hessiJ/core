package com.sensing.core.coreviewer;

import android.app.Dialog;
import android.app.ProgressDialog;
import android.app.Service;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;

import java.nio.ByteBuffer;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.UUID;

/**
 * Created by Arthur Buchta on 19.10.2018.
 */

public class BleGattManager extends Service implements Datalogger.Listener {

    private static final String TAG = "BLE_GATT_MANAGER";
    private static final String UUID_CORE_SERVICE = "19b10001-e8f2-537e-4f6c-d104768a1214";
    private static final String UUID_CORE_SETTINGS_CHARACTERISTIC = "19b10002-e8f2-537e-4f6c-d104768a1214";
    private static final String UUID_CORE_DATA_CHARACTERISTIC = "19b10003-e8f2-537e-4f6c-d104768a1214";
    private static final String UUID_CORE_DESCRIPTOR = "00002902-0000-1000-8000-00805f9b34fb";

    private static BleGattManager sInstance;
    private BluetoothGatt mGatt;
    private BluetoothDevice device;

    private static Context sContext;
    private Map<String, BluetoothGatt> mRegisteredGattProfiles = new HashMap<>();

    final Handler mHandler = new Handler();

    private Datalogger mDatalogger;
    private int mModuloCounter = 0;
    private int mModuloCounterRssi = 0;
    private boolean mIsLogging = false;
    private ProgressDialog mProgressDialog;

    public static BleGattManager getInstance() {
        if(sInstance == null) {
            sInstance = new BleGattManager();
        }
        return sInstance;
    }

    public BleGattManager(){
        mProgressDialog = new ProgressDialog(sContext);
        mDatalogger = new Datalogger(sContext, this, false);
    }

    /**
     * Create a new logfile wiht current date and time in filename and start writing
     * notification-data into it.
     */
    public void startNewLog() {
        mIsLogging = true;
        Toast.makeText(getContext(), "Start logging of connected devices.", Toast.LENGTH_SHORT).show();
    }

    public void stopLog() {
        mIsLogging = false;
        Toast.makeText(getContext(), "Stop logging of connected devices", Toast.LENGTH_SHORT).show();
        mDatalogger.reset();
    }

    public BluetoothDevice getDevice(){ return device; }

    private Context getContext(){ return sContext; }

    public BluetoothGatt getGatt(String address){
        return mRegisteredGattProfiles.get(address);
    }

    private void readRssi(String address){
        mModuloCounterRssi++;
        if((mModuloCounterRssi % 100) == 0) {
            BluetoothGatt gattProfile = mRegisteredGattProfiles.get(address);
            gattProfile.readRemoteRssi();
        }
    }



    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }



    private void broadcastUpdateSensorData(final String action, BluetoothGattCharacteristic characteristic  , String address) {
        byte[] data = characteristic.getValue();
        String increment = Integer.toString(data[0] & 0xff);

        double calibrationValue = DataManager.getInstance().getCalibrationValue().get(address);

        ArrayList<Double> sensorData = new ArrayList<>(DeviceData.getAdcDataNotification(data, calibrationValue));

        // write received data to log if enabled
        if(mIsLogging) {
            mDatalogger.log(address, increment, String.valueOf(sensorData.get(0)));
            mDatalogger.log(address, increment, String.valueOf(sensorData.get(1)));
            mDatalogger.log(address, increment, String.valueOf(sensorData.get(2)));
            mDatalogger.log(address, increment, String.valueOf(sensorData.get(3)));
            mDatalogger.log(address, increment, String.valueOf(sensorData.get(4)));
            mDatalogger.log(address, increment, String.valueOf(sensorData.get(5)));
        }
        
        if(PeripheralDetailsDialogFragment.getInstance().isActive()) {
            updateDialogSensorData(address, sensorData.get(0));
            readRssi(address);
        }
        // broadcast received data
        Intent intent = new Intent("BLE_SENSOR_DATA");
        intent.putExtra("address", address);
        intent.putExtra("value", sensorData.get(0));
        intent.putExtra("min", Collections.min(sensorData));
        intent.putExtra("max", Collections.max(sensorData));
        LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
    }

    private void updateDialogSensorData(String address, double sensorValue) {

        mModuloCounter++;
        if((mModuloCounter % 100) == 0) {
            ((MainActivity)sContext).runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    PeripheralDetailsDialogFragment.getInstance().updateSensorValue(address, sensorValue);
                }
            });
        }
    }


    private void broadcastUpdateConfiguration(final String action, BluetoothGattCharacteristic characteristic  , String adress) {
        // Send
        byte[] data = characteristic.getValue();
        Intent intent = new Intent("CONFIGURATION");
        intent.putExtra(adress,
                data);
        LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
    }

    private void broadcastUpdateConnected(String address, Boolean state) {
        Intent intent = new Intent("connectionState");
        intent.putExtra("address", address);
        intent.putExtra("state", state);
        LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
    }

    public void connectBleDevice(BluetoothDevice device, Context context) {
        List<BluetoothDevice> connectedBleDevices = getConnectedBleDevices(context);
        connect(device, connectedBleDevices);
        showProgressDialog("Bluetooth connection.", "Wait until connection is being established...");
    }

    public void disconnectBleDevice(BluetoothDevice device, Context context) {
        List<BluetoothDevice> connectedBleDevices = getConnectedBleDevices(context);
        disconnect(device, connectedBleDevices);
        showProgressDialog("Bluetooth connection.", "Wait until connection is being closed...");
    }

    public List<BluetoothDevice> getConnectedBleDevices(Context context){
        BluetoothManager bluetoothManager = (BluetoothManager) context.getSystemService(context.BLUETOOTH_SERVICE);
        return bluetoothManager.getConnectedDevices(BluetoothProfile.GATT);
    }

    public List<BluetoothDevice> getConnectedBleDevices(){
        BluetoothManager bluetoothManager = (BluetoothManager) sContext.getSystemService(sContext.BLUETOOTH_SERVICE);
        return bluetoothManager.getConnectedDevices(BluetoothProfile.GATT);
    }

    public boolean isDeviceConnected(String address, Context context){
        if(deviceWasOrIsConnected(address)){
            BluetoothDevice device = getGatt(address).getDevice();
           return getConnectedBleDevices(context).contains(device);
        }else{
            return false;
        }
    }

    private boolean deviceWasOrIsConnected(String address){
        if(getGatt(address) != null){
            return true;
        }else{
            return false;
        }
    }

    private void connect(BluetoothDevice device, List<BluetoothDevice> connectedBleDevices){
        Log.d(TAG, "connectBleDevice");
        if (mRegisteredGattProfiles.get(device.getAddress()) == null) {
            mRegisteredGattProfiles.put(device.getAddress(), device.connectGatt(sContext, true, gattCallback));
        }else if(!connectedBleDevices.contains(device)){
            mRegisteredGattProfiles.put(device.getAddress(),device.connectGatt(sContext, true, gattCallback));
        }else{
            //TODO: What to do if mGatt != null and device is connected???
        }
    }

    private void disconnect(BluetoothDevice device, List<BluetoothDevice> connectedBleDevices){
        Log.d(TAG, "disconnect");
        if (mRegisteredGattProfiles.get(device.getAddress()) == null) {
            Log.d(TAG, "Device was never connected");
        }else if(connectedBleDevices.contains(device)){
            BluetoothGatt deviceProfile = mRegisteredGattProfiles.get(device.getAddress());
            deviceProfile.disconnect();
        }else{
            Log.d(TAG, "Device is already disconnected");
        }
    }


    public void disconnectAllBle(){
        for(BluetoothGatt gatt : mRegisteredGattProfiles.values()){
            gatt.disconnect();
        }
    }

/*
    public boolean readCharacteristic(BluetoothGattCharacteristic characteristic){
        return getGatt().readCharacteristic(characteristic);
    }
*/


    public boolean setCalibrationValue(String address, float calibrationValue){
        BluetoothGattCharacteristic characteristic =
                getCharacteristic(address, UUID_CORE_SERVICE, UUID_CORE_SETTINGS_CHARACTERISTIC);
        byte calibrationValueIdentifier = 0x21;
        byte[] tempBytes = ByteBuffer.allocate(4).putFloat(calibrationValue).array();
        byte[] floatAsBytesFormatted = {calibrationValueIdentifier, tempBytes[3], tempBytes[2], tempBytes[1], tempBytes[0]};
        return writeCharacteristic(address, characteristic, floatAsBytesFormatted);
    }

    public boolean setCalibrationUnit(String address, byte calibrationUint){
        BluetoothGattCharacteristic characteristic =
                getCharacteristic(address, UUID_CORE_SERVICE, UUID_CORE_SETTINGS_CHARACTERISTIC);

        byte calibrationUnitIdentifier = 0x20;
        byte[] package2send = {calibrationUnitIdentifier, calibrationUint};
        return writeCharacteristic(address, characteristic, package2send);
    };

    public boolean resetDevice(String address) {
        BluetoothGattCharacteristic characteristic =
                getCharacteristic(address, UUID_CORE_SERVICE, UUID_CORE_SETTINGS_CHARACTERISTIC);
        byte[] resetCode = {0x24};
        return writeCharacteristic(address, characteristic, resetCode);
    }

    /**
     * @brief sets the devicename of the peripheral if connected
     * @param address peripheral address
     * @param newPeripheralName
     * @return
     */
    public boolean setPeripheralName(String address, String newPeripheralName){

        String peripheralNameIdentifier = "#";
        BluetoothGattCharacteristic characteristic =
                getCharacteristic(address, UUID_CORE_SERVICE, UUID_CORE_SETTINGS_CHARACTERISTIC);

        newPeripheralName = peripheralNameIdentifier + newPeripheralName;
        return writeCharacteristic(address, characteristic, newPeripheralName);
    }

    public boolean writeCharacteristic(String address, BluetoothGattCharacteristic characteristic, String value){
        //int properties = characteristic.getProperties();
        characteristic.setValue(value);
        return getGatt(address).writeCharacteristic(characteristic);
    }

    public boolean writeCharacteristic(String address, BluetoothGattCharacteristic characteristic, byte[] bytes){
        //int properties = characteristic.getProperties();
        characteristic.setValue(bytes);
        return getGatt(address).writeCharacteristic(characteristic);
    }


    public boolean startNotification(String address,
                                     String uuid_service,
                                     String uuid_characteristic){

        BluetoothGattCharacteristic characteristic =
                getCharacteristic(address, uuid_service, uuid_characteristic);

        if(characteristicExist(characteristic)){
            return enableNotification(address, characteristic);
        }else{
            Log.d(TAG, "startNotification: " + "Service: " + uuid_service +
                    " or Characteristic: " + uuid_characteristic + " does not exist!");
            return false;
        }
    }

    /**
     *
     * @param uuid_service
     * @param uuid_characteristic
     * @return BluetoothGattCharacteristic null if service or characteristic does not exist
     */
    private BluetoothGattCharacteristic getCharacteristic(String address, String uuid_service, String uuid_characteristic){
        for (BluetoothGattService bluetoothGattService : getGatt(address).getServices()) {
            Log.d(TAG, "getCharacteristic: " + bluetoothGattService.toString());
        }
        BluetoothGattService service = getGatt(address).getService(UUID.fromString(uuid_service));
        if(service == null) {
            return null;
        }
        return service.getCharacteristic(UUID.fromString(uuid_characteristic));
    }


    private boolean characteristicExist(BluetoothGattCharacteristic characteristic){
        if(characteristic == null){
            return false;
        }else{
            return true;
        }
    }


    private boolean enableNotification(String address, BluetoothGattCharacteristic characteristic){

        getGatt(address).setCharacteristicNotification(characteristic,true);
        BluetoothGattDescriptor descriptor = characteristic.getDescriptor(
                UUID.fromString(UUID_CORE_DESCRIPTOR));

        if(descriptor!=null){ // if descriptor exist
            descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
            return getGatt(address).writeDescriptor(descriptor);
        }else{
            Log.d(TAG, "enableNotification: " + "Descriptor: " + UUID_CORE_DESCRIPTOR + "does not exist!");
            return false;
        }
    }

/*
    public boolean disableNotification(BluetoothGattCharacteristic characteristic){
        Log.e("enableNotification", "Enable Notif. for charac: " + characteristic.getUuid().toString());
        boolean boolTemp = false;
        Log.e("enableNotify", "SetCharaToNotify: " + getGatt().setCharacteristicNotification(characteristic,true));

        BluetoothGattDescriptor descriptor = characteristic.getDescriptor(
                UUID.fromString("00002902-0000-1000-8000-00805f9b34fb"));
        descriptor.setValue(BluetoothGattDescriptor.DISABLE_NOTIFICATION_VALUE);


        //boolTemp = getGatt().writeDescriptor(descriptor);
        return boolTemp;
    }
*/


    private final BluetoothGattCallback gattCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            Log.d(TAG, "gattCallback" + "Status: " + status);
            switch (newState) {
                case BluetoothProfile.STATE_CONNECTED:
                    Log.d(TAG, "gattCallback" + gatt.getDevice().getName() +
                            " " +  gatt.getDevice().getAddress().toString() + " STATE_CONNECTED");

                    //
                    broadcastUpdateConnected(gatt.getDevice().getAddress().toString(), true);
                    gatt.discoverServices();
                    mHandler.postDelayed(new Runnable(){
                        @Override
                        public void run() {
                            startNotification(gatt.getDevice().getAddress().toString(), UUID_CORE_SERVICE, UUID_CORE_DATA_CHARACTERISTIC);
                            Toast.makeText(sContext, "Connected Device: " + gatt.getDevice().getAddress(), Toast.LENGTH_SHORT).show();

                            // remove "loading"-dialog
                            if(mProgressDialog != null)
                                mProgressDialog.dismiss();
                        }
                        }, 1000);

                    break;
                case BluetoothProfile.STATE_DISCONNECTED:
                    Log.d(TAG, "gattCallback" + gatt.getDevice().getName() +
                            " " + gatt.getDevice().getAddress().toString() + "STATE_DISCONNECTED");
                    //
                    gatt.close();
                    showToastDisconnect(gatt.getDevice().getAddress());
                    broadcastUpdateConnected(gatt.getDevice().getAddress().toString(), false);
                    break;
                default:
                    Log.d(TAG, "gattCallback" + gatt.getDevice().getName() +
                            " " + gatt.getDevice().getAddress().toString() + "STATE_OTHER");
            }
            ((MainActivity)sContext).runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    DeviceListAdapter.getInstance().notifyDataSetChanged();
                }
            });
        }

        private void showToastDisconnect(String address){
            mHandler.postDelayed(new Runnable(){
                @Override
                public void run() {
                    Toast.makeText(sContext, "Disconnected Device: " + address, Toast.LENGTH_SHORT).show();
                    // remove "loading"-dialog
                    if(mProgressDialog != null)
                        mProgressDialog.dismiss();
                }
            }, 1000);
        }
        /*
        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            List<BluetoothGattService> services = gatt.getServices();
            Log.e("onServicesDiscovered", services.toString());
            gatt.readCharacteristic(services.get(1).getCharacteristics().get
                    (0));
        }
        */

        @Override
        public void onCharacteristicRead(BluetoothGatt gatt,
                                         BluetoothGattCharacteristic
                                                 characteristic, int status) {

            final byte[] data = characteristic.getValue();
            broadcastUpdateConfiguration("DataAvailable", characteristic, gatt.getDevice().getAddress());
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt,
                                            BluetoothGattCharacteristic characteristic) {
            //Log.d(TAG, "onCharacteristicChanged: " + characteristic.getValue());

            broadcastUpdateSensorData("DataAvailable", characteristic, gatt.getDevice().getAddress());
        }

        @Override
        public void onReadRemoteRssi(BluetoothGatt gatt, int rssi, int status){
            Log.d(TAG, "onReadRemoteRssi: " + rssi);
            DataManager.getInstance().setRssi(gatt.getDevice().getAddress(), rssi);

            ((MainActivity)sContext).runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    PeripheralDetailsDialogFragment.getInstance().updateRssiBattValue();
                }
            });

        }
    };

    private void showProgressDialog(String title, String message) {
        mProgressDialog = new ProgressDialog(sContext);
        mProgressDialog.setTitle(title);
        mProgressDialog.setMessage(message);
        mProgressDialog.setCancelable(true);
        mProgressDialog.show();
    }

    public static void setContext(Context context) {
        sContext = context;
    }

    public String getmLogFilename() {
        return mDatalogger.getmFilename();
    }

    public void setmLogFilename(String mLogFilename) {
        mDatalogger.setmFilename(mLogFilename);
    }

    @Override
    public void onFileSizeUpdate(long fileSize) {
        Log.d(TAG, "onFileSizeUpdate: " + fileSize);
        ((MainActivity)sContext).runOnUiThread(new Runnable() {

            @Override
            public void run() {
                TextView sizeTextView = ((MainActivity)sContext).findViewById(R.id.recording_status_text);
//                long value = fileSize/(long)(1024 * 1024);
//                Log.d(TAG, "run: value: " + value);
                sizeTextView.setText(humanReadableByteCount(fileSize, false));
            }
        });
    }

    /**
     * Convert bytes to Human readable format.
     * @param bytes Byte-value
     * @param si
     * @return String of bytes-parameter
     */
    public static String humanReadableByteCount(long bytes, boolean si) {
        int unit = si ? 1000 : 1024;
        if (bytes < unit) return bytes + " B";
        int exp = (int) (Math.log(bytes) / Math.log(unit));
        String pre = (si ? "kMGTPE" : "KMGTPE").charAt(exp-1) + (si ? "" : "i");
        return String.format("%.1f %sB", bytes / Math.pow(unit, exp), pre);
    }
}

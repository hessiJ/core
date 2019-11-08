package com.sensing.core.coreviewer;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanFilter;
import android.bluetooth.le.ScanResult;
import android.bluetooth.le.ScanSettings;

import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Handler;
import android.os.ParcelUuid;
import android.support.v4.content.ContextCompat;
import android.support.v4.content.LocalBroadcastManager;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static android.content.Context.BLUETOOTH_SERVICE;
import static com.sensing.core.coreviewer.Utilities.bytesToHexString;


public class BleGapManager {
    private static final String TAG = "BLE_GAP_MANAGER";

    private static final String UUID_CORE_SERVICE = "19b10001-e8f2-537e-4f6c-d104768a1214";

    private static int REQUEST_FINE_LOCATION = 1;
    private static final int REQUEST_ENABLE_BT = 2;
    private static final int mUpdateBoundary = 10;
    private AppCompatActivity mParent;
    private BluetoothAdapter mBluetoothAdapter;
    private BtleScanCallback mScanCallback;
    private BluetoothLeScanner mBluetoothLeScanner;
    private DataManager mDataManager;
    private boolean mScanning = false;
    private HashMap<String, BluetoothDevice> mScanResults = new HashMap<>(); // device address -> device
    private Handler mHandler = null;
    private ScanFilter mNameFilter;
    private Map<String, Integer> mReceivingCounter = new HashMap<>();


    BleGapManager(AppCompatActivity parent) {
        mParent = parent;
        BluetoothManager bluetoothManager = (BluetoothManager) mParent.getSystemService(BLUETOOTH_SERVICE);
        mBluetoothAdapter = bluetoothManager.getAdapter();
        mParent = parent;
        mDataManager = DataManager.getInstance();
    }

    /**
     * Check whether access to bluetooth adapter and location permission is granted
     * @return true if bluetooth access and location permission is granted, false otherwise
     */
    private boolean hasPermissions() {
        if (mBluetoothAdapter == null || !mBluetoothAdapter.isEnabled()) {
            requestBluetoothEnable();
            return false;
        } else if (!hasLocationPermissions()) {
            requestLocationPermission();
            return false;
        }
        return true;
    }

    /**
     * Tries to start bluetooth on device.
     */
    private void requestBluetoothEnable() {
        Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
        mParent.startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        Log.d(TAG, "requestBluetoothEnable: Requested user enables Bluetooth. Try starting the scan again.");
    }

    /**
     * Check whether location permission is granted
     * @return true if location permission is granted
     */
    private boolean hasLocationPermissions() {
        return ContextCompat.checkSelfPermission(mParent, Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED;
    }

    /**
     * Request location permission.
     */
    private void requestLocationPermission() {
        mParent.requestPermissions(new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, REQUEST_FINE_LOCATION);
    }

    public void startScan() {
        if (!hasPermissions() || mScanning) {
            return;
        }
        List<ScanFilter> filters = new ArrayList<>();
        ScanFilter uuidFilter = new ScanFilter.Builder()
                .setServiceUuid(ParcelUuid.fromString(UUID_CORE_SERVICE))
                //.setDeviceAddress("DF:88:C8:3A:2E:2E")
//                .setDeviceName("core")
                .build();
        mNameFilter = new ScanFilter.Builder()
//                .setServiceUuid(ParcelUuid.fromString(UUID_CORE_SERVICE))
//                .setDeviceAddress("DF:88:C8:3A:2E:2E")
                .setDeviceName("core")
                .build();

        filters.add(uuidFilter);
        filters.add(mNameFilter);

        // filter for set of addresses
//        for(String address : adresses) {
//            ScanFilter scanFilter = new ScanFilter.Builder()
//                    .setDeviceAddress(address)
//                    .build();
//            filters.add(scanFilter);
//        }

        ScanSettings settings = new ScanSettings.Builder()
                .setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY)
                .build();
        //mScanCallback = new BtleScanCallback(mScanResults);
        mScanCallback = new BtleScanCallback();

        mBluetoothLeScanner = mBluetoothAdapter.getBluetoothLeScanner();
        Log.d(TAG, "Starting scan...");
        mBluetoothLeScanner.startScan(filters, settings, mScanCallback);
        mScanning = true;

        // limit scanning interval; scans infinitely when commented out
        //mHandler = new Handler();
        //mHandler.postDelayed(this::stopScan, SCAN_PERIOD);
    }

    public void stopScan() {
        if (mScanning && mBluetoothAdapter != null && mBluetoothAdapter.isEnabled() && mBluetoothLeScanner != null) {
            mBluetoothLeScanner.stopScan(mScanCallback);
            Log.d(TAG, "Stopping scan...");

//            Log.d(TAG, "Received " + advCounter + " advertisements.");
//            advCounter = 0;

            scanComplete();
        }

        mScanCallback = null;
        mScanning = false;
        mHandler = null;
    }

    private void scanComplete() {
        if (mScanResults.isEmpty()) {
            return;
        }
        for (String deviceAddress : mScanResults.keySet()) {
            Log.d(TAG, "Found device: " + deviceAddress);
        }
    }

    public HashMap<String, BluetoothDevice> getScanResults() {
        return mScanResults;
    }

    public boolean hasScanResults() {
        return !mScanResults.isEmpty();
    }


    private final BluetoothGattCallback gattCallback = new BluetoothGattCallback() {
            @Override
            public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
                Log.i("onConnectionStateChange", "Status: " + status);
                switch (newState) {
                    case BluetoothProfile.STATE_CONNECTED:

                        Log.d(TAG, "onConnectionStateChange " + gatt.getDevice().getName() +
                                " " +  gatt.getDevice().getAddress().toString() + " STATE_CONNECTED");
                        gatt.discoverServices();
                        break;
                    case BluetoothProfile.STATE_DISCONNECTED:
                        Log.d(TAG, "onConnectionStateChange " + gatt.getDevice().getName() +
                                " " + gatt.getDevice().getAddress().toString() + "STATE_DISCONNECTED");

                        break;
                    default:
                        Log.d(TAG, "onConnectionStateChange " + gatt.getDevice().getName() +
                                " " + gatt.getDevice().getAddress().toString() + "STATE_OTHER");
                }
            }

            @Override
            public void onServicesDiscovered(BluetoothGatt gatt, int status) {
                super.onServicesDiscovered(gatt, status);
                switch (status) {
                    case BluetoothGatt.GATT_SUCCESS: gatt.close();
                        Log.d(TAG,"gattCallback: onServicesDiscovered: SUCCESS for " + gatt.getDevice().getAddress());
                        break;
                    case BluetoothGatt.GATT_FAILURE:
                        Log.d(TAG,"gattCallback: onServicesDiscovered: FAILURE for " + gatt.getDevice().getAddress());
                        break;
                    default:
                        Log.d(TAG,"gattCallback: onServicesDiscovered: UNKNOWN STATUS for " + gatt.getDevice().getAddress());

                }
            }

            @Override
            public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
//            super.onCharacteristicWrite(gatt, characteristic, status);
                if(status == BluetoothGatt.GATT_SUCCESS) {
                    Log.d(TAG,"gattCallback: GATT write success: " + characteristic.getUuid() + " value: " + bytesToHexString(characteristic.getValue()));
                }
                if(status == BluetoothGatt.GATT_FAILURE) {
                    // TODO reconnect
                    Log.d(TAG,"gattCallback:  FAILURE: Address: " + gatt.getDevice().getAddress() + " characteristik UUID: " + characteristic.getUuid());
                }
                if(status == BluetoothGatt.GATT_WRITE_NOT_PERMITTED) {
                    Log.d(TAG,"gattCallback: onCharacteristicWrite: WRITE NOT PERMITTED: Address: " + gatt.getDevice().getAddress() + " characteristik UUID: " + characteristic.getUuid());
                }
            }

            @Override
            public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
                Log.d(TAG,"gattCallback: onCharacteristicWrite: ");

                super.onCharacteristicChanged(gatt, characteristic);
            }
    };


    private class BtleScanCallback extends ScanCallback {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            addScanResult(result);
            int manufacturerId = 0xffff;
            DeviceData data = new DeviceData(result.getScanRecord().getManufacturerSpecificData(manufacturerId));
            byte[] manufactureData = result.getScanRecord().getManufacturerSpecificData(manufacturerId);


            //Log.v(TAG, "onScanResult: Raw data: " + Utilities.bytesToHexString(result.getScanRecord().getBytes()));
            //Log.v(TAG, "onScanResult: Raw data: " + result.toString());

            if (result.getScanRecord().getManufacturerSpecificData(manufacturerId) != null) {
                //Log.v(TAG, "onScanResult: Manufacturer specific data: " + Utilities.bytesToHexString(data.getmRawData()));
                //Log.v(TAG, "onScanResult: Received value from " + result.getDevice().getAddress() + " value: " + data.getAdcDataAdvertisement().get(0));

                mDataManager.setResult(result);
                mDataManager.add(result.getDevice().getAddress(), data);
                double calibrationValue = DataManager.getInstance().getCalibrationValue().get(result.getDevice().getAddress());

                ArrayList<Double> sensorData = new ArrayList<>(DeviceData.getAdcDataAdvertisement(manufactureData, calibrationValue));
                String tempAddress = result.getDevice().getAddress();

                assignCounter(tempAddress, mReceivingCounter.get(tempAddress));
                Integer currentCounterVal = mReceivingCounter.get(tempAddress);
                updateInterfaces(tempAddress, currentCounterVal, sensorData.get(0));

                // add received data point to plot
                broadcastData(result.getDevice().getName(),
                        tempAddress,
                        sensorData.get(0),
                        sensorData.get(1),
                        sensorData.get(2));
            } else {
                Log.d(TAG, "onScanResult: unknown manufacturer id: skipping received data.");
            }
        }

        private void assignCounter(String address, Integer counter){
            if(counter == null){
                mReceivingCounter.put(address, 0);
            }else{
                mReceivingCounter.put(address, counter+1);
            }
        }

        private void resetCounter(String address){
            mReceivingCounter.put(address, 0);
        }

        private void updateInterfaces(String address, Integer counterVal, Double sensorData){
            if(counterVal>mUpdateBoundary) {
                DeviceListAdapter.getInstance().init();
                DeviceListAdapter.getInstance().notifyDataSetChanged();

                PeripheralDetailsDialogFragment.getInstance().updateRssiBattValue();
                PeripheralDetailsDialogFragment.getInstance().updateSensorValue(address, sensorData);
                resetCounter(address);
            }
        }

        private void broadcastData(String name, String address, Double value, Double min, Double max) {
            Intent intent = new Intent("BLE_SENSOR_DATA");


            intent.putExtra("address", address);
            intent.putExtra("value", value);
            intent.putExtra("min", min);
            intent.putExtra("max", max);


            LocalBroadcastManager.getInstance(mParent).sendBroadcast(intent);
        }

        @Override
        public void onBatchScanResults(List<ScanResult> results) {
            for (ScanResult result : results) {
                addScanResult(result);
            }
        }
        @Override
        public void onScanFailed(int errorCode) {
            Log.e(TAG, "BLE Scan Failed with code " + errorCode);
        }

        private void addScanResult(ScanResult result) {
            BluetoothDevice device = result.getDevice();
            String deviceAddress = device.getAddress();
            mScanResults.put(deviceAddress, device);
//            mAdcOnButton.setEnabled(true);
//            mAdcOffButton.setEnabled(true);
        }
    }



    interface DataListener {
        void appendData(double value, String deviceAddress, String deviceName);
    }
}

package com.sensing.core.coreviewer;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.le.BluetoothLeScanner;

import android.content.BroadcastReceiver;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.os.Handler;
import android.support.annotation.LayoutRes;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v4.content.LocalBroadcastManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.Toast;

import java.util.Arrays;
import java.util.HashMap;
import java.util.List;

public class MainActivity extends AppCompatActivity implements
        PeripheralDetailsDialogFragment.PeripheralDialogFragmentListener,
        BleGapManager.DataListener {

    public GraphFragment mGraph;

    private static final int REQUEST_ENABLE_BT = 2;
    private static final String TAG = "MAIN_ACTIVITY";
//    private static final long SCAN_PERIOD = 30000;

    private static final String UUID_CORE_SERVICE = "19b10001-e8f2-537e-4f6c-d104768a1214";
    private static final String UUID_CORE_DATA_CHARACTERISTIC = "19b10003-e8f2-537e-4f6c-d104768a1214";

    private static final List<String> adresses = Arrays.asList("E1:16:34:2A:8A:A6", "E4:3D:8C:07:E1:1C");
    private static final byte[] SIGNAL_ADC_OFF = {(byte) 0x01, (byte) 0x00};
    private static final byte[] SIGNAL_ADC_ON = {(byte) 0x01, (byte) 0x01};
    private static final String LINE_CHART_TAG = "LINE_CHART";
    private static final String BAR_CHART_TAG = "BAR_CHART";
    private static final String DEVICE_FRAGMENT_TAG = "DeviceList";

    private static int REQUEST_FINE_LOCATION = 1;
    private BluetoothAdapter mBluetoothAdapter;
    private boolean mLogging = false;
    private boolean mInBarGraph = true;
    private HashMap<String, BluetoothDevice> mScanResults = new HashMap<>(); // device address -> device // TODO: use DataManager instead
    private DataManager mDataManager;
//    private BtleScanCallback mScanCallback;
    private BluetoothLeScanner mBluetoothLeScanner;
    private Handler mHandler;

    private ImageButton mBleDevicesButton;
    private ImageButton mLogButton;

    private ImageButton mSwitchGraph;
    private ImageButton mZoomInButton;
    private ImageButton mZoomOutButton;
    private ImageButton mSettingsButton;
    private ImageButton mSnapshotButton;
    private boolean mAdcEnabled;
    private BleGapManager mConnectionManager;

    private FragmentManager mFragmentManager;

    private FrameLayout frameLayout;
    private ColorStateList mStdColors;

    private enum GraphState { LIVE_VIEW, BAR_CHART }
    private GraphState mGraphState = GraphState.BAR_CHART; // configure which graph is shown at startup

    private BroadcastReceiver mMessageReceiver;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(getLayoutResId());

        // prevent automatic stand-by
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);


        BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(BLUETOOTH_SERVICE);
        mBluetoothAdapter = bluetoothManager.getAdapter();

        mConnectionManager = new BleGapManager(this);

        configureButtons();

        // inflate graph
        mFragmentManager = getSupportFragmentManager();
        frameLayout = (FrameLayout) findViewById(R.id.fragment_container);
        FragmentTransaction transaction = mFragmentManager.beginTransaction();
        if(mGraphState == GraphState.LIVE_VIEW) {
            mGraph = new LiveChartFragment();
        } else if(mGraphState == GraphState.BAR_CHART) {
            mGraph = new BarChartFragment();
        }
        String tag = (mGraphState == GraphState.LIVE_VIEW) ? LINE_CHART_TAG : BAR_CHART_TAG;
        mFragmentManager.beginTransaction().add(R.id.fragment_container, mGraph, tag).commit();

        // inflate list, if on tablet, i.e. if the view contains the appropriate Framelayout
        if(findViewById(R.id.list_fragment_container) != null) {
            FragmentTransaction transaction2 = getSupportFragmentManager().beginTransaction();
            transaction2.replace(R.id.list_fragment_container, DevicesFragment.getInstance());
            transaction2.commit();
        }

        getSupportActionBar().hide();

        BleGattManager.setContext(this);

        TextView recLabel = findViewById(R.id.recording_status_label);
        mStdColors =  recLabel.getTextColors();
    }

    @Override
    protected void onPause() {
        super.onPause();
        LocalBroadcastManager.getInstance(this).unregisterReceiver(mMessageReceiver);
        mConnectionManager.stopScan();
    }

//    @Override
//    public void onBackPressed() {
//        //Ignore back button when in tablet layout
//        if(getSupportFragmentManager().findFragmentByTag(DEVICE_FRAGMENT_TAG) == null)
//            return;
//        super.onBackPressed();
//
//    }

    private void configureButtons() {

        mBleDevicesButton = (ImageButton) findViewById(R.id.openListFragment);

        if(mBleDevicesButton != null) {
            mBleDevicesButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {

                    // inflate only once on multiple button presses
                    if(getSupportFragmentManager().findFragmentByTag(DEVICE_FRAGMENT_TAG) != null)
                        return;

                    FragmentTransaction transaction = getSupportFragmentManager().beginTransaction();
                    transaction.replace(R.id.fragment_container, DevicesFragment.getInstance(), DEVICE_FRAGMENT_TAG);
                    transaction.addToBackStack(null);
                    transaction.commit();
                }
            });
        }

        mLogButton = (ImageButton) findViewById(R.id.button_start_log);
        mLogButton.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View view){
                toggleImageAndLogging();
            }
        });



        mSwitchGraph = (ImageButton) findViewById(R.id.switchGraph);
        mSwitchGraph.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View view){
                toggleImageAndSwitchGraph();
            }
        });



        mZoomInButton = (ImageButton) findViewById(R.id.button_zoom_in);
        mZoomInButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
//                if(mGraphState == GraphState.BAR_CHART){
////                    mGraph.zoomIn();
////                }
                mGraph.zoomIn();
            }
        });

        mZoomOutButton = (ImageButton) findViewById(R.id.button_zoom_out);
        mZoomOutButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
//                if(mGraphState == GraphState.BAR_CHART){
////                    mGraph.zoomOut();
////                }
                mGraph.zoomOut();
            }
        });
        // make zoom buttons invisible when autoscale deactivated
        boolean autoscaleOn = getPreferences(AppCompatActivity.MODE_PRIVATE).getBoolean("auto_scale_checked", false);
        if(autoscaleOn) {
            mZoomInButton.setVisibility(View.GONE);
            mZoomOutButton.setVisibility(View.GONE);
        } else {
            mZoomInButton.setVisibility(View.VISIBLE);
            mZoomOutButton.setVisibility(View.VISIBLE);
        }


        mSettingsButton = (ImageButton) findViewById(R.id.settingsButton);
        mSettingsButton.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View v) {
                SettingsFragmentDialog settings = SettingsFragmentDialog.getInstance();
                if(mFragmentManager.findFragmentByTag("settings") != null) {
                    return;
                }
                settings.show(mFragmentManager, "settings");
            }
        });

        mSnapshotButton = (ImageButton) findViewById(R.id.button_snapshot);
        mSnapshotButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mGraph.takeSnapshot();
                Toast.makeText(MainActivity.this, "Snapshot of graph taken.", Toast.LENGTH_SHORT).show();
            }
        });


    }


    private void toggleImageAndLogging(){
        TextView statusLabel = (TextView) findViewById(R.id.recording_status_label);
        TextView statusText = (TextView) findViewById(R.id.recording_status_text);
        if(mLogging){
            mLogButton.setImageResource(R.mipmap.play_button);
            BleGattManager.getInstance().stopLog();

            //update status field
            statusLabel.setText(getString(R.string.not_recording_text));
            statusText.setText("");
            statusLabel.setTextColor(mStdColors); // reset color to original color according to theme

            mLogging = false;
        }else{
            BleGattManager.getInstance().startNewLog();
            mLogButton.setImageResource(R.mipmap.stop_button);

            // update status field
            statusLabel.setText("Current log-file-size: ");
            statusLabel.setTextColor(Color.RED);

            // see if connected to any device. If not notify the user.
            BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(BLUETOOTH_SERVICE);
            List<BluetoothDevice> devices = bluetoothManager.getConnectedDevices(BluetoothProfile.GATT);
            if(devices.size() == 0) {
                Toast.makeText(this, "Note: Only data from connected devices is logged. You are currently not connected ot any devices.", Toast.LENGTH_LONG).show();
            }
            Log.d(TAG, "toggleImageAndLogging: " + devices);

            mLogging = true;
        }
    }

    private void toggleImageAndSwitchGraph(){
        switchGraphFragment();
        if(mGraphState == GraphState.LIVE_VIEW){
            mSwitchGraph.setImageResource(R.mipmap.bargraph_button);
        }else{
            mSwitchGraph.setImageResource(R.mipmap.linegraph_button);
        }
    }

    public void switchGraphFragment(){
        if(getSupportFragmentManager().findFragmentByTag(DEVICE_FRAGMENT_TAG) != null) {
            getSupportFragmentManager().popBackStack();
        }
        switchGraph();
    }


    public void bleConnect(String address){
        Log.d(TAG, "bleConnect: ");
        BluetoothDevice bleDevice = DataManager.getInstance().getBleDevice().get(address);
        BleGattManager.getInstance().connectBleDevice(bleDevice, getBaseContext());
    }

    public void bleDisconnect(String address){
        Log.d(TAG, "bleDisconnect: ");
        BluetoothDevice bleDevice = DataManager.getInstance().getBleDevice().get(address);
        BleGattManager.getInstance().disconnectBleDevice(bleDevice, getBaseContext());
    }


    public void bleStartNotification(String address){
        Log.d(TAG, "bleStartNotification: ");
        BleGattManager.getInstance().startNotification(address, UUID_CORE_SERVICE, UUID_CORE_DATA_CHARACTERISTIC);
    }


    private void switchGraph() {
        String tag = (mGraphState == GraphState.LIVE_VIEW) ? LINE_CHART_TAG : BAR_CHART_TAG;
        Log.d(TAG, "switchGraph: fm size (before switch): " + mFragmentManager.getBackStackEntryCount());

        switch (mGraphState) {
            case LIVE_VIEW: // switch from live chart to bar chart
                Log.d(TAG, "switchGraph: switch to bar chart");
                mGraph = new BarChartFragment();
                mGraphState = GraphState.BAR_CHART;
//                }
                break;
            case BAR_CHART: // switch from bar chart to live chart
                Log.d(TAG, "switchGraph: switch to live chart");
                mGraph = new LiveChartFragment();
                mGraphState = GraphState.LIVE_VIEW;
                break;
            default:
                Log.e(TAG, "switchGraph: This should not have happened.");
                mGraph = new LiveChartFragment();

                mGraphState = GraphState.LIVE_VIEW;
        }
//        Log.d(TAG, "switchGraph: getBackStackEntryCount " + mFragmentManager.getBackStackEntryCount());
        FragmentTransaction transaction = mFragmentManager.beginTransaction();
        transaction.replace(R.id.fragment_container, mGraph);
        transaction.commit();

        Log.d(TAG, "switchGraph: fm size (after switch): " + mFragmentManager.getBackStackEntryCount());
        boolean autoscaleOn = getPreferences(AppCompatActivity.MODE_PRIVATE).getBoolean("auto_scale_checked", false);
        if((mGraphState == GraphState.LIVE_VIEW) && autoscaleOn){
            setVisibilityZoomButtons(View.GONE);
        } else {
            setVisibilityZoomButtons(View.VISIBLE);
        }
    }

    /**
     * Toggle devices to send sensor values or to stop
     */
    private void toggleSampling() {
        HashMap<String, BluetoothDevice> scanResults = mConnectionManager.getScanResults();
        for(BluetoothDevice device : scanResults.values()) {
            byte[] signal = mAdcEnabled ? SIGNAL_ADC_ON : SIGNAL_ADC_OFF;
            Log.d(TAG, "toggleSampling(): Signal: " + Utilities.bytesToHexString(signal));
            //mConnectionManager.writeCharactersitic(device, SERVICE_UUID, CHARACTERISTIC_UUID, signal);
        }
    }

    @Override
    protected void onResume() {
        super.onResume();

        // End if device has no BLE
        // TODO: End with proper dialog when device has no BLE
        if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
            finish();
        }

        LocalBroadcastManager.getInstance(this).registerReceiver(mMessageReceiver,
                new IntentFilter("ble_data_received"));

        mConnectionManager.startScan();
    }

    @Override
    public void onConnectbuttonPressed(String address) {
        Log.d(TAG, "onConnectbuttonPressed");
       // mConnectionManager.connect(address);

    }

    @Override
    public void appendData(double value, String deviceAddress, String deviceName) {
        Log.d(TAG, "appendData");
        mGraph.appendData(value, 0, 0, deviceAddress, deviceName);
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
    }

    @LayoutRes
    protected int getLayoutResId() {
        return R.layout.activity_masterdetail;
    }

    public void setResetMarkerInterval(int interval){
        mGraph.setmMarkerResetInterval(interval);
    }

    public void setAutoscale(boolean isOn) {
        mGraph.setAutoscale(isOn);

        if(isOn && (mGraphState == GraphState.LIVE_VIEW)) {
            setVisibilityZoomButtons(View.GONE);
        } else {
            setVisibilityZoomButtons(View.VISIBLE);
        }
    }

    /**
     * Hides or shows the zoom buttons.
     * @param visibility View.GONE to hide zoom buttons; View.VISIBLE to show buttons
     */
    public void setVisibilityZoomButtons(int visibility) {
        mZoomInButton.setVisibility(visibility);
        mZoomOutButton.setVisibility(visibility);
    }

    public void setVisible(String address, boolean isVisible) {
        mGraph.setVisible(address, isVisible);
    }

    public void stopScan(){
        mConnectionManager.stopScan();
    }

    public void startScan(){
        mConnectionManager.startScan();
    }
}

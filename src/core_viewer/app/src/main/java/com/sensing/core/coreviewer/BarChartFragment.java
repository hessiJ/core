package com.sensing.core.coreviewer;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.content.LocalBroadcastManager;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.util.TypedValue;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.jjoe64.graphview.DefaultLabelFormatter;
import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.ValueDependentColor;
import com.jjoe64.graphview.helper.StaticLabelsFormatter;
import com.jjoe64.graphview.series.BarGraphSeries;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.DataPointInterface;
import com.jjoe64.graphview.series.OnDataPointTapListener;
import com.jjoe64.graphview.series.PointsGraphSeries;
import com.jjoe64.graphview.series.Series;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;

import static java.lang.Math.abs;


public class BarChartFragment extends GraphFragment {

    private static final double ZOOM_VALUE = 0.3;
    private static final int COLOR_CONNECTED = Color.GREEN;
    private static final int COLOR_STANDARD = Color.BLUE;
    private final int MAX_DATAPOINTS = 40; // maximum of datapoints the graph can hold
    private final boolean SCROLL_TO_END = true;
    private GraphView mGraph;
    private BarGraphSeries<DataPoint> mSeries = new BarGraphSeries<>();
    private PointsGraphSeries<DataPoint> mMaxima = new PointsGraphSeries<>();
    private PointsGraphSeries<DataPoint> mMinima = new PointsGraphSeries<>();
    private HashMap<String, Double> mAddressToMaximum = new HashMap<>();
    private HashMap<String, Double> mAddressToMinimum = new HashMap<>();
    private HashMap<String, Long> mAddressToResetTimeMax = new HashMap<>();
    private HashMap<String, Long> mAddressToResetTimeMin = new HashMap<>();
    private HashMap<String, Double> mAddressToSensorData = new HashMap<>();
    private HashMap<String, Boolean> mAddressToConnectionState = new HashMap<>();
    private HashMap<Integer, Boolean> mIndexToConnetionStatus = new HashMap<>(); // necessary for coloring of bars according to connection

    private int dataPointIndex = 0;
    private ArrayList<String> mLabels = new ArrayList<>();
    private String TAG = "BAR_CHART_FRAGMENT";
    private StaticLabelsFormatter mStaticLabelsFormatter;
    private Float Y_ABS_VALUE;
    private BroadcastReceiver mMessageReceiver;
    private SharedPreferences mPreferences;

    private int mMarkerResetInterval;

    public BarChartFragment() {
        // Required empty public constructor
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d(TAG, "onCreate: ");

        mMessageReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                String address;

                switch(intent.getAction()) {
                    case "BLE_SENSOR_DATA":

                        address = intent.getStringExtra("address");
                        Double value = intent.getExtras().getDouble("value");
                        Double min = intent.getExtras().getDouble("min");
                        Double max = intent.getExtras().getDouble("max");
                        String name = DataManager.getInstance().getName().get(address);

                        appendData(value, min, max, address, name);


                        break;
                    case "connectionState":
                        address = intent.getStringExtra("address");
                        boolean state = intent.getBooleanExtra("state", false);
                        mAddressToConnectionState.put(address, state);
                        Log.d(TAG, "onReceive: received update on connection status for " + address +" -> " + state);
                        break;
                }
            }
        };

        mPreferences = getActivity().getPreferences(AppCompatActivity.MODE_PRIVATE);
        //restore state
        Float defaultValue = 25000f;
        try {
            Y_ABS_VALUE = mPreferences.getFloat("y_abs_value", defaultValue);
        } catch (Exception e) {
            Y_ABS_VALUE = defaultValue; //FIXME
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        Log.d(TAG,"onCreateView:");
        View rootView = inflater.inflate(R.layout.fragment_bar_chart, container, false);

        mGraph = (GraphView) rootView.findViewById(R.id.bar_chart);

        mStaticLabelsFormatter = new StaticLabelsFormatter(mGraph);

//        appendData(-10000, "11:22:33:44:55", "MySensor");
//        appendData(10000, "11:22:33:44:56", "MySensor");
//        appendData(40000, "11:22:33:44:56", "MySensor");
//        appendData(-40000, "11:22:33:44:57", "MySensor");
//        appendData(-20000, "11:22:33:44:58", "MySensor");
//        appendData(30000, "11:22:33:44:59", "MySensor");
//        appendData(40000, "11:22:33:44:60", "MySensor");
//        appendData(40000, "11:22:33:44:34", "MySensor");
//        appendData(40000, "11:22:33:44:35", "MySensor");
//        appendData(40000, "11:22:33:44:36", "MySensor");
//        appendData(40000, "11:22:33:44:37", "MySensor");
//        appendData(40000, "11:22:33:44:38", "MySensor");
//        appendData(40000, "11:22:33:44:39", "MySensor");
//        appendData(40000, "11:22:33:44:30", "MySensor");

        setUpSeries();

        setUpMinMaxMarkers();

        setUpGraphStyle();

        return rootView;
    }

    private List<BluetoothDevice> getConnectedDevices() {
        BluetoothManager bluetoothManager = (BluetoothManager) getActivity().getSystemService(getActivity().BLUETOOTH_SERVICE);
        return bluetoothManager.getConnectedDevices(BluetoothProfile.GATT);
    }

    private void setUpSeries() {

        // set up series for not connected devices
        mSeries.setOnDataPointTapListener(new OnDataPointTapListener() {
            @Override
            public void onTap(Series series, DataPointInterface dataPoint) {
                //Toast.makeText(mGraph.getContext(), "On Data Point clicked: " + dataPoint, Toast.LENGTH_SHORT).show();
                byte[] bytes;// = [0xff, ]

                Bundle bundle = new Bundle();

                DataManager dataManager = DataManager.getInstance();
                List<DeviceData> deviceData;
                double counter = 1.0;

                for(String key: mAddressToSensorData.keySet()){
                    if(counter == dataPoint.getX()){
                        //deviceData = dataManager.getSensorData(key);
                        //bundle.putParcelable("DeviceData", deviceData.get(deviceData.size()-1));
                        bundle.putString("Address", key);
                    }
                    counter++;
                }

                PeripheralDetailsDialogFragment details = PeripheralDetailsDialogFragment.getInstance();
                details.setArguments(bundle);
                if((getFragmentManager().findFragmentByTag(PeripheralDetailsDialogFragment.TAG) == null) && !details.isAdded()) {
                    details.show(getFragmentManager(), PeripheralDetailsDialogFragment.TAG);
                }
            }
        });
        mSeries.setSpacing(30);
        mGraph.addSeries(mSeries);
    }

    private void setUpGraphStyle() {
        float dip = 14f;
        Resources r = getResources();
        float px = TypedValue.applyDimension(
                TypedValue.COMPLEX_UNIT_DIP,
                dip,
                r.getDisplayMetrics()
        );

        mGraph.getGridLabelRenderer().setTextSize(px);

        mGraph.getViewport().setMinX(0d);
        mGraph.getViewport().setMaxX((float)dataPointIndex+1);
        mGraph.getViewport().setXAxisBoundsManual(true);
        mGraph.getViewport().setScrollable(true);

        mGraph.getViewport().setYAxisBoundsManual(true);
        mGraph.getViewport().setMaxY(Y_ABS_VALUE);
        mGraph.getViewport().setMinY(-Y_ABS_VALUE);
        mGraph.getGridLabelRenderer().setNumHorizontalLabels(0);
//        mGraph.getViewport().setScalableY(true);
        // background color needs to be explicitly set for screenshot
        mGraph.setBackgroundColor(Color.WHITE);
        String verticalAxisTitle = mPreferences.getString("verticalAxisTitle", "No Title found");
        mGraph.getGridLabelRenderer().setVerticalAxisTitle(verticalAxisTitle);
    }

    private void setUpMinMaxMarkers() {
        // draw custom shape
        PointsGraphSeries.CustomShape minMaxMarker = new PointsGraphSeries.CustomShape() {
            @Override
            public void draw(Canvas canvas, Paint paint, float x, float y, DataPointInterface dataPoint) {
                paint.setStrokeWidth(3);
                canvas.drawLine(x-40, y, x+40, y, paint);
            }
        };

        mMaxima.setCustomShape(minMaxMarker);
        mMaxima.setSize(5);
        mMaxima.setColor(Color.RED);
        mGraph.addSeries(mMaxima);

        mMinima.setCustomShape(minMaxMarker);
        mMinima.setSize(5);
        mMinima.setColor(Color.RED);
        mGraph.addSeries(mMinima);
    }

    public void setVerticalAxisLabel(String title){
        SharedPreferences.Editor editor = mPreferences.edit();
        editor.putString("verticalAxisTitle", title);
        editor.commit();
        mGraph.getGridLabelRenderer().setVerticalAxisTitle(title);
    }

    public void appendData(double value, double min, double max, String deviceAddress, String deviceName) {

        if(mAddressToSensorData.containsKey(deviceAddress)){
        } else {
            Log.d(TAG,"New device.");
            // Adjust graph width
            mGraph.getViewport().setMinX(0d);
            mGraph.getViewport().setMaxX((float)++dataPointIndex + 1);

            // include new device to maxima
            mAddressToMaximum.put(deviceAddress, max);
            mAddressToMinimum.put(deviceAddress, min);

            // save time
            mAddressToResetTimeMax.put(deviceAddress, System.currentTimeMillis());
            mAddressToResetTimeMin.put(deviceAddress, System.currentTimeMillis());
        }

        // save current value
        mAddressToSensorData.put(deviceAddress, value);

        // Into the following arrays the values of all graphs are copied; the one new value is set
        // appropriately. This is neccessary, because individual values can not be set, only the whole
        // data-series with resetData() (see below).
        DataPoint[] updatedData = new DataPoint[mAddressToSensorData.size()+1];
        DataPoint[] updatedMaxima = new DataPoint[mAddressToSensorData.size()];
        DataPoint[] updatedMinima = new DataPoint[mAddressToSensorData.size()];


        updatedData[0] = new DataPoint(0,0);

        String[] labels = new String[mAddressToSensorData.size()+2];
        // first and last element of labels must be "" to look good
        labels[0] = labels[mAddressToSensorData.size()+1] = "";

        // update maxima if necessary
        if (max > mAddressToMaximum.get(deviceAddress)) {
            mAddressToMaximum.put(deviceAddress, max);
            // reset time for max marker
            mAddressToResetTimeMax.put(deviceAddress, System.currentTimeMillis());
        }
        if (min < mAddressToMinimum.get(deviceAddress)) {
            mAddressToMinimum.put(deviceAddress, min);
            // reset time for max marker
            mAddressToResetTimeMin.put(deviceAddress, System.currentTimeMillis());
        }

        // reset marker because time is up
        if((System.currentTimeMillis() - mAddressToResetTimeMax.get(deviceAddress)) > mMarkerResetInterval){
            mAddressToResetTimeMax.put(deviceAddress, System.currentTimeMillis());
            mAddressToMaximum.put(deviceAddress, value);
        }
        if((System.currentTimeMillis() - mAddressToResetTimeMin.get(deviceAddress)) > mMarkerResetInterval){
            mAddressToResetTimeMin.put(deviceAddress, System.currentTimeMillis());
            mAddressToMinimum.put(deviceAddress, value);
        }

        // copy all values into array
        int xValue = 1;
        for(String address : mAddressToSensorData.keySet()) {
            updatedData[xValue] = new DataPoint(xValue, mAddressToSensorData.get(address));
//            labels[xValue] = address + " \n(" + DataManager.getInstance().getName().get(address) + ")";
            labels[xValue] = DataManager.getInstance().getName().get(address);

            // update connection status
            mIndexToConnetionStatus.put(xValue, isConnected(address));

            updatedMaxima[xValue-1] = new DataPoint(xValue, mAddressToMaximum.get(address));
            updatedMinima[xValue-1] = new DataPoint(xValue, mAddressToMinimum.get(address));

            xValue++;
        }

        mSeries.resetData(updatedData);
        mMaxima.resetData(updatedMaxima);
        mMinima.resetData(updatedMinima);

        // set colors depending on if device connected or not
        mSeries.setValueDependentColor(new ValueDependentColor<DataPoint>() {
            @Override
            public int get(DataPoint data) {
                int value = (int) data.getX();
                if(!mIndexToConnetionStatus.containsKey(value))
                    return COLOR_STANDARD;
                boolean status =  mIndexToConnetionStatus.get(value);
                if(status)
                    return COLOR_CONNECTED;
                else
                    return COLOR_STANDARD;
            }
        });

        NumberFormat nf = NumberFormat.getInstance();
        nf.setMinimumFractionDigits(1);

        // update x-axis labels
        mStaticLabelsFormatter.setHorizontalLabels(labels);
        mStaticLabelsFormatter.setDynamicLabelFormatter(new DefaultLabelFormatter(nf, nf));
        mGraph.getGridLabelRenderer().setLabelFormatter(mStaticLabelsFormatter);
//        mGraph.getGridLabelRenderer().setHorizontalLabelsAngle(45);
//        mGraph.getGridLabelRenderer().setLabelsSpace(30);
        mGraph.getGridLabelRenderer().setLabelHorizontalHeight(37);
    }

    private boolean isConnected(String deviceAddress) {
        if(mAddressToConnectionState.get(deviceAddress) == null)
            return false;
        return mAddressToConnectionState.get(deviceAddress);
    }

    @Override
    public void onPause() {
        super.onPause();
        // persist configuration
        Log.d(TAG, "onPause: Saving state for Y_ABS_VALUE: " + Y_ABS_VALUE);
        SharedPreferences.Editor editor = mPreferences.edit();
        editor.putFloat("y_abs_value", Y_ABS_VALUE);
        editor.putInt("marker_reset_interval", mMarkerResetInterval);
        editor.commit();
    }

    /**
     * Take snapshot of currently shown graph.
     */

    public void takeSnapshot() {
        File rootPath = getActivity().getExternalFilesDir(null);
        if(!rootPath.exists()) {
            rootPath.mkdirs();
        }
        File file = new File(rootPath, "snapshot_" + new Date().toString()+".png");
        try {
            FileOutputStream fOut = new FileOutputStream(file);
            Bitmap snapshot = mGraph.takeSnapshot();
            snapshot.compress(Bitmap.CompressFormat.PNG, 100, fOut);
            fOut.flush(); // Not really required
            fOut.close(); // do not forget to close the stream
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    /**
     * Converts the ArrayList of mLabels to a format that fits into StaticLabelsFormatter.
     * Note: It's size is two elements bigger than mLabels, since the first and last element need to be
     * empty strings.
     * @return The elements of member mLabels in a format that fits into StaticLabelsFormatter.
     */
    private String[] convertLabels() {
        String[] l = new String[mLabels.size()+2];
        l[0] = l[mLabels.size()+1] = ""; //First ans last label have to be set to "" to fit
        for(int i=0; i<mLabels.size(); ++i) {
            l[i+1] = mLabels.get(i);
        }
        return l;
    }

    @Override
    public void onResume() {
        super.onResume();
        Log.d(TAG, "onResume: ");
        LocalBroadcastManager.getInstance(getActivity()).registerReceiver(mMessageReceiver,
                new IntentFilter("BLE_SENSOR_DATA"));
        LocalBroadcastManager.getInstance(getActivity()).registerReceiver(mMessageReceiver,
                new IntentFilter("connectionState"));

        //restore state
        Float defaultValue = 25000f;
        try {
            Y_ABS_VALUE = mPreferences.getFloat("y_abs_value", defaultValue);
        } catch (Exception e) {
            Y_ABS_VALUE = defaultValue; // FIXME:
        }
        mMarkerResetInterval = mPreferences.getInt("marker_reset_interval", 5000);

        // restore color for connected devs
        BluetoothManager bluetoothManager = (BluetoothManager) getActivity().getSystemService(getActivity().BLUETOOTH_SERVICE);
        List<BluetoothDevice> connectedDevices = bluetoothManager.getConnectedDevices(BluetoothProfile.GATT);
        for(BluetoothDevice dev : connectedDevices) {
            mAddressToConnectionState.put(dev.getAddress(), true);
            Log.d(TAG, "onResume: restore connection state for " + dev.getAddress());
        }

        // set zoom buttons visible
        ((MainActivity)getActivity()).setVisibilityZoomButtons(View.VISIBLE);
    }

    /**
     * Zoom in y-axis.
     */
    @Override
    public void zoomIn() {
        Y_ABS_VALUE = (float)(Y_ABS_VALUE*ZOOM_VALUE);
        mGraph.getViewport().setMaxY(Y_ABS_VALUE);
        mGraph.getViewport().setMinY(-Y_ABS_VALUE);
        refresh();
        Log.d(TAG, "zoomIn: Y_ABS_VALUE: " + Y_ABS_VALUE);
    }

    /**
     * Zoom out y-axis
     */
    @Override
    public void zoomOut() {
        Y_ABS_VALUE = (float)(Y_ABS_VALUE/ZOOM_VALUE);
        mGraph.getViewport().setMaxY(Y_ABS_VALUE);
        mGraph.getViewport().setMinY(-Y_ABS_VALUE);
        refresh();
        Log.d(TAG, "zoomOut: Y_ABS_VALUE: " + Y_ABS_VALUE);
    }

    /**
     * Forces the graph to redraw. Otherwise the graph only redraws when new data is inserted.
     */
    private void refresh() {
        mGraph.onDataChanged(false,false);
    }

    public void setmMarkerResetInterval(int mMarkerResetInterval) {
        this.mMarkerResetInterval = mMarkerResetInterval;
        // persist value
        SharedPreferences.Editor editor = mPreferences.edit();
        editor.putInt("marker_reset_interval", mMarkerResetInterval);
        editor.commit();
    }

    @Override
    public void setAutoscale(boolean isOn) {
        // persist value
        SharedPreferences.Editor editor = mPreferences.edit();
        editor.putBoolean("auto_scale_checked", isOn);
        editor.commit();
    }

    @Override
    public void setVisible(String address, boolean isVisible) {
        Log.d(TAG, "setVisible: Ignore - not relevant to this graph type.");
    }
}

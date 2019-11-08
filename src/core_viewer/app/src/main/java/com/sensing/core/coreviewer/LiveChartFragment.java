package com.sensing.core.coreviewer;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.Nullable;
import android.support.v4.content.LocalBroadcastManager;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.LineGraphSeries;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Date;
import java.util.HashMap;
import java.util.Random;


public class LiveChartFragment extends GraphFragment  {

    private SharedPreferences mPreferences;
    private static final int MAX_X_AXIS = 3000;
    private static LiveChartFragment sInstance;
    private final Handler mHandler = new Handler();
    private GraphView mGraph;
    private double graph2LastXValue = 5d;
    private static final String TAG = "LIVE_CHART_FRAGMENT";
    private Random mRand = new Random();
    private float mHue = 0; // mHue value for coloring of graphs
    private int Y_ABS_VALUE;
    private static final double ZOOM_VALUE = 0.3;
    private boolean mAutoscale;

    HashMap<String, LineGraphSeries<DataPoint>> mDataSeries;

    private BroadcastReceiver mMessageReceiver;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mMessageReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                switch(intent.getAction()) {
                    case "BLE_SENSOR_DATA":
                        /*
                        Double value = intent.getDoubleExtra("value", 0);
                        String address = intent.getStringExtra("address");
                        String name = intent.getStringExtra("name");
                        */

                        String address =  intent.getExtras().getString("address");
                        Double value = intent.getExtras().getDouble("value");
                        Double min = intent.getExtras().getDouble("min");
                        Double max = intent.getExtras().getDouble("max");
                        String name = DataManager.getInstance().getName().get(address);

                        appendData(value, min, max, address, name);


                        break;
                }
            }
        };

        mPreferences = getActivity().getPreferences(AppCompatActivity.MODE_PRIVATE);
        //restore state
        restorePersistedState();
    }
    
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        Log.d(TAG,"onCreateView: LiveChartFragment");
        View rootView = inflater.inflate(R.layout.fragment_live_chart, container, false);

        mGraph = (GraphView) rootView.findViewById(R.id.graph);
        mDataSeries = new HashMap<>();
        mGraph.getViewport().setXAxisBoundsManual(true);
        mGraph.getViewport().setMinX(0);
        mGraph.getViewport().setMaxX(1000);
        mGraph.getViewport().setScalable(true);
        mGraph.getViewport().setMaxXAxisSize(MAX_X_AXIS);

        // background color needs to be explicitly set for screenshot
        mGraph.setBackgroundColor(Color.WHITE);

        mGraph.getViewport().setScalableY(false);
        mGraph.getViewport().setYAxisBoundsManual(!mAutoscale);
        mGraph.getViewport().setMaxY(Y_ABS_VALUE);
        mGraph.getViewport().setMinY(-Y_ABS_VALUE);

        mGraph.setBackgroundColor(Color.WHITE);
        String verticalAxisTitle = mPreferences.getString("verticalAxisTitle", "No Title found");
        mGraph.getGridLabelRenderer().setVerticalAxisTitle(verticalAxisTitle);

        return rootView;
    }

    // TODO remove after debug
    private DataPoint[] generateData() {
        int count = 30;
        DataPoint[] values = new DataPoint[count];
        for (int i=0; i<count; i++) {
            double x = i;
            double f = mRand.nextDouble()*0.15+0.3;
            double y = Math.sin(i*f+2) + mRand.nextDouble()*0.3;
            DataPoint v = new DataPoint(x, y);
            values[i] = v;
        }
        return values;
    }

    public void appendData(double value, double min, double max, String deviceAddress, String deviceName) {
        graph2LastXValue += 1d;

        // ignore new values and drop series, because device is hidden.
        Boolean isVisible = DataManager.getInstance().isVisible(deviceAddress);
        if(isVisible!=null && !isVisible) {
            return;
        }

        if(mDataSeries.containsKey(deviceAddress)) {
            Log.d(TAG,"Value: " + value + " from address " + deviceAddress);

            mDataSeries.get(deviceAddress).appendData(new DataPoint(graph2LastXValue, value), true, MAX_X_AXIS);
        } else {
            // device not known yet
            Log.d(TAG,"Value: " + value + " from address " + deviceAddress);
            Log.d(TAG,"New address!");
            mDataSeries.put(deviceAddress, new LineGraphSeries<>());

            mDataSeries.get(deviceAddress).appendData(new DataPoint(graph2LastXValue, value), true, MAX_X_AXIS);
            mDataSeries.get(deviceAddress).setTitle(deviceName);
            float[] hsv = new float[]{mHue, 255, 255};
            mDataSeries.get(deviceAddress).setColor(Color.HSVToColor(hsv));
            mHue = (mHue + 100) % 255;
            mGraph.addSeries(mDataSeries.get(deviceAddress));

            // refresh legend with new found device
            mGraph.getLegendRenderer().setVisible(true); // show legend
//            mGraph.getLegendRenderer().setAlign(LegendRenderer.LegendAlign.TOP);
            mGraph.getLegendRenderer().setFixedPosition(50, 50);

        }
    }

    @Override
    public void onResume() {
        super.onResume();
        LocalBroadcastManager.getInstance(getActivity()).registerReceiver(mMessageReceiver,
                new IntentFilter("BLE_SENSOR_DATA"));
        restorePersistedState();

    }

    private void restorePersistedState() {
        // restore y-axis zoom
        Y_ABS_VALUE = mPreferences.getInt("y_abs_value_live_chart", 2500000);
        Log.d(TAG, "onResume: restore value for Y_ABS_VALUE to: " + Y_ABS_VALUE);
        mAutoscale = mPreferences.getBoolean("auto_scale_checked", false);
    }

    @Override
    public void onPause() {
        Log.d(TAG, "onPause: " + TAG);
        super.onPause();
        LocalBroadcastManager.getInstance(getActivity()).unregisterReceiver(mMessageReceiver);

        // persist configuration
        Log.d(TAG, "onPause: Saving state for Y_ABS_VALUE: " + Y_ABS_VALUE);
        SharedPreferences.Editor editor = mPreferences.edit();
        editor.putInt("y_abs_value_live_chart", Y_ABS_VALUE);
        editor.putBoolean("auto_scale_checked", mAutoscale);
        editor.commit();

    }

//    @Override
//    public void zoomIn() {
//        Log.e(TAG, "zoomIn: Zoom is not enabled in live chart.");
//    }

    /**
     * Zoom in y-axis.
     */
    @Override
    public void zoomIn() {
        Y_ABS_VALUE = Y_ABS_VALUE - (int)(Y_ABS_VALUE*ZOOM_VALUE);
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
        Y_ABS_VALUE = Y_ABS_VALUE + (int)(Y_ABS_VALUE*ZOOM_VALUE);
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

    public void takeSnapshot() {
        File rootPath = getActivity().getExternalFilesDir(null);
        if(!rootPath.exists()) {
            rootPath.mkdirs();
        }
        File file = new File(rootPath, "snapshot_" + new Date().toString()+".png");
        try {
            FileOutputStream fOut = new FileOutputStream(file);
            Bitmap snapshot = mGraph.takeSnapshot();
            snapshot.compress(Bitmap.CompressFormat.JPEG, 85, fOut);
            fOut.flush(); // Not really required
            fOut.close(); // do not forget to close the stream
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    public void setVerticalAxisLabel(String title){
        SharedPreferences.Editor editor = mPreferences.edit();
        editor.putString("verticalAxisTitle", title);
        editor.commit();
        mGraph.getGridLabelRenderer().setVerticalAxisTitle(title);
    }

    /**
     * Applies the change that has been made in the settings dialog, while the bar chart is not on screen.
     * @param interval
     */
    @Override
    public void setmMarkerResetInterval(int interval) {
        // persist value for the barchart-fragment
        SharedPreferences.Editor editor = mPreferences.edit();
        editor.putInt("marker_reset_interval", interval);
        editor.commit();
    }

    @Override
    public void setAutoscale(boolean isOn) {
        mAutoscale = isOn;
        mGraph.getViewport().setScalableY(false);
        mGraph.getViewport().setYAxisBoundsManual(!mAutoscale);
        // persist value
        SharedPreferences.Editor editor = mPreferences.edit();
        editor.putBoolean("auto_scale_checked", mAutoscale);
        editor.commit();
    }

    @Override
    public void setVisible(String address, boolean isVisible) {
        Log.d(TAG, "setVisible: change visibility for address: " + address + " to " + isVisible);
        DataManager.getInstance().setVisible(address, isVisible);
    }
}

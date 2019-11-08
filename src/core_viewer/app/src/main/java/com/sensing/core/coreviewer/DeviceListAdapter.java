package com.sensing.core.coreviewer;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.graphics.Color;
import android.provider.ContactsContract;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.Filter;
import android.widget.Filterable;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class DeviceListAdapter extends RecyclerView.Adapter<DeviceListAdapter.MyViewHolder> implements Filterable {

    private static DeviceListAdapter sInstance;
    private static final String TAG = "DEVICES_LIST_ADAPTER";
    private DataManager dataManager;

    private List<String> mFilteredBleDeviceAddresses = null;
    private ItemFilter mFilter = new ItemFilter();
    private boolean mInitialized = false;
    private MyViewHolder mViewHolder;
    private Map<Integer, Boolean> mIsVisible = new HashMap<>();

    // Provide a reference to the views for each data item
    // Complex data items may need more than one view per item, and
    // you provide access to all the views for a data item in a view holder

    public static DeviceListAdapter getInstance() {
        if(sInstance == null) {
            sInstance = new DeviceListAdapter();
        }
        return sInstance;
    }

    @Override
    public Filter getFilter() {
        return mFilter;
    }

    public void clearAll(Context context){
        mFilteredBleDeviceAddresses.clear();

        Log.d(TAG, "clearAll: " + mFilteredBleDeviceAddresses);
//        //start filtering
        EditText searchField = (EditText) ((MainActivity)context).findViewById(R.id.searchDevices);
        getFilter().filter(searchField.getText());
    }

    public void refreshFiltering() {
        Log.d(TAG, "refreshFiltering: ");
        String s = DevicesFragment.getInstance().getSearchText();
        getFilter().filter(s);
    }

    public static class MyViewHolder extends RecyclerView.ViewHolder {

        public TextView deviceName;
        public ImageView deviceRssi;
        public ImageView deviceBattery;
        public ImageView visibleImage;
        public View layout;


        public MyViewHolder(View v) {
            super(v);
            layout = v;
            deviceName = (TextView)v.findViewById(R.id.name_list);
            deviceRssi = (ImageView) v.findViewById(R.id.rssi_list);
            deviceBattery = (ImageView) v.findViewById(R.id.battery_list);
            visibleImage = (ImageView) v.findViewById(R.id.visible_image);
        }
    }

    // Provide a suitable constructor (depends on the kind of dataset)
    public DeviceListAdapter() {
        Log.d(TAG, "DeviceListAdapter: ctor");
        dataManager = DataManager.getInstance();
        mFilteredBleDeviceAddresses = new ArrayList<>(dataManager.getName().keySet());
    }


    // Create new views (invoked by the layout manager)
    @Override
    public DeviceListAdapter.MyViewHolder onCreateViewHolder(ViewGroup parent,
                                                     int viewType) {
        // create a new view
        View view = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.device_list_view, parent, false);

        mViewHolder = new MyViewHolder(view);
        return mViewHolder;
    }

    // Replace the contents of a view (invoked by the layout manager)
    @Override
    public void onBindViewHolder(MyViewHolder holder, int position) {
        // - get element from your dataset at this position
        // - replace the contents of the view with that element
        String currentBleDeviceAddress = mFilteredBleDeviceAddresses.get(position);
        holder.deviceName.setText(dataManager.getName().get(currentBleDeviceAddress));
        setRssiPicture(holder, currentBleDeviceAddress);
        setBatteryPicture(holder, currentBleDeviceAddress);

        // switch image in list item that indicates whether a device is visible in the graph or not
        ImageView visibleImage = holder.visibleImage;
        if(isVisible(position)){
            visibleImage.setImageResource(R.drawable.visible);
        } else {
            visibleImage.setImageResource(R.drawable.hidden);
        }

        // set background of list item according to connection status of device.
        if(BleGattManager.getInstance().isDeviceConnected(currentBleDeviceAddress, holder.itemView.getContext())) {
            holder.itemView.setBackgroundColor(0xFFDDDDDD);
        } else {
            holder.itemView.setBackgroundColor(Color.WHITE);
        }
    }

    private void setRssiPicture(MyViewHolder holder, String currentBleDeviceAddress){
        ImageView deviceRssi = holder.deviceRssi;
        int rssi = dataManager.getRssi().get(currentBleDeviceAddress);
        if(rssi>-80) {
            deviceRssi.setImageResource(R.drawable.signal_full);
        }else if(rssi<=-80 && rssi>-100){
            deviceRssi.setImageResource(R.drawable.signal_half);
        }else if(rssi<=-100 && rssi>-120){
            deviceRssi.setImageResource(R.drawable.signal_low);
        }else{
            deviceRssi.setImageResource(R.drawable.signal_no);
        }
    }

    private void setBatteryPicture(MyViewHolder holder, String currentBleDeviceAddress){
        ImageView deviceBatteryLvl = holder.deviceBattery;
        int batteryLvl = dataManager.getBatteryLvl().get(currentBleDeviceAddress);
        if(batteryLvl>=803){
            deviceBatteryLvl.setImageResource(R.drawable.battery_full);
        }else if(batteryLvl<=802 && batteryLvl>772){
            deviceBatteryLvl.setImageResource(R.drawable.battery_half);
        }else{
            deviceBatteryLvl.setImageResource(R.drawable.battery_empty);
        }
    }


    public String getAddress(int position){
        return mFilteredBleDeviceAddresses.get(position);
//        return mBleDeviceAddresses.get(position);
    }


    // Return the size of your dataset (invoked by the layout manager)
    @Override
    public int getItemCount() {
        return mFilteredBleDeviceAddresses.size();
//        return dataManager.getName().size();
    }

    public void init() {
        if(!mInitialized) {
            mFilteredBleDeviceAddresses = new ArrayList<>(dataManager.getName().keySet());
        }
    }

    public void toggleVisible(int position) {
        mIsVisible.put(position, !isVisible(position));
    }

    public boolean isVisible(int position) {
        if (mIsVisible.containsKey(position)){
            return mIsVisible.get(position);
        }
        return true;
    }

    private class ItemFilter extends Filter {
        @Override
        protected FilterResults performFiltering(CharSequence constraint) {
            String filterString = constraint.toString().toLowerCase();
            FilterResults results = new FilterResults();
//            final List<String> list = mBleDeviceAddresses;
            Log.d(TAG, "performFiltering: " + DataManager.getInstance().getSensorData().keySet());
            final List<String> list  = new ArrayList<>(dataManager.getSensorData().keySet());


            int count = list.size();

            // if there is no constraint, don't filter
            if (constraint == null || constraint.length() == 0) {
//                results.values = mBleDeviceAddresses;
//                results.count = mBleDeviceAddresses.size();
                results.values = new ArrayList<>(dataManager.getSensorData().keySet());
                results.count = new ArrayList<>(dataManager.getSensorData().keySet()).size();
                return results;
            }
            final ArrayList<String> nlist = new ArrayList<>(count);
            String filterableString;

            for(String address : DataManager.getInstance().getSensorData().keySet()){
                filterableString = DataManager.getInstance().getName().get(address);
                if (filterableString.toLowerCase().contains(filterString)) {
                    nlist.add(address);
                }
            }
            results.values = nlist;
            results.count = nlist.size();

            return results;
        }

        @Override
        protected void publishResults(CharSequence constraint, FilterResults results) {
            mFilteredBleDeviceAddresses = ((List<String>)results.values);

            Log.d(TAG, "publishResults: filtered" + mFilteredBleDeviceAddresses);

            List<BluetoothDevice> devices =  BleGattManager.getInstance().getConnectedBleDevices();
            //addAll connected
            for(BluetoothDevice dev : devices) {
                mFilteredBleDeviceAddresses.add(dev.getAddress());
            }

            mInitialized = true;
            notifyDataSetChanged();
        }
    }

}

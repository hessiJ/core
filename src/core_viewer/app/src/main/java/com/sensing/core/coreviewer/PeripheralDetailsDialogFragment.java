package com.sensing.core.coreviewer;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.support.v4.app.DialogFragment;
import android.support.v7.app.AlertDialog;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import java.util.Map;

public class PeripheralDetailsDialogFragment extends DialogFragment {

    public static final String TAG = "PERI_DETAILS_DIALOG";
    private static PeripheralDetailsDialogFragment sInstance;

    private boolean mIsDeviceConnected;
    private String mSelectedAddress;

    private View mPer_frag_view;

    private Button mConnectDisconnectButton;
    private Button mCancelButton;
    private Button mApplyChangesButton;

    private TextView mChangeDeviceNameTextView;
    private TextView mChangeCalibrationValue;
    private TextView mNameTxtView;
    private TextView mAddressTxtView;
    private TextView mBatteryValueTxtView;
    private TextView mRssiTxtView;
    private TextView mFirmwareTxtView;

    private Spinner mChangePropertiesSpinner;
    private Spinner mChangeCalibrationUnitSpinner;

    private boolean isActive = false;


    public boolean isActive(){
        return isActive;
    }
    @Override
    public void onDestroy(){
        super.onDestroy();
        Log.d(TAG, "onDestroy: ");
        isActive = false;
    }

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        isActive = true;

        pullAddressAndConnectionStatus();
        AlertDialog.Builder builder = inflateCustomDialog();

        initializeTextViews();
        initializeSpinner();

        setUpButtons();
        enableOrDisableEntries();

        AlertDialog dialog = builder.create();
        return dialog;
    }

    private void setUpButtons() {
        initializeApplyChangesButton();
        initializeDisconnectButton();
        initializeCancelButton();

    }

    private void pullAddressAndConnectionStatus(){
        Bundle arg = getArguments();
        mSelectedAddress = arg.getString("Address");
        mIsDeviceConnected = BleGattManager.getInstance().isDeviceConnected(mSelectedAddress, getActivity());
    }

    private AlertDialog.Builder inflateCustomDialog(){
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        LayoutInflater inflater = requireActivity().getLayoutInflater();
        mPer_frag_view = inflater.inflate(R.layout.peripherals_details_dialog, null);
        builder.setView(mPer_frag_view);
        return builder;
    }

    private void initializeTextViews(){

        mNameTxtView = mPer_frag_view.findViewById(R.id.peripheral_name_val);
        mNameTxtView.setText(DataManager.getInstance().getName().get(mSelectedAddress));

        mAddressTxtView = mPer_frag_view.findViewById(R.id.address_value);
        mAddressTxtView.setText(mSelectedAddress);
        //Log.d(TAG, "onCreateDialog: " + deviceData.getAdcDataAdvertisement());

        mBatteryValueTxtView = mPer_frag_view.findViewById(R.id.battery_val);
        mBatteryValueTxtView.setText(DataManager.getInstance().getBatteryLvl().get(mSelectedAddress) + "");

        mRssiTxtView = mPer_frag_view.findViewById(R.id.signal_val);
        mRssiTxtView.setText(DataManager.getInstance().getRssi().get(mSelectedAddress) + "dBm");

        mFirmwareTxtView = mPer_frag_view.findViewById(R.id.firmware_value);
        mFirmwareTxtView.setText(DataManager.getInstance().getRevisionLvl().get(mSelectedAddress).toString());

        mChangeCalibrationValue = mPer_frag_view.findViewById(R.id.change_calibration_value);
        mChangeCalibrationValue.setHint(DataManager.getInstance().getCalibrationValue().get(mSelectedAddress) + "");
        mChangeCalibrationValue.setVisibility(View.GONE);

        mChangeDeviceNameTextView = mPer_frag_view.findViewById(R.id.change_name);
        mChangeDeviceNameTextView.setHint(DataManager.getInstance().getName().get(mSelectedAddress));
        mChangeDeviceNameTextView.setVisibility(View.VISIBLE);

    }

    private void initializeSpinner(){

        mChangePropertiesSpinner
                = mPer_frag_view.findViewById(R.id.change_properties);
        ArrayAdapter<CharSequence> adapterProperties = ArrayAdapter.createFromResource(
                PeripheralDetailsDialogFragment.getInstance().getContext(),
                R.array.change_properties,
                android.R.layout.simple_spinner_dropdown_item
        );
        adapterProperties.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mChangePropertiesSpinner.setAdapter(adapterProperties);

        mChangePropertiesSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parentView, View selectedItemView, int position, long id) {
                // your code here
                Log.d(TAG, "onItemSelected: " + mChangePropertiesSpinner.getSelectedItem());
                showSelection(mChangePropertiesSpinner.getSelectedItem().toString());
            }

            @Override
            public void onNothingSelected(AdapterView<?> parentView) {
                // your code here
                Log.d(TAG, "onItemSelected: " + mChangePropertiesSpinner.getSelectedItem());
            }
        });


        mChangeCalibrationUnitSpinner
                = mPer_frag_view.findViewById(R.id.change_calibration_unit);
        ArrayAdapter<CharSequence> adapterCalibrationUnits = ArrayAdapter.createFromResource(
                PeripheralDetailsDialogFragment.getInstance().getContext(),
                R.array.calibration_values_array,
                android.R.layout.simple_spinner_dropdown_item
        );
        adapterCalibrationUnits.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mChangeCalibrationUnitSpinner.setAdapter(adapterCalibrationUnits);
        mChangeCalibrationUnitSpinner.setVisibility(View.GONE);

    }


    private void showSelection(String selectedItem){
        switch (selectedItem){
            case "Calibration unit":
                mChangeCalibrationUnitSpinner.setVisibility(View.VISIBLE);
                mChangeDeviceNameTextView.setVisibility(View.GONE);
                mChangeCalibrationValue.setVisibility(View.GONE);
                break;

            case "Calibration value":
                mChangeCalibrationUnitSpinner.setVisibility(View.GONE);
                mChangeDeviceNameTextView.setVisibility(View.GONE);
                mChangeCalibrationValue.setVisibility(View.VISIBLE);
                break;

            case "Name":
                mChangeCalibrationUnitSpinner.setVisibility(View.GONE);
                mChangeDeviceNameTextView.setVisibility(View.VISIBLE);
                mChangeCalibrationValue.setVisibility(View.GONE);
                break;
            case "Reset device":
                mChangeCalibrationUnitSpinner.setVisibility(View.GONE);
                mChangeDeviceNameTextView.setVisibility(View.GONE);
                mChangeCalibrationValue.setVisibility(View.GONE);
                break;
            default:

        }
    }

    private void initializeApplyChangesButton(){
        mApplyChangesButton = mPer_frag_view.findViewById(R.id.applyChangesButton);
        applyChangesVisibleIfConnected();
        mApplyChangesButton.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                commitChanges();
            }
        });
    }

    private void applyChangesVisibleIfConnected(){
        if(mIsDeviceConnected) {
            mApplyChangesButton.setVisibility(View.VISIBLE);
            mApplyChangesButton.setEnabled(true);
        }
        else{
            mApplyChangesButton.setEnabled(false);
            mApplyChangesButton.setVisibility(View.GONE);

        }
    }

    private void commitChanges(){

        switch (mChangePropertiesSpinner.getSelectedItem().toString()) {
            case "Calibration unit":
                changeCalibrationUnit();
                break;
            case "Calibration value":
                changeCalibrationValue();
                break;
            case "Name":
                changeDeviceName();
                break;
            case "Reset device":
                resetDevice();
                break;
            default:
        }

        resetDevice();
        getDialog().cancel();
    }

    private void changeDeviceName(){
        if(isDataInChangeDeviceName()) {
            Log.d(TAG, "onClick: " + BleGattManager.getInstance().setPeripheralName(mSelectedAddress, mChangeDeviceNameTextView.getText().toString()));
        }
    }

    private boolean isDataInChangeDeviceName(){
        return !mChangeDeviceNameTextView.getText().toString().isEmpty();
    }

    private void changeCalibrationValue(){
        if(isDataInCalibrationField()) {
            float floatOfString = Float.valueOf(mChangeCalibrationValue.getText().toString());
            Log.d(TAG, "FloatOfString: " + floatOfString);
            DataManager.getInstance().setCalibrationValue(mSelectedAddress, floatOfString);
            BleGattManager.getInstance().setCalibrationValue(mSelectedAddress, floatOfString);
        }
    }

    boolean isDataInCalibrationField(){
        return !mChangeCalibrationValue.getText().toString().isEmpty();
    }

    private void changeCalibrationUnit(){
        if(isCalibrationUnitSelected()){
            String selectedItem = mChangeCalibrationUnitSpinner.getSelectedItem().toString();
            int item2Number = convertSpinnerItem2Number(selectedItem);
            DataManager.getInstance().setCalibrationUnit(mSelectedAddress, selectedItem);
            BleGattManager.getInstance().setCalibrationUnit(mSelectedAddress, (byte)item2Number);
            ((MainActivity)getActivity()).mGraph.setVerticalAxisLabel(selectedItem);
        }
    }



    boolean isCalibrationUnitSelected(){
        return !mChangeCalibrationUnitSpinner.getSelectedItem().toString().equals("");
    }


    private void resetDevice(){
        if(isResetDeviceSelected()){
            Toast.makeText(getActivity(), "Reset device with address " + mSelectedAddress, Toast.LENGTH_SHORT).show();
            BleGattManager.getInstance().resetDevice(mSelectedAddress);
        }
    }


    boolean isResetDeviceSelected(){
        return mChangePropertiesSpinner.getSelectedItem().toString().equals("Reset device");
    }

    private int convertSpinnerItem2Number(String selectedItem){
        Map<String, Integer> availableCalibrationUnits = DataManager.getInstance().getAvailableCalibrationUnits();
        int item2Number =  availableCalibrationUnits.get(selectedItem);
        return item2Number;
    }

    private void initializeDisconnectButton(){
        mConnectDisconnectButton = mPer_frag_view.findViewById(R.id.connectButton);
        setDisconnectConnectText();
        mConnectDisconnectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                toggleConnection();
            }
        });
    }

    private void setDisconnectConnectText(){
        if(mIsDeviceConnected) {
            mConnectDisconnectButton.setText(R.string.disconnect);
        }
        else{
            mConnectDisconnectButton.setText(R.string.connect);

        }
    }

    private void toggleConnection(){
        if(mIsDeviceConnected) {
            ((MainActivity) getActivity()).bleDisconnect(mSelectedAddress);
            mConnectDisconnectButton.setText(R.string.disconnect);
            getDialog().cancel();
        }
        else{
            ((MainActivity) getActivity()).bleConnect(mSelectedAddress);
            mConnectDisconnectButton.setText(R.string.connect);
            getDialog().cancel();
        }
    }

    private void initializeCancelButton(){
        mCancelButton = mPer_frag_view.findViewById(R.id.cancelButton);
        mCancelButton.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                getDialog().cancel();
            }
        });
    }

    private void enableOrDisableEntries(){
        if(mIsDeviceConnected){
            mChangeDeviceNameTextView.setEnabled(true);
            mChangeCalibrationValue.setEnabled(true);
            mChangePropertiesSpinner.setEnabled(true);
            mChangeCalibrationUnitSpinner.setEnabled(true);

        }else{
            mChangeDeviceNameTextView.setEnabled(false);
            mChangeCalibrationValue.setEnabled(false);
            mChangeCalibrationUnitSpinner.setEnabled(false);
            //mChangePropertiesSpinner.setEnabled(false);
        }
    }

    public static PeripheralDetailsDialogFragment getInstance() {
        if(sInstance == null) {
            sInstance = new PeripheralDetailsDialogFragment();
        }
        return sInstance;
    }


    public void updateRssiBattValue(){
        if(mPer_frag_view != null) {
            TextView per_rssi = mPer_frag_view.findViewById(R.id.signal_val);
            per_rssi.setText(DataManager.getInstance().getRssi().get(mSelectedAddress) + "dBm");

            TextView per_batt_val = mPer_frag_view.findViewById(R.id.battery_val);
            per_batt_val.setText(DataManager.getInstance().getBatteryLvl().get(mSelectedAddress) + "");
        }

    }

    public void updateSensorValue(String address, double sensorValue){
        if((mPer_frag_view != null) && (address.equals(mSelectedAddress))) {
            Log.d(TAG, "updateSensorValue: " + sensorValue + "Address: " + address);
            TextView adc_value = mPer_frag_view.findViewById(R.id.sensor_value);
//            DecimalFormat df = new DecimalFormat("#.#####");
//            df.setRoundingMode(RoundingMode.UNNECESSARY);
//            String roundedValue = df.format(sensorValue);
            String roundedValue = String.format("%.5g%n", sensorValue);
            adc_value.setText(roundedValue + "");
        }
    }

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
    }

    public interface PeripheralDialogFragmentListener {

        public void onConnectbuttonPressed(String address);
    }

}

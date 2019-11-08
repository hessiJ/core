package com.sensing.core.coreviewer;

import android.app.Dialog;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.support.v4.app.DialogFragment;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;

import java.lang.reflect.Array;


public class SettingsFragmentDialog extends DialogFragment {

    private static final String TAG = "SETTINGS_FRAGMENT";

    private View per_frag_view;

    private static SettingsFragmentDialog sInstance;
    MainActivity mParent;
    private EditText mMinMaxResetIntervalEditText;
    private SharedPreferences mPreferences;
    private CheckBox mAutoscaleCheckBox;

    public static SettingsFragmentDialog getInstance() {
        if(sInstance == null) {
            sInstance = new SettingsFragmentDialog();
        }
        return sInstance;
    }

    @Override
    public void onStart() {
        super.onStart();
        Log.d(TAG, "onStart: " + BleGattManager.getInstance().getmLogFilename());
    }

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        Dialog fragmentDialog = new Dialog(getContext());

        Log.d(TAG, "onCreateDialog:");
        Bundle arg = getArguments();
        mParent = (MainActivity)getActivity();

        mPreferences = mParent.getPreferences(AppCompatActivity.MODE_PRIVATE);
        Button disconnectAllButton;



        // Use the Builder class for convenient dialog construction
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());

        // Get the layout inflater
        LayoutInflater inflater = requireActivity().getLayoutInflater();
        per_frag_view = inflater.inflate(R.layout.fragment_settings_fragment_dialog, null);

        builder.setView(per_frag_view);

        disconnectAllButton = per_frag_view.findViewById(R.id.disconnectaAll);
        disconnectAllButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                BleGattManager.getInstance().disconnectAllBle();
            }
        });

        mMinMaxResetIntervalEditText = (EditText) per_frag_view.findViewById(R.id.reset_interval_value);
        // show hint in text field with current value
        int value = mPreferences.getInt("marker_reset_interval", 5000);
        mMinMaxResetIntervalEditText.setHint(Integer.toString(value));

        mAutoscaleCheckBox = (CheckBox) per_frag_view.findViewById(R.id.autoscale_checkbox);
        // set up check box to activate or deactivate autoscale and vertical gesture zooming.
        boolean autoscaleChecked = mPreferences.getBoolean("auto_scale_checked", false);
        mAutoscaleCheckBox.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                Log.d(TAG, "onCheckedChanged: to " + isChecked);
                mParent.setAutoscale(isChecked);
            }
        });
        mAutoscaleCheckBox.setChecked(autoscaleChecked);

        AlertDialog dialog = builder.create();
        return dialog;
    }

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
    }

    public interface Listener {

        public void setResetMarkerInterval(int interval);
    }

    @Override
    public void onStop() {
        super.onStop();

        // send values of input fields to parent
        // send value for reset interval
        int interval;
        String sVal = mMinMaxResetIntervalEditText.getText().toString();
        if(sVal.length() > 0) { // ignore empty field
            interval = Integer.valueOf(mMinMaxResetIntervalEditText.getText().toString());
            mParent.setResetMarkerInterval(interval);
        }
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        Log.d(TAG, "onDestroyView: ");

    }
}

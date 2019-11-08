package com.sensing.core.coreviewer;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.widget.SwipeRefreshLayout;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.EditorInfo;
import android.widget.EditText;
import android.widget.Toast;


public class DevicesFragment extends Fragment {

    private static DevicesFragment sInstance;
    private static final String TAG = "DEVICES_FRAGMENT";
    private RecyclerView mRecyclerView;
    private RecyclerView.LayoutManager mLayoutManager;
    private DeviceListAdapter mAdapter;
    private EditText mSearchField;
    private EditText mDeviceSearchEditText;
    private SwipeRefreshLayout mSwipeLayout;





    public static DevicesFragment getInstance() {
        if(sInstance == null) {
            sInstance = new DevicesFragment();
        }
        return sInstance;
    }

    private void refreshAction(){
//        ((MainActivity)getActivity()).stopScan();
        DataManager.getInstance().clearAll();
        DeviceListAdapter.getInstance().clearAll(getActivity());
//        ((MainActivity)getActivity()).startScan();
//        DeviceListAdapter.getInstance().notifyDataSetChanged();

        mSwipeLayout.setRefreshing(false);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        View view = inflater.inflate(R.layout.fragment_devices, container, false);

        mRecyclerView = (RecyclerView) view.findViewById(R.id.devicesView);
        mSwipeLayout = (SwipeRefreshLayout) view.findViewById(R.id.swiperefresh);

        mSwipeLayout.setOnRefreshListener(new SwipeRefreshLayout.OnRefreshListener() {
            @Override
            public void onRefresh() {
                refreshAction();
            }
        });


        // use this setting to improve performance if you know that changes
        // in content do not change the layout size of the RecyclerView
        //mRecyclerView.setHasFixedSize(true);




        mLayoutManager = new LinearLayoutManager(this.getContext());

        mRecyclerView.addOnItemTouchListener(
                new RecyclerItemClickListener(this.getContext(), view.findViewById(R.id.devicesView) ,new RecyclerItemClickListener.OnItemClickListener() {
                    @Override public void onItemClick(View view, int position) {

                        Log.d(TAG, "addOnItemTouchListener: " + position);
                        if(isLayoutReady(position)) {
                            Bundle bundle = new Bundle();
                            PeripheralDetailsDialogFragment details = PeripheralDetailsDialogFragment.getInstance();
                            bundle.putString("Address", DeviceListAdapter.getInstance().getAddress(position));
                            details.setArguments(bundle);
                            if((getFragmentManager().findFragmentByTag(PeripheralDetailsDialogFragment.TAG) == null) && !details.isAdded()) {
                                details.show(getFragmentManager(), PeripheralDetailsDialogFragment.TAG);
                            }
                        }
                    }

                    @Override public void onLongItemClick(View view, int position) {
                        if(isLayoutReady(position)) {
                            Log.d(TAG, "onLongItemClick: ");
                            mAdapter.toggleVisible(position);
                            String address = mAdapter.getAddress(position);
                            ((MainActivity) getActivity()).setVisible(address, mAdapter.isVisible(position));
                            DeviceListAdapter.getInstance().notifyItemChanged(position);
                        }else{
                            Log.d(TAG, "onLongItemClick: Dropped position -1");
                            return;
                        }
                    }
                })
        );

        mRecyclerView.setLayoutManager(mLayoutManager);
        mAdapter = DeviceListAdapter.getInstance();
        mRecyclerView.setAdapter(mAdapter);
        // Inflate the layout for this fragment

        // configure search field
        configureSearch(view);
        return view;
    }

    private Boolean isLayoutReady(int position){
        if(position == -1){
            return false;
        }else{
            return true;
        }

    }

    private void configureSearch(View view) {
        mDeviceSearchEditText = (EditText) view.findViewById(R.id.searchDevices);
        mDeviceSearchEditText.setImeOptions(EditorInfo.IME_FLAG_NO_EXTRACT_UI | EditorInfo.IME_ACTION_SEARCH);
        mDeviceSearchEditText.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {
            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                DeviceListAdapter.getInstance().getFilter().filter(s);
            }

            @Override
            public void afterTextChanged(Editable s) {
                Log.d(TAG, "afterTextChanged: ");
            }
        });
    }

    public String getSearchText() {
        if(mDeviceSearchEditText == null)
            return "";
        return mDeviceSearchEditText.getText().toString();
    }
}

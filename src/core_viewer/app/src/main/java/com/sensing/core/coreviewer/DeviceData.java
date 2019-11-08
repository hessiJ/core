package com.sensing.core.coreviewer;

import android.os.Parcel;
import android.os.Parcelable;

import java.util.ArrayList;

/**
 * Utility class to de-serialize manufacturer specific BLE-data
 */
public class DeviceData implements Parcelable {
    private byte[] mRawData;

    public DeviceData(byte[] rawData) {
        mRawData = rawData;
    }


    protected DeviceData(Parcel in) {
        mRawData = in.createByteArray();
    }

    public static final Creator<DeviceData> CREATOR = new Creator<DeviceData>() {
        @Override
        public DeviceData createFromParcel(Parcel in) {
            return new DeviceData(in);
        }

        @Override
        public DeviceData[] newArray(int size) {
            return new DeviceData[size];
        }
    };


    public static ArrayList<Double> getAdcDataNotification(byte[] rawData, double calibrationValue) {
        ArrayList<Double> temp = new ArrayList<>();
        temp.add(SampleParser.threeByte2intAdcCoreStack(rawData[3], rawData[2], rawData[1])*calibrationValue);
        temp.add(SampleParser.threeByte2intAdcCoreStack(rawData[6], rawData[5], rawData[4])*calibrationValue);
        temp.add(SampleParser.threeByte2intAdcCoreStack(rawData[9], rawData[8], rawData[7])*calibrationValue);
        temp.add(SampleParser.threeByte2intAdcCoreStack(rawData[12], rawData[11], rawData[10])*calibrationValue);
        temp.add(SampleParser.threeByte2intAdcCoreStack(rawData[15], rawData[14], rawData[13])*calibrationValue);
        temp.add(SampleParser.threeByte2intAdcCoreStack(rawData[18], rawData[17], rawData[16 ])*calibrationValue);
        return temp;
    }

    public static ArrayList<Double> getAdcDataAdvertisement(byte[] rawData, double calibrationValue) {
        ArrayList<Double> temp = new ArrayList<>();
        byte dummyByte = 0x00;
        temp.add(SampleParser.threeByte2intAdcCoreStack(rawData[11], rawData[10], rawData[9]) * calibrationValue);
        try {
            temp.add(SampleParser.threeByte2intAdcCoreStack(dummyByte, rawData[13], rawData[12]) * calibrationValue);
            temp.add(SampleParser.threeByte2intAdcCoreStack(dummyByte, rawData[15], rawData[14]) * calibrationValue);
        }catch(Exception e){
            temp.add(0.0);
            temp.add(0.0);
        }
        return temp;
    }

    public static byte getCalibration(byte[] rawData) {
        return  rawData[0];
    }

    public static int getCalibrationValue(byte[] rawData) {
        return SampleParser.fourByte2intAdcCoreStack(rawData[4], rawData[3], rawData[2],rawData[1]);
    }

    public static int getBatteryLevel(byte[] rawData) {
        return SampleParser.twoByte2int(rawData[6], rawData[5]);
    }


    public byte[] getmRawData() {
        return mRawData;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByteArray(mRawData);
    }
}

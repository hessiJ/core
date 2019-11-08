package com.sensing.core.coreviewer;

import android.util.Log;

import java.util.ArrayList;


/**
 * Created by Arthur Buchta on 28.10.2018.
 */

public class SampleParser {
    private double accelScale = 0.00239501953124;
    private double magScale = 0.1;
    private double adcScale = 1;


    private String deviceAndValue;
    private RingBuffer ringBuffer = new RingBuffer(10);


    private ArrayList<Double> auxDataList;
    private ArrayList<Double> adcDataList = new ArrayList<>();

    private double adcOffset = 0;
    private Double calibVal = 0.0033;

    public SampleParser(String deviceAndValue){
        this.deviceAndValue = deviceAndValue;
    }

    public ArrayList<Double> parseAuxData(byte[] auxData){
        ArrayList<Double> returnList = new ArrayList<>();

        //accel x
        returnList.add(twoByte2int(auxData[3], auxData[2])*accelScale);
        //accel y
        returnList.add(twoByte2int(auxData[5], auxData[4])*accelScale);
        //accel z
        returnList.add(twoByte2int(auxData[7], auxData[6])*accelScale);
        //magn x
        returnList.add(twoByte2int(auxData[9], auxData[8])*magScale);
        //magn y
        returnList.add(twoByte2int(auxData[11], auxData[10])*magScale);
        //magn z
        returnList.add(twoByte2int(auxData[13], auxData[12])*magScale);

        returnList.add((int)auxData[14]*1.0);

        auxDataList = returnList;
        return returnList;
    }

    public ArrayList<Double> parseAdcData(byte[] adcData){
        ArrayList<Double> returnList = new ArrayList<>();
        //Log.e("parseAdcData", "Higher Byte: " + ((adcData[3]>>7) & 0x1));
        returnList.add((twoByte2intAdc(adcData[4], adcData[3])*adcScale - adcOffset)*calibVal);
        Log.e("parseADc", "data " + returnList);

        adcDataList = returnList;
        ringBuffer.put(returnList.get(0));
        return returnList;
    }

    public void setOffset(){
        //only set offset if values available
        if(adcDataList.size()>0) {
            Log.e("setOffset", "adc Offset: " + ringBuffer.getAverage());
            this.adcOffset += (ringBuffer.getAverage())/calibVal;
        }
    }

    public void setCalibVal(Double calibValue){
        Log.e("SetCalibVal", "Set Calib Val: " + calibValue);
        this.calibVal = calibValue;
    }

    static Integer twoByte2int(byte byteLower, byte byteHigher){

             return (byteLower & 0xff) | (byteHigher << 8);
    }

    static Integer twoByte2intAdc(byte byteLower, byte byteHigher){

        if(((byteHigher>>7) & 0x1)==0){
            return ((((byteLower & 0xff) | (byteHigher << 8))) -  65536);
        }else{
            return -1*(~(((byteLower & 0xff) | (byteHigher << 8))-1));
        }
    }

    public static Integer fourByte2intAdcCoreStack(byte one, byte two, byte three, byte four){
        return (one & 0xff) | ((two << 8) & 0xffff) | ((three << 16) & 0xffffff)| ((four << 24) & 0xffffffff);
    }

    public static Integer threeByte2intAdcCoreStack(byte one, byte two, byte three){
        int value =  ((one & 0xff) | ((two << 8) & 0xffff) | ((three << 16) & 0xffffff));
        value = value << 8;
        return value>>8;
    }

}

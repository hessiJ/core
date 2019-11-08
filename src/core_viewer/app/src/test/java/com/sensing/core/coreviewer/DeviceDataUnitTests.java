package com.sensing.core.coreviewer;

import org.junit.Test;

import static org.junit.Assert.*;

public class DeviceDataUnitTests {

    //0x026666C842036914FF00003A
    byte[] rawData = { 2, 102, 102, -56, 66, 3, 105, 20, -1, 0, 0, 58 };
    double delta = 0.00001;

    @Test
    public void getAdcDataAdvertisementTest() {
        int expected = -27279;
//        double actual = DeviceData.getAdcDataAdvertisement(rawData).get(0);
        int actual = SampleParser.threeByte2intAdcCoreStack((byte) 0x71, (byte)0x95, (byte)0xff);


        assertEquals(expected, actual);
    }
//
//    @Test
//    public void getBatteryLevel() {
//        int expected = 0x0340;
//        int actual = DeviceData.getBatteryLevel(rawData);
//        assertEquals(expected, actual);
//    }
//
//    @Test
//    public void getAdcData() {
//        int expected = 0xFFEABE;
//        int actual = DeviceData.getAdcData(rawData);
//        assertEquals(expected, actual);
//    }
//
//    DeviceData myTestData = new DeviceData(rawData);
//
//    @Test
//    public void getRevisionLevel_nonPatameterized() {
//        int expected = 0x3517;
//        int actual = myTestData.getRevisionLevel();
//        assertEquals(expected, actual);
//    }
//
//    @Test
//    public void getBatteryLevel_nonPatameterized() {
//        int expected = 0x0340;
//        int actual = myTestData.getBatteryLevel();
//        assertEquals(expected, actual);
//    }
//
//    @Test
//    public void getAdcData_nonPatameterized() {
//        int expected = 0xFFEABE;
//        int actual = myTestData.getAdcData();
//        assertEquals(expected, actual);
//    }

}

package com.sensing.core.coreviewer;

import org.apache.commons.collections.buffer.CircularFifoBuffer;
import org.junit.Before;
import org.junit.Test;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import static org.junit.Assert.*;

public class DataManagerTest {

    //        ArrayList<Integer> myData = new ArrayList<>(3);
    private CircularFifoBuffer myBuffer = new CircularFifoBuffer(3);
    @Before
    public void setUpForBufferTest() {
        myBuffer.add(1);
        myBuffer.add(2);
        myBuffer.add(3);
        myBuffer.add(4);
    }

    @Before
    public void setUp() {

    }

    @Test
    public void bufferTestGetCorrectElement() {

        int expected = 2;
        int actual = (int) myBuffer.get();
        assertEquals(expected, actual);
    }

    @Test
    public void bufferIterate() {
        int i;

        Iterator iter = myBuffer.iterator();

        int expect = 2;
        int actual = (int) iter.next();
        assertEquals(expect, actual);

        expect = 3;
        actual = (int) iter.next();
        assertEquals(expect, actual);

        expect = 4;
        actual = (int) iter.next();
        assertEquals(expect, actual);
    }

    @Test
    public void bufferTestRetainCorrectSize() {
        myBuffer.add(1);
        myBuffer.add(2);
        myBuffer.add(3);
        myBuffer.add(4);

        int expected = 3;
        int actual = myBuffer.size();
        assertEquals(expected, actual);
    }

    @Test
    public void bufferTestNonPrimitiveType() {
        CircularFifoBuffer myBuffer2 = new CircularFifoBuffer(3);
        ArrayList<Integer> intArray = new ArrayList<>();
        intArray.add(1);
        intArray.add(5);
        myBuffer2.add(intArray);

        ArrayList<Integer> containedArray = (ArrayList<Integer>) myBuffer2.get();
        int expected = 2;
        int actual = containedArray.size();
        assertEquals(expected, actual);

        expected = 1;
        actual = containedArray.get(0);
        assertEquals(expected, actual);

        expected = 5;
        actual = containedArray.get(1);
        assertEquals(expected, actual);
    }

    @Test
    public void getInstanceTestIsSingleton() {
//        Context dummyContext = new MockContext();
        DataManager myManager = DataManager.getInstance();
        DataManager myOtherManager = DataManager.getInstance();

        // Test if hashcodes match, which they should for a singleton
        // firstly add some data to make sure Hashcode differs
        byte[] rawData = { 83, 111, 109, 53, 23, 3, 64, -1, -22, -66 };
        myManager.add("11:22:33:44:55", new DeviceData(rawData));
        int expected = myManager.hashCode();
        int actual = myOtherManager.hashCode();
        assertEquals(expected, actual);

//        // make sure if this test makes sense with hashcode with other class
//        ArrayList<Integer> myList = new ArrayList<>();
//        myList.add(1);
//        ArrayList<Integer> myOtherList = new ArrayList<>();
//        myOtherList.add(2);
//        expected = myList.hashCode();
//        actual = myOtherList.hashCode();
//        assertNotEquals(expected, actual);
    }

   @Test
   public void getDeviceData() {
       DataManager myManager = DataManager.getInstance();
       String address = "11:22:33:44:55";
       // Test if hashcodes match, which they should for a singleton
       // firstly add some data to make sure Hashcode differs

       byte[] rawData = { 83, 111, 109, 53, 23, 3, 64, -1, -22, -66 };
       myManager.add(address, new DeviceData(rawData));

       int size = myManager.getCircularPufferSize(address);
       assertEquals(1,size);

       byte[] rawData2 = { 93, 111, 109, 53, 23, 3, 64, -1, -22, -66 };
       myManager.add(address, new DeviceData(rawData2));
       size = myManager.getCircularPufferSize(address);
       assertEquals(2,size);

       int size2 = myManager.getCircularPufferSize(address);
       List<DeviceData> deviceData = myManager.getSensorData(address);

       assertArrayEquals(rawData, deviceData.get(0).getmRawData());
       assertArrayEquals(rawData2, deviceData.get(1).getmRawData());
   }


    /**
     * Test adding and getting. This is in one single test because serperate tests would be just
     * redundant without Dependency Injection.
     * TODO: Use Dependency Injection
     */
    @Test
    public void addAndGet() {
        DataManager myManager = DataManager.getInstance();
        byte[] rawData = { 83, 111, 109, 53, 23, 3, 64, -1, -22, -66 };
        myManager.add("11:22:33:44:55", new DeviceData(rawData));

        List<DeviceData> obtainedData = myManager.getSensorData("11:22:33:44:55");
        byte expected = 83;
        byte actual = obtainedData.get(0).getmRawData()[0];
        assertEquals(expected, actual);

        expected = -66;
        actual = obtainedData.get(0).getmRawData()[9];
        assertEquals(expected, actual);
    }
}
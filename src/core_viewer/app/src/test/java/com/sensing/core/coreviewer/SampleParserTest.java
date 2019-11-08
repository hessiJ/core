package com.sensing.core.coreviewer;

import org.junit.Test;

import static org.junit.Assert.*;

public class SampleParserTest {
    //0x026666C842036914FF00003A
    byte[] rawData = { 2, 102, 102, -56, 66, 3, 105, 20, -1, 0x7f, (byte)0xff, (byte)0xff };
    double delta = 0.00001;


//    @Test
//    public void fourByte2intAdcCoreStack() {
//        int actual = SampleParser.fourByte2intAdcCoreStack(rawData[11], rawData[10] ,rawData[9], (byte)0x0);
//        int expected = -1;
//        assertEquals(actual, expected);
//    }

//    @Test
//    public void interpret24bitAsInt32() {
//        byte[] b = {0x7f, (byte)0xff, (byte)0xff};
//        int actual = SampleParser.interpret24bitAsInt32(b);
//        Log.d("bla", "interpret24bitAsInt32: " + Integer.toBinaryString(actual));
//        int expected = 123;
//        assertEquals(actual, expected);
//        Log.d("bla", "interpret24bitAsInt32: " + Integer.toBinaryString(actual));
//    }

    @Test
    public void threeByte2int() {
        byte[] b = { (byte)0xFF, (byte)0x95, (byte)0x71};
        int actual = SampleParser.threeByte2intAdcCoreStack(b[2], b[1], b[0]);

        int expected = -27279;
        assertEquals(expected, actual);

//        byte[] b = { (byte)0xFF, (byte)0xE4, (byte)0xDE};
//        int actual = SampleParser.threeByte2intAdcCoreStack(b[2], b[1], b[0]);
//
//        int expected = 1680;
//        assertEquals(expected, actual);
    }
}
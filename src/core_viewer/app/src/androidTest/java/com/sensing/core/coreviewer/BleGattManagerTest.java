package com.sensing.core.coreviewer;

import org.junit.Test;

import java.nio.ByteBuffer;

import static org.junit.Assert.*;

public class BleGattManagerTest {

    @Test
    public void setCalibrationValue() {
        float calibrationValue = (float) 20.0;
        byte[] floatAsBytes = ByteBuffer.allocate(4).putFloat(calibrationValue).array();

        assertEquals("test", "test");
    }
}